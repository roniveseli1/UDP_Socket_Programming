#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Message.h"

class MessageHandler
{
public:
    static bool sendMessage(int socket, const Message& msg, const sockaddr_in& destAddr)
    {
        char buffer[sizeof(Message)];
        msg.serialize(buffer);

        ssize_t bytesSent = sendto(socket, buffer, sizeof(buffer), 0,
            (struct sockaddr*)&destAddr, sizeof(destAddr));

        if (bytesSent == -1)
        {
            perror("Error sending message");
            return false;
        }
        return true;
    }

    static bool receiveMessage(int socket, Message& msg, sockaddr_in& srcAddr)
    {
        char buffer[sizeof(Message)];
        socklen_t srcAddrLen = sizeof(srcAddr);

        ssize_t bytesReceived = recvfrom(socket, buffer, sizeof(buffer), 0,
            (struct sockaddr*)&srcAddr, &srcAddrLen);

        if (bytesReceived == -1)
        {
            perror("Error receiving message");
            return false;
        }
        else if (bytesReceived == 0)
        {
            std::cout << "No data received (connection closed)" << std::endl;
            return false;
        }

        msg = Message::deserialize(buffer);
        return true;
    }
};
