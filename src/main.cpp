#include <iostream>

#include "TCPServer.h"

int main()
{
    TCPServer server(8080);

    if (!server.start())
    {
        std::cerr << "Failed to start server\n";
        return 1;
    }

    server.run();

    return 0;
}
