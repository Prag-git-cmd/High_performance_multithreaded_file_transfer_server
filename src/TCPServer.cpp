#include "TCPServer.h"

#include <iostream>
#include <thread>
#include <cstring>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


TCPServer::TCPServer(int port, std::size_t numThreads)
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
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

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
              << port << "\n";

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


void TCPServer::handleClient(int clientSocket)
{
    std::cout << "Handling client on thread "
              << std::this_thread::get_id()
              << "\n";

    const char* response =
        "Hello from multithreaded server!\n";

    send(
        clientSocket,
        response,
        std::strlen(response),
        0);

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
