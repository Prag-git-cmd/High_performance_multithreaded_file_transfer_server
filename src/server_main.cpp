#include <iostream>
#include <cstdlib>
#include "TCPServer.h"

int main(int argc, char* argv[])
{
    std::size_t numThreads = 3;

    if (argc > 1)
    {
        numThreads = std::stoul(argv[1]);
    }

    if (numThreads == 0)
    {
        std::cerr << "Number of threads must be greater than 0\n";
        return 1;
    }

    std::cout << "Starting server with "
              << numThreads
              << " worker threads\n";

    TCPServer server(8080, numThreads);

    if (!server.start())
    {
        std::cerr << "Failed to start server\n";
        return 1;
    }

    server.run();

    return 0;
}
