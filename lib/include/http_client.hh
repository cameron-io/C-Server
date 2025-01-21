#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <thread>
#include "http_server.hh"
#include "request_handler.hh"

class Client
{
public:
    Client(int clientFd);

    ~Client()
    {
        CLOSESOCKET(clientFd);
    }

    void handleClient();

private:
    int clientFd;
};

#endif