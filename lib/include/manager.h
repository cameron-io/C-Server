#ifndef WEB_MANAGER_H
#define WEB_MANAGER_H

#include "server.h"
#include "core.h"

struct client_info *get_client(struct client_info **client_list,
        SOCKET s);

void drop_client(struct client_info **client_list,
        struct client_info *client);

const char *get_client_address(struct client_info *ci);

fd_set wait_on_clients(struct client_info **client_list, SOCKET server);

#endif