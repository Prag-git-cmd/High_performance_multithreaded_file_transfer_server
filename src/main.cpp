#include <iostream>
#include <string>

#include "TCPClient.h"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage:\n";
        std::cout << "  ./client upload <file>\n";
        std::cout << "  ./client download <remote_file> <output_file>\n";

        return 1;
    }

    std::string operation = argv[1];

    TCPClient client("127.0.0.1", 8080);

    if (!client.connectToServer())
    {
        return 1;
    }

    if (operation == "upload")
    {
        std::string filePath = argv[2];

        if (!client.uploadFile(filePath))
        {
            std::cerr << "Upload failed\n";
            return 1;
        }

        std::string response;

        if (client.receiveMessage(response))
        {
            std::cout << "Server response: "
                      << response
                      << "\n";
        }
    }
    else if (operation == "download")
    {
        if (argc < 4)
        {
            std::cerr
                << "Download requires output file\n";

            return 1;
        }

        std::string remoteFile = argv[2];
        std::string outputFile = argv[3];

        if (!client.downloadFile(
                remoteFile,
                outputFile))
        {
            std::cerr << "Download failed\n";
            return 1;
        }
    }
    else
    {
        std::cerr << "Unknown operation: "
                  << operation
                  << "\n";

        return 1;
    }

    return 0;
}
