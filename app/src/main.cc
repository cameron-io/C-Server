#include <iostream>
#include <memory>
#include "http_server.hh"
#include "event_manager.hh"

int main()
{
    std::shared_ptr<HttpServer> httpServer = std::make_shared<HttpServer>();

    // Create epoll instance
    std::unique_ptr<EventManager> epoll = std::make_unique<EventManager>(httpServer);

    // Add server socket to epoll
    epoll->addEventStream();

    std::cout << "Listening on port: " << PORT << std::endl;
    epoll->startEventLoop();

    return 0;
}