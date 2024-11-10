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

string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t timeNow = chrono::system_clock::to_time_t(now);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&timeNow));
    return string(buffer);
}

int main(){
    

    return 0;
}
