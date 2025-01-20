#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "request_handler.hh"

class Client
{
public:
    Client(int clientFd);
    void handleClient();

private:
    int clientFd;
};

#endif