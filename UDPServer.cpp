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


int main(){
    

    return 0;
}