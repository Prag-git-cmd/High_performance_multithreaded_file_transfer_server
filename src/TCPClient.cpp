#include "TCPClient.h"

#include <iostream>
#include <cstring>
#include <fstream>

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
bool TCPClient::uploadFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "Failed to open file: "
                  << filePath << "\n";

        return false;
    }

    file.seekg(0, std::ios::end);

    std::streamsize fileSize = file.tellg();

    file.seekg(0, std::ios::beg);

    std::string command = "UPLOAD";

    sendMessage(command);

    std::string fileName = filePath;

    std::size_t position = filePath.find_last_of("/\\");

    if (position != std::string::npos)
    {
        fileName = filePath.substr(position + 1);
    }

    sendMessage(fileName);

    std::string sizeMessage = std::to_string(fileSize);

    sendMessage(sizeMessage);

    char buffer[4096];

    std::streamsize totalSent = 0;

    while (file)
    {
        file.read(buffer, sizeof(buffer));

        std::streamsize bytesRead = file.gcount();

        if (bytesRead <= 0)
        {
            break;
        }

        std::streamsize bytesSent = 0;

        while (bytesSent < bytesRead)
        {
            ssize_t result = send(
                clientSocket,
                buffer + bytesSent,
                bytesRead - bytesSent,
                0);

            if (result <= 0)
            {
                std::cerr << "Failed to send file data\n";
                return false;
            }

            bytesSent += result;
            totalSent += result;
        }
    }

    std::cout << "Uploaded "
              << totalSent
              << " bytes\n";

    return true;
}
