#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {
    const char* serverIp = "127.0.0.1";
    const int serverPort = 1200;
    const int bufferSize = 2048;

    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0) {
        cerr << "Failed to create socket." << endl;
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIp, &serverAddr.sin_addr);

    while (true) {
        string userInput;
        cout << "Enter a request or type 'exit' to close the connection: ";
        getline(cin, userInput);

        sendto(clientSocket, userInput.c_str(), userInput.size(), 0,
               (sockaddr*)&serverAddr, sizeof(serverAddr));

        if (userInput == "exit") {
            break;
        }

        char buffer[bufferSize];
        sockaddr_in fromAddr = {};
        socklen_t fromAddrLen = sizeof(fromAddr);

        int receivedBytes = recvfrom(clientSocket, buffer, bufferSize - 1, 0,
                                     (sockaddr*)&fromAddr, &fromAddrLen);
        if (receivedBytes < 0) {
            cerr << "Failed to receive data." << endl;
            continue;
        }

        buffer[receivedBytes] = '\0';
        cout << "Data received from server: " << buffer << endl;
    }

    close(clientSocket);
    return 0;
}
