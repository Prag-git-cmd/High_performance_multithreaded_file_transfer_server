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

    if (!client.downloadFile(
            "sample.txt",
            "downloaded_sample.txt"))
    {
        std::cerr << "Download failed\n";
        return 1;
    }

    return 0;
}
