#ifndef WEB_SERVER_H
#define WEB_SERVER_H

typedef struct {
    int (*listen)(char*);
} Server;

Server* new_tcp_server();

#endif