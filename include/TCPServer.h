#ifndef TCP_SERVER_H
#define TCP_SERVER_H

class TCPServer
{
public:
    TCPServer(int port);
    ~TCPServer();

    bool start();
    void run();

private:
    int serverSocket;
    int port;

    bool createSocket();
    bool bindSocket();
    bool listenForConnections();
};

#endif
