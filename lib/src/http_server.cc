#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "http_server.hh"

int Server::createSocket() {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        exit(1);
    }
    return serverFd;
}

void Server::bindSocket(int serverFd) {
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    if (bind(serverFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(serverFd);
        exit(1);
    }
}

void Server::listenSocket(int serverFd) {
    if (listen(serverFd, MAX_CLIENTS) == -1) {
        std::cerr << "Failed to listen." << std::endl;
        close(serverFd);
        exit(1);
    }
}

int Server::acceptConnection(int serverFd) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    return accept(serverFd, (struct sockaddr*)&clientAddress, &clientAddressLength);
}
