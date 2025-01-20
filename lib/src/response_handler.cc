#include <unistd.h>
#include <string.h>
#include <iostream>
#include "response_handler.hh"

void ResponseHandler::sendBadRequest(int clientFd)
{
    const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 11\r\n\r\nBad Request";
    write(clientFd, c400, strlen(c400));
}

void ResponseHandler::sendNotFound(int clientFd)
{
    const char *c404 = "HTTP/1.1 404 Not Found\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 9\r\n\r\nNot Found";
    write(clientFd, c404, strlen(c404));
}

void ResponseHandler::sendFile(
    int clientFd,
    FILE *fp,
    const char *contentType,
    size_t contentLength)
{
#define BSIZE 1024
    char buffer[BSIZE];

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Connection: close\r\n");
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Content-Length: %u\r\n", (unsigned int)contentLength);
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Content-Type: %s\r\n", contentType);
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "\r\n");
    write(clientFd, buffer, strlen(buffer));

    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        write(clientFd, buffer, r);
        r = fread(buffer, 1, BSIZE, fp);
    }
}