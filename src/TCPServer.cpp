#include "TCPServer.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include <cstdint>
#include <algorithm>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


TCPServer::TCPServer(
    int port,
    std::size_t numThreads)
    : serverSocket(-1),
      port(port),
      threadPool(numThreads)
{
}


TCPServer::~TCPServer()
{
    if (serverSocket != -1)
    {
        close(serverSocket);
    }
}


bool TCPServer::createSocket()
{
    serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0);

    if (serverSocket == -1)
    {
        std::cerr << "Failed to create socket\n";
        return false;
    }

    std::cout << "Socket created successfully\n";

    return true;
}


bool TCPServer::bindSocket()
{
    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)) == -1)
    {
        std::cerr << "Bind failed\n";
        return false;
    }

    std::cout << "Socket bound to port "
              << port
              << "\n";

    return true;
}


bool TCPServer::listenForConnections()
{
    if (listen(serverSocket, 10) == -1)
    {
        std::cerr << "Listen failed\n";
        return false;
    }

    std::cout << "Server is listening...\n";

    return true;
}


bool TCPServer::start()
{
    if (!createSocket())
    {
        return false;
    }

    if (!bindSocket())
    {
        return false;
    }

    if (!listenForConnections())
    {
        return false;
    }

    return true;
}


bool TCPServer::receiveAll(
    int clientSocket,
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


bool TCPServer::sendAll(
    int clientSocket,
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


bool TCPServer::receiveString(
    int clientSocket,
    std::string& data)
{
    std::uint64_t networkSize = 0;

    if (!receiveAll(
            clientSocket,
            reinterpret_cast<char*>(&networkSize),
            sizeof(networkSize)))
    {
        return false;
    }

    std::uint64_t size =
        be64toh(networkSize);

    data.resize(size);

    return receiveAll(
        clientSocket,
        data.data(),
        size);
}


bool TCPServer::receiveFile(
    int clientSocket)
{
    std::string command;

    if (!receiveString(
            clientSocket,
            command))
    {
        return false;
    }

    if (command != "UPLOAD")
    {
        std::cerr << "Unknown command: "
                  << command
                  << "\n";

        return false;
    }

    std::string fileName;

    if (!receiveString(
            clientSocket,
            fileName))
    {
        return false;
    }

    std::uint64_t networkFileSize = 0;

    if (!receiveAll(
            clientSocket,
            reinterpret_cast<char*>(&networkFileSize),
            sizeof(networkFileSize)))
    {
        return false;
    }

    std::uint64_t fileSize =
        be64toh(networkFileSize);

    std::ofstream outputFile(
        "received_" + fileName,
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

    std::cout << "Received file: "
              << fileName
              << " ("
              << totalReceived
              << " bytes)\n";

    return totalReceived == fileSize;
}


void TCPServer::handleClient(
    int clientSocket)
{
    std::cout << "Handling client on thread "
              << std::this_thread::get_id()
              << "\n";

    std::string command;

    if (!receiveString(
            clientSocket,
            command))
    {
        close(clientSocket);
        return;
    }

    if (command == "UPLOAD")
    {
        /*
         * The command has already been received,
         * so receiveFile() needs to continue
         * with the remaining upload data.
         */
        std::string fileName;

        if (!receiveString(
                clientSocket,
                fileName))
        {
            close(clientSocket);
            return;
        }

        std::uint64_t networkFileSize = 0;

        if (!receiveAll(
                clientSocket,
                reinterpret_cast<char*>(&networkFileSize),
                sizeof(networkFileSize)))
        {
            close(clientSocket);
            return;
        }

        std::uint64_t fileSize =
            be64toh(networkFileSize);

        std::ofstream outputFile(
            "received_" + fileName,
            std::ios::binary);

        if (!outputFile)
        {
            close(clientSocket);
            return;
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
                close(clientSocket);
                return;
            }

            outputFile.write(
                buffer,
                bytesReceived);

            totalReceived += bytesReceived;
        }

        outputFile.close();

        std::cout << "Received file: "
                  << fileName
                  << " ("
                  << totalReceived
                  << " bytes)\n";

        const char* response =
            "UPLOAD SUCCESS";

        sendAll(
            clientSocket,
            response,
            std::strlen(response));
    }
    else if (command == "DOWNLOAD")
    {
        std::string fileName;

        if (!receiveString(
                clientSocket,
                fileName))
        {
            close(clientSocket);
            return;
        }

        sendFile(
            clientSocket,
            fileName);
    }

    close(clientSocket);
}


void TCPServer::run()
{
    while (true)
    {
        sockaddr_in clientAddress{};

        socklen_t clientLength =
            sizeof(clientAddress);

        int clientSocket = accept(
            serverSocket,
            reinterpret_cast<sockaddr*>(&clientAddress),
            &clientLength);

        if (clientSocket == -1)
        {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "Client connected\n";

        threadPool.enqueue(
            [this, clientSocket]()
            {
                handleClient(clientSocket);
            });
    }
}

bool TCPServer::sendFile(
    int clientSocket,
    const std::string& fileName)
{
    std::ifstream file(
        fileName,
        std::ios::binary);

    if (!file)
    {
        std::cerr << "File not found: "
                  << fileName
                  << "\n";

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

    std::uint64_t networkFileSize =
        htobe64(fileSize);

    if (!sendAll(
            clientSocket,
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
                clientSocket,
                buffer,
                bytesRead))
        {
            return false;
        }

        totalSent += bytesRead;
    }

    std::cout << "Sent file: "
              << fileName
              << " ("
              << totalSent
              << " bytes)\n";

    return totalSent == fileSize;
}
