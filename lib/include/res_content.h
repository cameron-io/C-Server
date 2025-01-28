#ifndef RESPONSE_CONTENT_H
#define RESPONSE_CONTENT_H

#define BSIZE 1024

char *set_headers(
    const char *status_code,
    const char *content_type,
    unsigned int content_length);

char *ok(
    const char *content_type,
    const char *data);

char *no_content();

char *bad_request(const char *data);

char *not_found(const char *data);

int read_file_contents(
    char *destination,
    const char *full_path);

#endif