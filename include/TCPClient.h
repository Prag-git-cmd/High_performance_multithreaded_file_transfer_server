#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>
#include <cstdint>

class TCPClient
{
public:
    TCPClient(const std::string& serverIP, int port);
    ~TCPClient();

    bool connectToServer();

    bool uploadFile(const std::string& filePath);

    bool receiveMessage(std::string& response);

private:
    int clientSocket;
    std::string serverIP;
    int port;

    bool sendAll(const char* data, std::size_t size);
    bool receiveAll(char* data, std::size_t size);

    bool sendString(const std::string& data);
    bool receiveString(std::string& data);
};

#endif
