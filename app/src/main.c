#include "server.h"
#include "content.h"
#include "socket.h"
#include "core.h"
#include "handle.h"
#include "manager.h"
#include "responses.h"

int main() {
    SOCKET server = create_socket(0, "8080");
    struct client_info *client_list = 0;

    while(1) {
        fd_set reads;
        reads = wait_on_clients(&client_list, server);

        if (FD_ISSET(server, &reads)) {
            struct client_info *client = get_client(&client_list, -1);

            client->socket = accept(server,
                    (struct sockaddr*) &(client->address),
                    &(client->address_length));

            if (!ISVALIDSOCKET(client->socket)) {
                fprintf(stderr, "accept() failed. (%d)\n",
                        GETSOCKETERRNO());
                return 1;
            }

            printf("New connection from %s.\n",
                    get_client_address(client));
            handle_requests(client_list, &reads);
        }
    } //while(1)

    printf("\nClosing socket...\n");
    CLOSESOCKET(server);

    printf("Finished.\n");
    return 0;
}
