#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "server.h"
#include "conn_core.h"
#include "conn_manager.h"
#include "req_handler.h"

int tcp_listener(char* port);
SOCKET new_tcp_socket(const char* host, const char *port);

Server* new_tcp_server() {
    Server* app = (Server*) malloc(sizeof(Server));
    app->listen = tcp_listener;
    return app;
}

int tcp_listener(char* port) {
    SOCKET server = new_tcp_socket(0, port);
    struct client_info *client_list = 0;

    while(1) {
        fd_set reads;
        /*
            Sets server SOCKET as FD
        */
        reads = wait_on_clients(&client_list, server);

        /*
            Check if these reads from select() are for Server SOCKET FD
        */
        if (FD_ISSET(server, &reads)) {
            struct client_info *client = get_client(&client_list, -1);

            /*
                Client SOCKET is set by accept()
            */
            client->socket = accept(server,
                    (struct sockaddr*) &(client->address),
                    &(client->address_length));

            if (!ISVALIDSOCKET(client->socket)) {
                fprintf(stderr, "accept() failed. (%d)\n", errno);
                return 1;
            }

            printf("New connection: %s\n", get_client_address(client));
            handle_clients(&client_list, &reads);
        }
    } //while(1)

    printf("\nClosing socket...\n");
    CLOSESOCKET(server);

    printf("Finished.\n");
    return 0;
}

SOCKET new_tcp_socket(const char* host, const char *port) {
    /*
        Configure local address
    */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(host, port, &hints, &bind_address);

    /*
        Create socket
    */
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        exit(1);
    }

    /*
        Bind socket
    */
    if (bind(socket_listen,
            bind_address->ai_addr,
            bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", errno);
        exit(1);
    }
    freeaddrinfo(bind_address);

    /*
        Listen
    */
    printf("TCP Server listening on port: %s...\n", port);
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", errno);
        exit(1);
    }

    return socket_listen;
}