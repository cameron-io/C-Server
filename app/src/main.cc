#include <memory>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "http_server.hh"
#include "event_manager.hh"
#include "request_handler.hh"
#include "response_handler.hh"

void IntHandler(int sig)
{
    char c;

    signal(sig, SIG_IGN);
    printf("\nDo you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y')
    {
        printf("Shutting down...\n");
        HttpServer::Stop();

#if defined(_WIN32)
        WSACleanup();
#endif

        printf("Finished.\n");

        exit(0);
    }
    signal(SIGINT, IntHandler);
    getchar();
}

int main()
{
#ifdef _WIN32
    signal(SIGINT, IntHandler);
#else
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = IntHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
#endif

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    SOCKET serverFd = HttpServer::Init();

    std::unique_ptr<EventManager> eventManager =
        std::make_unique<EventManager>(serverFd);

    printf("Listening on port: %d\n", PORT);
    eventManager->StartEventLoop();

    return 0;
}