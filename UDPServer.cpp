#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <set>
#include <unordered_map>
#include <ctime>
#include <thread>
#include <dirent.h>

using namespace std;

set<string> clients;
unordered_map<string, time_t> clientLastActivity;
const int timeoutSeconds = 30;

void sendMessageToClient(int serverSocket, const string &message, sockaddr_in &clientAddr) {
    sendto(serverSocket, message.c_str(), message.size(), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
    cout << "Message sent to client: " << message << endl;
}

string clientToString(const sockaddr_in &clientAddr) {
    return inet_ntoa(clientAddr.sin_addr) + string(":") + to_string(ntohs(clientAddr.sin_port));
}
bool hasFullAccess(const sockaddr_in& clientAddr) {
    return ntohs(clientAddr.sin_port) == 1515;
}

void handleRead(int serverSocket, const string &filename, sockaddr_in &clientAddr) {
    ifstream inFile(filename);
    if (!inFile) {
        sendMessageToClient(serverSocket, "Error: Could not open the file: " + filename, clientAddr);
        return;
    }

    string line, fileContent;
    while (getline(inFile, line)) {
        fileContent += line + "\n";
    }
    inFile.close();

    if (!fileContent.empty()) {
        sendMessageToClient(serverSocket, fileContent, clientAddr);
    } else {
        sendMessageToClient(serverSocket, "The file is empty.", clientAddr);
    }
}

void handleWrite(int serverSocket, const string &data, sockaddr_in &clientAddr) {
    ofstream outFile("server_output.txt", ios::app);
    if (outFile.is_open()) {
        outFile << data << endl;
        outFile.close();
        sendMessageToClient(serverSocket, "Data written to file successfully.", clientAddr);
    } else {
        sendMessageToClient(serverSocket, "Failed to write data to file.", clientAddr);
    }
}

void handleExecute(int serverSocket, const string &command, sockaddr_in &clientAddr) {
    char buffer[128];
    string result;
    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe) {
        sendMessageToClient(serverSocket, "Failed to execute command.", clientAddr);
        return;
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    if (!result.empty()) {
        sendMessageToClient(serverSocket, result, clientAddr);
    } else {
        sendMessageToClient(serverSocket, "No output from command.", clientAddr);
    }
}

void handleReadDir(int serverSocket, const string &directory, sockaddr_in &clientAddr) {
    DIR* dir = opendir(directory.c_str());
    if (!dir) {
        sendMessageToClient(serverSocket, "Error: Could not open directory: " + directory, clientAddr);
        return;
    }

    struct dirent* entry;
    string dirContents = "Directory contents:\n";
    while ((entry = readdir(dir)) != nullptr) {
        dirContents += entry->d_name;
        dirContents += (entry->d_type == DT_DIR) ? "/" : "";  // Append '/' for directories
        dirContents += "\n";
    }
    closedir(dir);

    sendMessageToClient(serverSocket, dirContents, clientAddr);
}
void handleListClients(int serverSocket, sockaddr_in &clientAddr) {
    string clientList = "Connected clients:\n";
    for (const auto& client : clients) {
        clientList += client + "\n";
    }
    sendMessageToClient(serverSocket, clientList, clientAddr);
}

void disconnectInactiveClients(int serverSocket) {
    while (true) {
        time_t currentTime = time(nullptr);
        for (auto it = clients.begin(); it != clients.end();) {
            if (currentTime - clientLastActivity[*it] > timeoutSeconds) {
                sockaddr_in clientAddr = {};
                string ip = (*it).substr(0, (*it).find(':'));
                int port = stoi((*it).substr((*it).find(':') + 1));
                inet_pton(AF_INET, ip.c_str(), &clientAddr.sin_addr);
                clientAddr.sin_port = htons(port);
                cout << "Client " << *it << " disconnected due to inactivity." << endl;

                clientLastActivity.erase(*it);
                it = clients.erase(it);  // Remove the client from the set
            } else {
                ++it;
            }
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
}

string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t timeNow = chrono::system_clock::to_time_t(now);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&timeNow));
    return string(buffer);
}

void logRequest(const string &clientIP, const string &request) {
    string timestamp = getCurrentTimestamp();

    cout << "[" << timestamp << "] Client IP: " << clientIP << ", Request: " << request << endl;

    ofstream logFile("server_log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << "[" << timestamp << "] Client IP: " << clientIP << ", Request: " << request << endl;
        logFile.close();
    } else {
        cerr << "Error opening log file!" << endl;
    }
}

//MAIN
int main() {
    const int serverPort = 1200;
    const int bufferSize = 2048;

    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        cerr << "Failed to create socket." << endl;
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Binding failed." << endl;
        close(serverSocket);
        return 1;
    }

    cout << "Server started on port " << serverPort << endl;

    thread(disconnectInactiveClients, serverSocket).detach();  

    while (true) {
        char buffer[bufferSize];
        sockaddr_in clientAddr = {};
        socklen_t clientAddrLen = sizeof(clientAddr);

        int receivedBytes = recvfrom(serverSocket, buffer, bufferSize - 1, 0, (sockaddr*)&clientAddr, &clientAddrLen);
        if (receivedBytes < 0) {
            cerr << "Failed to receive data." << endl;
            continue;
        }

        buffer[receivedBytes] = '\0';
        string clientMessage(buffer);
        string clientInfo = clientToString(clientAddr);

        clientLastActivity[clientInfo] = time(nullptr);  

        if (clientMessage == "DISCONNECT") {
            clients.erase(clientInfo);
            clientLastActivity.erase(clientInfo);
            sendMessageToClient(serverSocket, "You have been disconnected.", clientAddr);
            cout << "Client " << clientInfo << " manually disconnected." << endl;
            continue;
        }

        if (clients.size() >= 4 && clients.find(clientInfo) == clients.end()) {
            sendMessageToClient(serverSocket, "Server is full", clientAddr);
            continue;
        }

        clients.insert(clientInfo);
        cout << "-------------------------------" << endl;
        cout << "Client connected from " << clientInfo << endl;
        cout << "Request from client: " << clientMessage << endl;

        
        if (hasFullAccess(clientAddr)) {
            if (clientMessage == "LIST") {
                handleListClients(serverSocket, clientAddr);
            } else if (clientMessage.substr(0, 4) == "READ") {
                string filename = clientMessage.substr(5);
                handleRead(serverSocket, filename, clientAddr);
            } else if (clientMessage.substr(0, 5) == "WRITE") {
                string data = clientMessage.substr(6);
                handleWrite(serverSocket, data, clientAddr);
            } else if (clientMessage.substr(0, 7) == "EXECUTE") {
                string command = clientMessage.substr(8);
                handleExecute(serverSocket, command, clientAddr);
            } else if (clientMessage.substr(0, 7) == "DIRREAD") {
                string directory = clientMessage.substr(8);
                handleReadDir(serverSocket, directory, clientAddr);
            } else {
                sendMessageToClient(serverSocket, "Unknown command.", clientAddr);
            }
        } else {
            if (clientMessage.substr(0, 4) == "READ") {
                string filename = clientMessage.substr(5);
                handleRead(serverSocket, filename, clientAddr);
            } else {
                sendMessageToClient(serverSocket, "You only have access to the READ command.", clientAddr);
            }
        }
        
         logRequest(clientInfo, clientMessage);
        
    }

    close(serverSocket);
    return 0;
}


