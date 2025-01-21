#include <string.h>
#include <stdio.h>
#include "response_handler.hh"
#include "http_server.hh"

#define BSIZE 1024

void ResponseHandler::sendHeaders(
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

void ResponseHandler::sendOk(
    int clientFd,
    std::string contentType,
    std::string data)
{
    sendHeaders(clientFd, "200 OK", contentType, data.length());
    send(clientFd, data.c_str(), data.length(), 0);
}

void ResponseHandler::sendNoContent(int clientFd)
{
    sendHeaders(clientFd, "204 No Content", "text/plain", 0);
}

void ResponseHandler::sendBadRequest(
    int clientFd,
    std::string data)
{
    sendHeaders(clientFd, "400 Bad Request", "text/plain", data.length());
    send(clientFd, data.c_str(), data.length(), 0);
}

void ResponseHandler::sendNotFound(
    int clientFd,
    std::string data)
{
    sendHeaders(clientFd, "404 Not Found", "text/plain", data.length());
    send(clientFd, data.c_str(), data.length(), 0);
}

void ResponseHandler::sendFile(
    int clientFd,
    FILE *fp,
    std::string contentType,
    size_t contentLength)
{
    sendHeaders(clientFd, "200 OK", contentType, contentLength);

    char buffer[BSIZE];
    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        send(clientFd, buffer, r, 0);
        r = fread(buffer, 1, BSIZE, fp);
    }
}