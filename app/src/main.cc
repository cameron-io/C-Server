#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <vector>
#include "http_server.hh"
#include "http_client.hh"
#include "request_handler.hh"

constexpr int MAX_EVENTS = 10;

int main() {
    struct epoll_event event, events[MAX_EVENTS];

    auto httpServer = new HttpServer();

    // Create socket
    int serverFd = httpServer->createSocket();

    // Bind socket to address and port
    httpServer->bindSocket();

    // Listen for incoming connections
    httpServer->listenSocket();

    // Create epoll instance
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        std::cerr << "Failed to create epoll instance." << std::endl;
        close(serverFd);
        return 1;
    }

    // Add server socket to epoll
    event.events = EPOLLIN;
    event.data.fd = serverFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &event) == -1) {
        std::cerr << "Failed to add server socket to epoll instance." << std::endl;
        close(serverFd);
        close(epollFd);
        return 1;
    }

    std::cout << "Server started. Listening on port " << PORT << std::endl;

    while (true) {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1) {
            std::cerr << "Failed to wait for events." << std::endl;
            break;
        }

        for (int i = 0; i < numEvents; ++i) {
            if (events[i].data.fd == serverFd) {
                // Accept new client connection
                int clientFd = httpServer->acceptConnection();
                if (clientFd == -1) {
                    std::cerr << "Failed to accept client connection." << std::endl;
                    continue;
                }

                // Add client socket to epoll
                event.events = EPOLLIN;
                event.data.fd = clientFd;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
                    std::cerr << "Failed to add client socket to epoll instance." << std::endl;
                    close(clientFd);
                    continue;
                }

                // Create a new thread to handle the client connection
                Client* cl = new Client(clientFd);
                std::thread clientThread([&cl]() { cl->handleClient(); });
                clientThread.detach();
            } else {
                // Handle client data
                int clientFd = events[i].data.fd;
                Client* cl = new Client(clientFd);
                std::thread clientThread([&cl]() { cl->handleClient(); });
                clientThread.detach();
            }
        }
    }

    close(serverFd);
    close(epollFd);
    return 0;
}