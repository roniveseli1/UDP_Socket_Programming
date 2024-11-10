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

    // Create a UDP socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0) {
        cerr << "Failed to create socket." << endl;
        return 1;
    }

    // Client address structure to bind to a specific IP and port
    sockaddr_in clientAddr = {};
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(1515); // Use a valid port, e.g., 6969
    clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Assign IP address 127.0.0.1

    // Bind the client socket to the specified IP and port
    if (bind(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) < 0) {
        cerr << "Binding failed!" << endl;
        close(clientSocket);
        return 1;
    }

    // Server address structure
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIp, &serverAddr.sin_addr); // Server IP address

    while (true)
    {
        


        
    }
    


    close(clientSocket);
    return 0;
}

