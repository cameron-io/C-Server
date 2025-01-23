#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>
#include "http_server.hh"

namespace ReqHandler
{
    int Handle(SOCKET clientFd, std::string request);
}

#endif