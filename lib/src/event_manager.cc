#include <stdexcept>
#include <thread>
#include <iostream>
#include <memory>
#include "event_manager.hh"
#include "http_server.hh"
#include "http_client.hh"

int EventManager::setupInstance()
{
    int epollFd = epoll_create1(0);
    if (epollFd == -1)
    {
        throw std::runtime_error("Failed to create epoll instance.");
    }
    return epollFd;
}

void EventManager::addEventStream()
{
    event.events = EPOLLIN;
    event.data.fd = server->serverFd;
    if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, server->serverFd, &event) == -1)
    {
        throw std::runtime_error("Failed to add server to epoll instance.");
    }
}

void EventManager::startEventLoop()
{
    while (true)
    {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1)
        {
            std::cerr << "Failed to wait for events." << std::endl;
            break;
        }

        for (int i = 0; i < numEvents; ++i)
        {
            int clientFd;
            if (events[i].data.fd == server->serverFd)
            {
                // Accept new client connection
                clientFd = server->acceptConnection();
                if (clientFd == -1)
                {
                    std::cerr << "Failed to accept client connection." << std::endl;
                    continue;
                }

                // Add client socket to epoll
                event.events = EPOLLIN;
                event.data.fd = clientFd;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1)
                {
                    std::cerr << "Failed to add client socket to epoll instance." << std::endl;
                    close(clientFd);
                    continue;
                }
            }
            else
            {
                // Handle client data
                clientFd = events[i].data.fd;
            }

            // Create a new thread to handle the client connection
            std::thread clientThread(
                [clientFd]()
                {
                    std::unique_ptr<Client> cl = std::make_unique<Client>(clientFd);
                    cl->handleClient();
                });
            clientThread.detach();
        }
    }
}