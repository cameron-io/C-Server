#include <unistd.h>
#include <string.h>
#include <iostream>
#include "response_handler.hh"

#define BSIZE 1024

void ResponseHandler::sendHeaders(
    int clientFd,
    const char *statusCode,
    const char *contentType,
    unsigned int contentLength)
{
    char buffer[BSIZE];

    sprintf(buffer, "HTTP/1.1 %s\r\n", statusCode);
    write(clientFd, buffer, strlen(buffer));

    // Default CORS-Headers
    sprintf(buffer, "Access-Control-Allow-Origin: *\r\n");
    write(clientFd, buffer, strlen(buffer));
    sprintf(buffer, "Access-Control-Allow-Methods: GET, PUT, POST, DELETE, OPTIONS\r\n");
    write(clientFd, buffer, strlen(buffer));
    sprintf(buffer, "Access-Control-Allow-Headers: *\r\n");
    write(clientFd, buffer, strlen(buffer));
    sprintf(buffer, "Access-Control-Allow-Credentials: true\r\n");
    write(clientFd, buffer, strlen(buffer));
    sprintf(buffer, "Access-Control-Max-Age: 86400\r\n");
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Content-Length: %u\r\n", contentLength);
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Content-Type: %s\r\n", contentType);
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Connection: close\r\n");
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "\r\n");
    write(clientFd, buffer, strlen(buffer));
}

void ResponseHandler::sendOk(
    int clientFd,
    const char *contentType,
    const char *data)
{
    sendHeaders(clientFd, "200 OK", contentType, strlen(data));
    write(clientFd, data, strlen(data));
}

void ResponseHandler::sendNoContent(int clientFd)
{
    sendHeaders(clientFd, "204 No Content", "text/plain", 0);
}

void ResponseHandler::sendBadRequest(
    int clientFd,
    const char *data)
{
    sendHeaders(clientFd, "400 Bad Request", "text/plain", strlen(data));
    write(clientFd, data, strlen(data));
}

void ResponseHandler::sendNotFound(int clientFd)
{
    const char *data = "Not Found";
    sendHeaders(clientFd, "404 Not Found", "text/plain", strlen(data));
    write(clientFd, data, strlen(data));
}

void ResponseHandler::sendFile(
    int clientFd,
    FILE *fp,
    const char *contentType,
    size_t contentLength)
{
    sendHeaders(clientFd, "200 OK", contentType, contentLength);

    char buffer[BSIZE];
    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        write(clientFd, buffer, r);
        r = fread(buffer, 1, BSIZE, fp);
    }
}