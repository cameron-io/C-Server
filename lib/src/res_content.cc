#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "res_content.hh"

char *ok(
    const char *content_type,
    int content_length,
    const char *data)
{
    char *headers = set_headers("200 OK", content_type, (unsigned int)content_length);
    return strcat(headers, data);
}

char *no_content()
{
    char *buffer = (char *)malloc(BSIZE);
    buffer = set_headers("204 No Content", "text/plain", 0);
    return buffer;
}

char *bad_request(const char *data)
{
    char *headers = set_headers("400 Bad Request", "text/plain", strlen(data));
    return strcat(headers, data);
}

char *not_found(const char *data)
{
    char *headers = set_headers("404 Not Found", "text/plain", strlen(data));
    return strcat(headers, data);
}

int read_file_contents(
    char *buffer,
    const char *full_path)
{
    FILE *fp = fopen(full_path, "rb");
    if (!fp)
        return -1;

    fseek(fp, 0L, SEEK_END);
    size_t content_length = ftell(fp);
    rewind(fp);

    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        r = fread(buffer + r, 1, BSIZE, fp);
    }
    fclose(fp);

    return content_length;
}

char *set_headers(
    const char *status_code,
    const char *content_type,
    unsigned int content_length)
{
    const char *headers =
        "HTTP/1.1 %s\r\n"
        "Content-Length: %u\r\n"
        "Content-Type: %s\r\n"
        "Connection: close\r\n"
        // Default CORS-Headers
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, PUT, POST, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: *\r\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "Access-Control-Max-Age: 86400\r\n"
        "\r\n";

    char *buffer = (char *)malloc(BSIZE);
    snprintf(buffer, BSIZE, headers,
             status_code,
             content_length,
             content_type);
    return buffer;
}