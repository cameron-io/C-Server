#ifndef WEB_FILE_MAPPER_H
#define WEB_FILE_MAPPER_H

#include "core.h"

void serve_resource(
    struct client_info **client_list,
    struct client_info *client,
    const char *path,
    const char *base_path
);

#endif