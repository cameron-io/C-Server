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
#include "server_callback.hh"
#include "req_data.hh"
#include "req_callback.hh"

#define DEFAULT_PORT 8080

static uv_tcp_t server;

int main()
{
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