#ifndef WEB_RESPONSES_H
#define WEB_RESPONSES_H

#include "conn_core.h"

void serve_resource(
    struct client_info **client_list,
    struct client_info *client,
    const char *path,
    const char *base_path
);

void send_400(struct client_info **client_list,
        struct client_info *client);

void send_404(struct client_info **client_list,
        struct client_info *client);

#endif