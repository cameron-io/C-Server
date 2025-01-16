#ifndef WEB_RESPONSES_H
#define WEB_RESPONSES_H

#include "core.h"

void send_400(struct client_info **client_list,
        struct client_info *client);

void send_404(struct client_info **client_list,
        struct client_info *client);

#endif