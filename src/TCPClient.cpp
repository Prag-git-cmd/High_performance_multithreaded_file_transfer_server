#include "TCPClient.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


TCPClient::TCPClient(
    const std::string& serverIP,
    int port)
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
    clientSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0);

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
        std::cerr << "Invalid server IP\n";
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


bool TCPClient::sendAll(
    const char* data,
    std::size_t size)
{
    std::size_t totalSent = 0;

    while (totalSent < size)
    {
        ssize_t bytesSent = send(
            clientSocket,
            data + totalSent,
            size - totalSent,
            0);

        if (bytesSent <= 0)
        {
            return false;
        }

        totalSent += bytesSent;
    }

    return true;
}


bool TCPClient::receiveAll(
    char* data,
    std::size_t size)
{
    std::size_t totalReceived = 0;

    while (totalReceived < size)
    {
        ssize_t bytesReceived = recv(
            clientSocket,
            data + totalReceived,
            size - totalReceived,
            0);

        if (bytesReceived <= 0)
        {
            return false;
        }

        totalReceived += bytesReceived;
    }

    return true;
}


bool TCPClient::sendString(
    const std::string& data)
{
    std::uint64_t size = data.size();

    std::uint64_t networkSize =
        htobe64(size);

    if (!sendAll(
            reinterpret_cast<char*>(&networkSize),
            sizeof(networkSize)))
    {
        return false;
    }

    return sendAll(
        data.data(),
        data.size());
}


bool TCPClient::receiveString(
    std::string& data)
{
    std::uint64_t networkSize = 0;

    if (!receiveAll(
            reinterpret_cast<char*>(&networkSize),
            sizeof(networkSize)))
    {
        return false;
    }

    std::uint64_t size =
        be64toh(networkSize);

    data.resize(size);

    return receiveAll(
        data.data(),
        size);
}


bool TCPClient::uploadFile(
    const std::string& filePath)
{
    std::ifstream file(
        filePath,
        std::ios::binary);

    if (!file)
    {
        std::cerr << "Failed to open file\n";
        return false;
    }

    file.seekg(
        0,
        std::ios::end);

    std::uint64_t fileSize =
        file.tellg();

    file.seekg(
        0,
        std::ios::beg);

    std::string fileName =
        filePath;

    std::size_t position =
        filePath.find_last_of("/\\");

    if (position != std::string::npos)
    {
        fileName =
            filePath.substr(position + 1);
    }

    if (!sendString("UPLOAD"))
    {
        return false;
    }

    if (!sendString(fileName))
    {
        return false;
    }

    std::uint64_t networkFileSize =
        htobe64(fileSize);

    if (!sendAll(
            reinterpret_cast<char*>(&networkFileSize),
            sizeof(networkFileSize)))
    {
        return false;
    }

    char buffer[4096];

    std::uint64_t totalSent = 0;

    while (file)
    {
        file.read(
            buffer,
            sizeof(buffer));

        std::streamsize bytesRead =
            file.gcount();

        if (bytesRead <= 0)
        {
            break;
        }

        if (!sendAll(
                buffer,
                bytesRead))
        {
            return false;
        }

        totalSent += bytesRead;
    }

    std::cout << "Uploaded "
              << totalSent
              << " bytes\n";

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
        std::cerr << "Failed to receive server response\n";
        return false;
    }

    response.assign(buffer, bytesReceived);

    return true;
}

bool TCPClient::downloadFile(
    const std::string& fileName,
    const std::string& outputPath)
{
    if (!sendString("DOWNLOAD"))
    {
        return false;
    }

    if (!sendString(fileName))
    {
        return false;
    }

    std::uint64_t networkFileSize = 0;

    if (!receiveAll(
            reinterpret_cast<char*>(&networkFileSize),
            sizeof(networkFileSize)))
    {
        return false;
    }

    std::uint64_t fileSize =
        be64toh(networkFileSize);

    std::ofstream outputFile(
        outputPath,
        std::ios::binary);

    if (!outputFile)
    {
        std::cerr << "Failed to create output file\n";
        return false;
    }

    char buffer[4096];

    std::uint64_t totalReceived = 0;

    while (totalReceived < fileSize)
    {
        std::size_t bytesToReceive =
            std::min<std::uint64_t>(
                sizeof(buffer),
                fileSize - totalReceived);

        ssize_t bytesReceived = recv(
            clientSocket,
            buffer,
            bytesToReceive,
            0);

        if (bytesReceived <= 0)
        {
            return false;
        }

        outputFile.write(
            buffer,
            bytesReceived);

        totalReceived += bytesReceived;
    }

    outputFile.close();

    std::cout << "Downloaded "
              << totalReceived
              << " bytes\n";

    return totalReceived == fileSize;
}
