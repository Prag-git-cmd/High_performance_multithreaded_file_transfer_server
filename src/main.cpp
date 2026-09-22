#include <iostream>
#include <string>

#include "TCPClient.h"

int main()
{
    TCPClient client("127.0.0.1", 8080);

    if (!client.connectToServer())
    {
        return 1;
    }

    std::string message = "Hello from TCP client";

    if (!client.sendMessage(message))
    {
        return 1;
    }

    std::string response;

    if (client.receiveMessage(response))
    {
        std::cout << "Server response: "
                  << response
                  << std::endl;
    }

    return 0;
}
