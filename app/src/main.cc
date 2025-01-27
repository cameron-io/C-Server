#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
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
#include "conn_handler.hh"
#include "req_handler.hh"
#include "req_data.hh"

#define DEFAULT_PORT 8080

static uv_tcp_t server;

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

int main()
{
#ifdef _WIN32
    signal(SIGINT, int_handler);
#else
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = int_handler;
    sigIntHandler.sa_flags = 0;
    sigemptyset(&sigIntHandler.sa_mask);

    sigaction(SIGINT, &sigIntHandler, NULL);
#endif

    loop = uv_default_loop();
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);
    /* initialize the server */
    uv_tcp_init(loop, &server);
    /* bind the server to the address above */
    uv_tcp_bind(&server, (struct sockaddr *)&addr, 0);
    printf("Listening on port: %d...\n", DEFAULT_PORT);
    int r = uv_listen((uv_stream_t *)&server, 128,
                      connection_cb);
    if (r)
    {
        return fprintf(stderr, "Error on listening: %s.\n",
                       uv_strerror(r));
    }
    /* execute all tasks in queue */
    return uv_run(loop, UV_RUN_DEFAULT);
}