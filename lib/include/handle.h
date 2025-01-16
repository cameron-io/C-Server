#ifndef WEB_HANDLE_H
#define WEB_HANDLE_H

#include "server.h"
#include "core.h"

void handle_requests(struct client_info *client_list, fd_set* reads);

#endif