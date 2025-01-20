#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <iostream>

class ResponseHandler
{
public:
    static void sendOK(
        int clientFd,
        const char *contentType,
        const char *data);

    static void sendBadRequest(int clientFd, const char *data);

    static void sendNotFound(int clientFd);

    static void sendFile(
        int clientFd,
        FILE *fp,
        const char *contentType,
        size_t contentLength);

private:
    static void sendHeaders(int clientFd, const char *statusCode, const char *contentType, unsigned int contentLength);
};

#endif