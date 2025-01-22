#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>
#include "http_server.hh"

void request_handler_handle(SOCKET clientFd, std::string r);

#endif