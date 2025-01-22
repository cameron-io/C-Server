#include <string.h>
#include <stdio.h>
#include "response_handler.hh"
#include "http_server.hh"

#define BSIZE 1024

void send_headers(int clientFd, std::string statusCode, std::string contentType, unsigned int contentLength);

void send_ok(
    int clientFd,
    std::string contentType,
    std::string data)
{
    send_headers(clientFd, "200 OK", contentType, data.length());
    send(clientFd, data.c_str(), data.length(), 0);
}

void send_no_content(int clientFd)
{
    send_headers(clientFd, "204 No Content", "text/plain", 0);
}

void send_bad_request(
    int clientFd,
    std::string data)
{
    send_headers(clientFd, "400 Bad Request", "text/plain", data.length());
    send(clientFd, data.c_str(), data.length(), 0);
}

void send_not_found(
    int clientFd,
    std::string data)
{
    send_headers(clientFd, "404 Not Found", "text/plain", data.length());
    send(clientFd, data.c_str(), data.length(), 0);
}

void send_file(
    int clientFd,
    FILE *fp,
    std::string contentType,
    size_t contentLength)
{
    send_headers(clientFd, "200 OK", contentType, contentLength);

    char buffer[BSIZE];
    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        send(clientFd, buffer, r, 0);
        r = fread(buffer, 1, BSIZE, fp);
    }
}

void send_headers(
    int clientFd,
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
    send(clientFd, buffer, strlen(buffer), 0);
}