#include <memory>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http_server.hh"
#include "event_manager.hh"
#include "request_handler.hh"
#include "response_handler.hh"

int main()
{

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    std::shared_ptr<HttpServer> httpServer = std::make_shared<HttpServer>();

    std::unique_ptr<EventManager> eventManager = std::make_unique<EventManager>(httpServer);

    std::cout << "Listening on port: " << PORT << std::endl;
    eventManager->startEventLoop();

#if defined(_WIN32)
    WSACleanup();
#endif

    return 0;
}