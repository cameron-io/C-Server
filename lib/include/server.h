#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "conn_core.h"

typedef struct {
    int (*listen)();
    void (*stop)();
    SOCKET socket;
} Server;

Server* new_tcp_server(char* port);

#endif