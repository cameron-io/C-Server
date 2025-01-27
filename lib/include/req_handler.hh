#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>
#include "core.hh"

int HandleRequest(SOCKET clientFd, std::string request);

#endif