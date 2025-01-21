#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#ifdef __linux__
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
#else
#include <memory>
#include "http_server.hh"

#define MAX_REQUEST_SIZE 2047

struct ClientInfo
{
    socklen_t addressLength;
    struct sockaddr_storage address;
    SOCKET socket;
    char request[MAX_REQUEST_SIZE + 1];
    int received;
    struct ClientInfo *next;
};

class EventManager
{
    std::shared_ptr<HttpServer> server;

public:
    EventManager(std::shared_ptr<HttpServer> httpServer)
        : server(httpServer)
    {
    }
    void startEventLoop();

private:
    struct ClientInfo *getClient(struct ClientInfo **clientList,
                                 SOCKET s);

    void dropClient(struct ClientInfo **clientList,
                    struct ClientInfo *client);

    fd_set waitOnClients(struct ClientInfo **clientList);
};
#endif

#endif