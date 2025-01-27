#include "res_content.hh"

#define BSIZE 1024

std::string ok(
    std::string contentType,
    std::string data)
{
    std::string headers = set_headers("200 ok", contentType, data.length());
    return headers + data;
}

std::string no_content()
{
    return set_headers("204 No Content", "text/plain", 0);
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

std::string read_file(
    FILE *fp)
{
    char buffer[BSIZE] = {0};
    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        r = fread(buffer + r, 1, BSIZE, fp);
    }
    return buffer;
}

std::string set_headers(
    std::string statusCode,
    std::string contentType,
    unsigned int contentLength)
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

    char buffer[BSIZE];
    snprintf(buffer, BSIZE, headers,
             statusCode.c_str(),
             contentLength,
             contentType.c_str());
    return buffer;
}