#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>

class TCPClient
{
public:
    TCPClient(const std::string& serverIP, int port);
    ~TCPClient();

    bool connectToServer();

    bool sendMessage(const std::string& message);
    bool receiveMessage(std::string& response);

    bool uploadFile(const std::string& filePath);

private:
    int clientSocket;
    std::string serverIP;
    int port;
};

#endif
