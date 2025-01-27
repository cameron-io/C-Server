#ifndef RESPONSE_CONTENT_H
#define RESPONSE_CONTENT_H

#include <string>

char *set_headers(
    const char *status_code,
    const char *content_type,
    unsigned int content_length);

char *ok(
    char *content_type,
    char *data);

char *no_content();

char *bad_request(
    const char *data);

char *not_found(
    const char *data);

char *read_file(
    FILE *fp);

#endif