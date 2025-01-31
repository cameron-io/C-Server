#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#endif

#include <uv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Connection.hpp"

#define DEFAULT_PORT 8080
#define BACKLOG 128

static uv_tcp_t server;

void int_handler(int sig);

int main()
{
#ifdef _WIN32
    signal(SIGINT, int_handler);
#else
    struct sigaction sig_int_handler;
    sig_int_handler.sa_handler = int_handler;
    sig_int_handler.sa_flags = 0;
    sigemptyset(&sig_int_handler.sa_mask);

    sigaction(SIGINT, &sig_int_handler, NULL);
#endif

    loop = uv_default_loop();
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);

    // initialize the server
    uv_tcp_init(loop, &server);

    // bind the server to the address above
    uv_tcp_bind(&server, (struct sockaddr *)&addr, 0);

    printf("Listening on port: %d...\n", DEFAULT_PORT);

    int r = uv_listen((uv_stream_t *)&server, BACKLOG,
                      handle_client_cb);
    if (r)
        return fprintf(stderr, "Error on listening: %s.\n",
                       uv_strerror(r));

    // execute all tasks in queue
    return uv_run(loop, UV_RUN_DEFAULT);
}

void int_handler(int sig)
{
    char c;

    signal(sig, SIG_IGN);
    printf("\nDo you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y')
    {
        printf("Shutting down...\n");
        uv_stop(loop);

#if defined(_WIN32)
        WSACleanup();
#endif

        printf("Finished.\n");
        exit(0);
    }
    signal(SIGINT, int_handler);
    getchar();
}
