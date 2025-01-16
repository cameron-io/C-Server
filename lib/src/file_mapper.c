#include "file_mapper.h"
#include "core.h"
#include "manager.h"
#include "responses.h"
#include "content.h"

void serve_resource(
    struct client_info **client_list,
    struct client_info *client,
    const char *path,
    const char *base_path
) {
    printf("GET\t%s\t%s\n", get_client_address(client), path);

    if (strcmp(path, "/") == 0) path = "/index.html";

    if (strlen(path) > 100) {
        send_400(client_list, client);
        return;
    }

    if (strstr(path, "..")) {
        send_404(client_list, client);
        return;
    }

    char full_path[128];
    sprintf(full_path, "%s%s", base_path, path);

    FILE *fp = fopen(full_path, "rb");

    if (!fp) {
        send_404(client_list, client);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    size_t cl = ftell(fp);
    rewind(fp);

    const char *ct = get_content_type(full_path);

#define BSIZE 1024
    char buffer[BSIZE];

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Connection: close\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %u\r\n", (unsigned int) cl);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n", ct);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    int r = fread(buffer, 1, BSIZE, fp);
    while (r) {
        send(client->socket, buffer, r, 0);
        r = fread(buffer, 1, BSIZE, fp);
    }

    fclose(fp);
    drop_client(client_list, client);
}
