#include "server.h"

int main() {
    Server* app = new_tcp_server();
    return app->listen("8080");
}
