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
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http_server.hh"
#include "request_handler.hh"
#include "response_handler.hh"

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

    struct ClientInfo *getClient(struct ClientInfo **clientList,
                                 SOCKET s)
    {
        struct ClientInfo *ci = *clientList;

        while (ci)
        {
            if (ci->socket == s)
                break;
            ci = ci->next;
        }

        if (ci)
            return ci;
        struct ClientInfo *n =
            (struct ClientInfo *)calloc(1, sizeof(struct ClientInfo));

        if (!n)
        {
            fprintf(stderr, "Out of memory.\n");
            exit(1);
        }

        n->addressLength = sizeof(n->address);
        n->next = *clientList;
        *clientList = n;
        return n;
    }

    void dropClient(struct ClientInfo **clientList,
                    struct ClientInfo *client)
    {
        CLOSESOCKET(client->socket);

        struct ClientInfo **p = clientList;

        while (*p)
        {
            if (*p == client)
            {
                *p = client->next;
                free(client);
                return;
            }
            p = &(*p)->next;
        }

        fprintf(stderr, "dropClient not found.\n");
        exit(1);
    }

    fd_set waitOnClients(struct ClientInfo **clientList)
    {
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(server->serverFd, &reads);
        SOCKET maxSocket = server->serverFd;

        struct ClientInfo *ci = *clientList;

        while (ci)
        {
            FD_SET(ci->socket, &reads);
            if (ci->socket > maxSocket)
                maxSocket = ci->socket;
            ci = ci->next;
        }

        if (select(maxSocket + 1, &reads, 0, 0, 0) < 0)
        {
            throw std::runtime_error("select() failed.");
        }

        return reads;
    }

    void startEventLoop()
    {
        struct ClientInfo *clientList = 0;

        while (true)
        {
            fd_set reads;
            reads = this->waitOnClients(&clientList);

            if (FD_ISSET(server->serverFd, &reads))
            {
                struct ClientInfo *client = this->getClient(&clientList, -1);

                client->socket = accept(server->serverFd,
                                        (struct sockaddr *)&(client->address),
                                        &(client->addressLength));

                if (!ISVALIDSOCKET(client->socket))
                {
                    throw std::runtime_error("accept() failed.");
                }
            }

            struct ClientInfo *client = clientList;
            while (client)
            {
                struct ClientInfo *next = client->next;

                if (FD_ISSET(client->socket, &reads))
                {
                    if (MAX_REQUEST_SIZE == client->received)
                    {
                        ResponseHandler::sendBadRequest(client->socket, "Request too large.");
                        client = next;
                        continue;
                    }

                    int r = recv(client->socket,
                                 client->request + client->received,
                                 MAX_REQUEST_SIZE - client->received, 0);

                    if (r < 1)
                    {
                        this->dropClient(&clientList, client);
                    }
                    else
                    {
                        client->received += r;
                        client->request[client->received] = 0;

                        char *q = strstr(client->request, "\r\n\r\n");
                        if (q)
                        {
                            *q = 0;
                            RequestHandler::handle(client->socket, client->request);
                            this->dropClient(&clientList, client);
                        }
                    }
                }
                client = next;
            }
        }
    }
};
#endif

#endif