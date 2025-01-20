#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <unistd.h>
#include <thread>
#include "request_handler.hh"

class Client
{
public:
    Client(int clientFd);
    ~Client() { close(clientFd); }
    void handleClient();

private:
    int clientFd;
};

#endif