#include "TCPClient.h"

#include <iostream>
#include <cstring>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


TCPClient::TCPClient(const std::string& serverIP, int port)
    : clientSocket(-1),
      serverIP(serverIP),
      port(port)
{
}


TCPClient::~TCPClient()
{
    if (clientSocket != -1)
    {
        close(clientSocket);
    }
}


bool TCPClient::connectToServer()
{
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == -1)
    {
        std::cerr << "Failed to create client socket\n";
        return false;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(
            AF_INET,
            serverIP.c_str(),
            &serverAddress.sin_addr) <= 0)
    {
        std::cerr << "Invalid server IP address\n";
        return false;
    }

    if (connect(
            clientSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to connect to server\n";
        return false;
    }

    std::cout << "Connected to server\n";

    return true;
}


bool TCPClient::sendMessage(const std::string& message)
{
    ssize_t bytesSent = send(
        clientSocket,
        message.c_str(),
        message.size(),
        0);

    if (bytesSent == -1)
    {
        std::cerr << "Failed to send message\n";
        return false;
    }

    return true;
}


bool TCPClient::receiveMessage(std::string& response)
{
    char buffer[4096];

    std::memset(buffer, 0, sizeof(buffer));

    ssize_t bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0);

    if (bytesReceived <= 0)
    {
        std::cerr << "Failed to receive response\n";
        return false;
    }

    response.assign(buffer, bytesReceived);

    return true;
}
