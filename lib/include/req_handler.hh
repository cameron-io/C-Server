#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>
#include "http_server.hh"

int HandleRequest(SOCKET clientFd, std::string request);

#endif