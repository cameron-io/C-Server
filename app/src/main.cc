#include <memory>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "http_server.hh"
#include "event_manager.hh"
#include "request_handler.hh"
#include "response_handler.hh"

void int_handler(int sig)
{
    char c;

    signal(sig, SIG_IGN);
    printf("\nDo you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y')
    {
        printf("Shutting down...");
        http_server_stop();

#if defined(_WIN32)
        WSACleanup();
#endif

        printf("OK\n");

        exit(0);
    }
    else
    {
        signal(SIGINT, int_handler);
    }
    getchar();
}

int main()
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = int_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    SOCKET serverFd = http_server_init();

    std::unique_ptr<EventManager> eventManager =
        std::make_unique<EventManager>(serverFd);

    std::cout << "Listening on port: " << PORT << std::endl;
    eventManager->startEventLoop();

    return 0;
}