#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "res_content.hh"

std::string ok(
    std::string content_type,
    int content_length,
    std::string data)
{
    std::string headers = set_headers("200 OK", content_type, (unsigned int)content_length);
    return headers + data;
}

std::string no_content()
{
    std::string buffer = set_headers("204 No Content", "text/plain", 0);
    return buffer;
}

std::string bad_request(std::string data)
{
    std::string headers = set_headers("400 Bad Request", "text/plain", data.length());
    return headers + data;
}

std::string not_found(std::string data)
{
    std::string headers = set_headers("404 Not Found", "text/plain", data.length());
    return headers + data;
}

int read_file_contents(
    char *buffer,
    std::string full_path)
{
    FILE *fp = fopen(full_path.c_str(), "rb");
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

std::string set_headers(
    std::string status_code,
    std::string content_type,
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
             status_code.c_str(),
             content_length,
             content_type.c_str());
    return buffer;
}