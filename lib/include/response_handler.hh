#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <iostream>
#include <string>

class ResponseHandler
{
public:
    static void sendNoContent(int clientFd);

    static void sendOk(
        int clientFd,
        std::string contentType,
        std::string data);

    static void sendBadRequest(int clientFd, std::string data);

    static void sendNotFound(int clientFd, std::string data);

    static void sendFile(
        int clientFd,
        FILE *fp,
        std::string contentType,
        size_t contentLength);

private:
    static void sendHeaders(int clientFd, std::string statusCode, std::string contentType, unsigned int contentLength);
};

#endif