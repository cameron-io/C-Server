#include "handler.h"
#include "core.h"
#include "manager.h"
#include "responses.h"
#include "content.h"
#include "file_mapper.h"

const char* base_path = "static";

void handle_requests(struct client_info *client_list, fd_set* reads) {
    struct client_info *client = client_list;

    while(client) {
        struct client_info *next = client->next;

        if (FD_ISSET(client->socket, reads)) {

            if (MAX_REQUEST_SIZE == client->received) {
                send_400(&client_list, client);
                client = next;
                continue;
            }

            int r = recv(client->socket,
                    client->request + client->received,
                    MAX_REQUEST_SIZE - client->received, 0);

            if (r < 1) {
                printf("Unexpected disconnect from %s.\n",
                        get_client_address(client));
                drop_client(&client_list, client);
                client = next;
                continue;
            }
            client->received += r;
            client->request[client->received] = 0;

            char *q = strstr(client->request, "\r\n\r\n");
            if (q) {
                *q = 0;

                if (strncmp("GET /", client->request, 5)) {
                    send_400(&client_list, client);
                } else {
                    char *path = client->request + 4;
                    char *end_path = strstr(path, " ");
                    if (!end_path) {
                        send_400(&client_list, client);
                    } else {
                        *end_path = 0;
                        serve_resource(&client_list, client, path, base_path);
                    }
                }
            } //if (q)
        }
        client = next;
    }
}
