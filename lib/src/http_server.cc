#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdexcept>
#include "http_server.hh"

int HttpServer::createSocket()
{
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1)
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

int HttpServer::acceptConnection()
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    return accept(this->serverFd, (struct sockaddr *)&clientAddress, &clientAddressLength);
}
