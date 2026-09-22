#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <cstddef>
#include <string>

#include "ThreadPool.h"

class TCPServer
{
public:
    TCPServer(int port, std::size_t numThreads);
    ~TCPServer();

    bool start();
    void run();

private:
    int serverSocket;
    int port;

    ThreadPool threadPool;

    bool createSocket();
    bool bindSocket();
    bool listenForConnections();

    void handleClient(int clientSocket);

    bool receiveFile(int clientSocket);
};

#endif
