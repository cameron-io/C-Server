#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <sys/epoll.h>
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
        this->epollFd = setupInstance();
        addEventStream();
    }

    ~EventManager()
    {
        close(epollFd);
    }

    void startEventLoop();

private:
    int setupInstance();
    void addEventStream();
};

#endif