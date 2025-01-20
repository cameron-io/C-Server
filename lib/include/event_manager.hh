#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <sys/epoll.h>
#include <stdexcept>
#include <memory>
#include "http_server.hh"

constexpr int MAX_EVENTS = 10;

class EventManager
{
    int epollFd;
    struct epoll_event event, events[MAX_EVENTS];
    std::shared_ptr<HttpServer> server;

public:
    EventManager(std::shared_ptr<HttpServer> httpServer)
        : server(httpServer)
    {
        int epollFd = epoll_create1(0);
        if (epollFd == -1)
        {
            throw std::runtime_error("Failed to create epoll instance.");
        }
        this->epollFd = epollFd;
    }

    ~EventManager()
    {
        close(epollFd);
    }

    void addEventStream();
    void startEventLoop();
};

#endif