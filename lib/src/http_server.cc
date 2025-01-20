#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "http_server.hh"

int HttpServer::createSocket() {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        exit(1);
    }
    this->serverFd = serverFd;
    return serverFd;
}

void HttpServer::bindSocket() {
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    if (bind(this->serverFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(this->serverFd);
        exit(1);
    }
}

void HttpServer::listenSocket() {
    if (listen(this->serverFd, MAX_CLIENTS) == -1) {
        std::cerr << "Failed to listen." << std::endl;
        close(serverFd);
        exit(1);
    }
}

int HttpServer::acceptConnection() {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    return accept(this->serverFd, (struct sockaddr*)&clientAddress, &clientAddressLength);
}
