#include <memory>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "cli.hh"
#include "http_server.hh"
#include "event_manager.hh"
#include "req_handler.hh"
#include "res_handler.hh"

int main()
{
#ifdef _WIN32
    signal(SIGINT, CLI::IntHandler);
#else
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = CLI::IntHandler;
    sigIntHandler.sa_flags = 0;
    sigemptyset(&sigIntHandler.sa_mask);

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