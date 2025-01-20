#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <iostream>

class ResponseHandler
{
public:
    static void sendBadRequest(int clientFd);

    static void sendNotFound(int clientFd);

    static void sendFile(
        int clientFd,
        FILE *fp,
        const char *contentType,
        size_t contentLength);
};

#endif