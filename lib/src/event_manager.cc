#include <sys/epoll.h>
#include <stdexcept>
#include <thread>
#include <iostream>
#include "event_manager.hh"
#include "http_server.hh"
#include "http_client.hh"

void EventManager::addEventStream()
{
    event.events = EPOLLIN;
    event.data.fd = server->serverFd;
    if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, server->serverFd, &event) == -1)
    {
        throw std::runtime_error("Failed to create epoll instance.");
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
            if (events[i].data.fd == server->serverFd)
            {
                // Accept new client connection
                int clientFd = server->acceptConnection();
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

                // Create a new thread to handle the client connection
                Client *cl = new Client(clientFd);
                std::thread clientThread([&cl]()
                                         { cl->handleClient(); });
                clientThread.detach();
            }
            else
            {
                // Handle client data
                int clientFd = events[i].data.fd;
                Client *cl = new Client(clientFd);
                std::thread clientThread([&cl]()
                                         { cl->handleClient(); });
                clientThread.detach();
            }
        }
    }
}