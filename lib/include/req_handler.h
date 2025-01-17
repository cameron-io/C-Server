#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include "conn_core.h"

void handle_clients(struct client_info **client_list, fd_set* reads);

#endif