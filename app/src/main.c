#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "server.h"

Server* app;

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\nClosing socket...\n");
        CLOSESOCKET(app->socket);

        printf("Finished.\n");
        exit(0);
    }
}

int main() {
    signal(SIGINT, signal_handler);
    app = new_tcp_server("8080");
    app->listen(app->socket);
}
