#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <iostream>
#include <string>
#include "core.hh"

int SendOK(
    SOCKET clientFd,
    std::string contentType,
    std::string data);

int SendNoContent(SOCKET clientFd);

int SendBadRequest(
    SOCKET clientFd,
    std::string data);

int SendNotFound(
    SOCKET clientFd,
    std::string data);

int SendFile(
    SOCKET clientFd,
    FILE *fp,
    std::string contentType,
    size_t contentLength);

#endif