#include <unistd.h>
#include "http_client.hh"
#include "http_server.hh"
#include "request_handler.hh"

Client::Client(int clientFd)
{
    this->clientFd = clientFd;
}

void Client::handleClient()
{
    char input_buffer[1024];

    while (true)
    {
        int bytesRead = read(clientFd, input_buffer, sizeof(input_buffer));
        if (bytesRead <= 0)
        {
            break;
        }
        RequestHandler::handle(clientFd, input_buffer);
    }

    CLOSESOCKET(clientFd);
}