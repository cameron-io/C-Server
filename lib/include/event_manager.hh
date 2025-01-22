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
        this->epollFd = SetupInstance();
        this->serverFd = serverSocket;
        AddEventStream();
    }

    ~EventManager()
    {
        CLOSESOCKET(epollFd);
        HttpServer::Stop();
    }

    void StartEventLoop();

private:
    int SetupInstance();
    void AddEventStream();
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
    void StartEventLoop();

private:
    struct ClientInfo *GetClient(struct ClientInfo **clientList);

    void DropClient(struct ClientInfo **clientList,
                    struct ClientInfo *client);

    fd_set WaitOnClients(struct ClientInfo **clientList);
};
#endif

#endif