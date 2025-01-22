#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <iostream>
#include <string>

namespace ResHandler
{

    void SendOK(
        int clientFd,
        std::string contentType,
        std::string data);

    void SendNoContent(int clientFd);

    void SendBadRequest(
        int clientFd,
        std::string data);

    void SendNotFound(
        int clientFd,
        std::string data);

    void SendFile(
        int clientFd,
        FILE *fp,
        std::string contentType,
        size_t contentLength);
}

#endif