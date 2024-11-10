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


int main(){
    

    return 0;
}