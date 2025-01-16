#include <stdlib.h>
#include "server.h"
#include "socket.h"
#include "core.h"
#include "manager.h"
#include "handler.h"

int main() {
    Server* app = new_tcp_server();
    return app->listen("8080");
}
