#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <iostream>
#include <string>

namespace ResHandler
{

    int SendOK(
        int clientFd,
        std::string contentType,
        std::string data);

    int SendNoContent(int clientFd);

    int SendBadRequest(
        int clientFd,
        std::string data);

    int SendNotFound(
        int clientFd,
        std::string data);

    int SendFile(
        int clientFd,
        FILE *fp,
        std::string contentType,
        size_t contentLength);
}

#endif