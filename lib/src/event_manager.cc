#ifdef __linux__

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
#else
#include <stdexcept>
#include "event_manager.hh"
#include "http_server.hh"
#include "request_handler.hh"
#include "response_handler.hh"

void EventManager::startEventLoop()
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

fd_set EventManager::waitOnClients(struct ClientInfo **clientList)
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

struct ClientInfo *EventManager::getClient(struct ClientInfo **clientList,
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
        throw std::runtime_error("Out of memory.");
    }

    n->addressLength = sizeof(n->address);
    n->next = *clientList;
    *clientList = n;
    return n;
}

void EventManager::dropClient(struct ClientInfo **clientList,
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

    throw std::runtime_error("dropClient not found.");
}

#endif