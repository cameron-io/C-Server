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
    SOCKET serverFd;

public:
    EventManager(SOCKET serverSocket)
    {
        this->epollFd = setupInstance();
        this->serverFd = serverSocket;
        addEventStream();
    }

    ~EventManager()
    {
        CLOSESOCKET(epollFd);
        http_server_stop();
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
    SOCKET socket;
    char request[MAX_REQUEST_SIZE + 1];
    int received;
    struct ClientInfo *next;
};

class EventManager
{
    SOCKET serverFd;

public:
    EventManager(SOCKET serverSocket)
    {
        this->serverFd = serverSocket;
    }
    void startEventLoop();

private:
    struct ClientInfo *getClient(struct ClientInfo **clientList);

    void dropClient(struct ClientInfo **clientList,
                    struct ClientInfo *client);

    fd_set waitOnClients(struct ClientInfo **clientList);
};
#endif

#endif