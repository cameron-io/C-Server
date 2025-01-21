#include <iostream>
#include <string.h>
#include <stdexcept>
#include "http_server.hh"
#include "request_handler.hh"

SOCKET HttpServer::createSocket()
{
    SOCKET serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (!ISVALIDSOCKET(serverFd))
    {
        throw std::runtime_error("Failed to create socket.");
    }
    this->serverFd = serverFd;
    return serverFd;
}

void HttpServer::bindSocket()
{
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    if (bind(this->serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        throw std::runtime_error("Failed to bind socket.");
    }
}

void HttpServer::listenSocket()
{
    if (listen(this->serverFd, MAX_CLIENTS) == -1)
    {
        throw std::runtime_error("Failed to listen.");
    }
}

SOCKET HttpServer::acceptConnection()
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    return accept(this->serverFd, (struct sockaddr *)&clientAddress, &clientAddressLength);
}

void HttpServer::readRequest(SOCKET clientFd)
{
    char input_buffer[1024];

    while (true)
    {
        int bytesRead = recv(clientFd, input_buffer, sizeof(input_buffer), 0);
        if (bytesRead <= 0)
        {
            break;
        }
        RequestHandler::handle(clientFd, input_buffer);
    }

    CLOSESOCKET(clientFd);
}
