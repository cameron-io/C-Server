#include <iostream>
#include <unistd.h>
#include <string.h>
#include "request_handler.hh"

#define BASE_PATH "static"

void RequestHandler::handle(int clientFd, char *request)
{
    if (strncmp("GET /", request, 5))
    {
        send400(clientFd);
    }
    else
    {
        char *path = request + 4;
        char *end_path = strstr(path, " ");
        if (!end_path)
        {
            send400(clientFd);
        }
        else
        {
            *end_path = 0;
            serveResource(clientFd, path);
        }
    }
}

void RequestHandler::send400(int clientFd)
{
    const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 11\r\n\r\nBad Request";
    write(clientFd, c400, strlen(c400));
}

void RequestHandler::send404(int clientFd)
{
    const char *c404 = "HTTP/1.1 404 Not Found\r\n"
                       "Connection: close\r\n"
                       "Content-Length: 9\r\n\r\nNot Found";
    write(clientFd, c404, strlen(c404));
}

void RequestHandler::serveResource(
    int clientFd,
    const char *path)
{
    if (strcmp(path, "/") == 0)
        path = "/index.html";

    if (strlen(path) > 100)
    {
        send400(clientFd);
        return;
    }

    if (strstr(path, ".."))
    {
        send404(clientFd);
        return;
    }

    char full_path[128];
    sprintf(full_path, "%s%s", BASE_PATH, path);

    FILE *fp = fopen(full_path, "rb");

    if (!fp)
    {
        send404(clientFd);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    size_t cl = ftell(fp);
    rewind(fp);

    const char *ct = getContentType(full_path);

#define BSIZE 1024
    char buffer[BSIZE];

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Connection: close\r\n");
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Content-Length: %u\r\n", (unsigned int)cl);
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "Content-Type: %s\r\n", ct);
    write(clientFd, buffer, strlen(buffer));

    sprintf(buffer, "\r\n");
    write(clientFd, buffer, strlen(buffer));

    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        write(clientFd, buffer, r);
        r = fread(buffer, 1, BSIZE, fp);
    }

    fclose(fp);
}

const char *RequestHandler::getContentType(const char *path)
{
    const char *last_dot = strrchr(path, '.');
    if (last_dot)
    {
        if (strcmp(last_dot, ".css") == 0)
            return "text/css";
        if (strcmp(last_dot, ".csv") == 0)
            return "text/csv";
        if (strcmp(last_dot, ".gif") == 0)
            return "image/gif";
        if (strcmp(last_dot, ".htm") == 0)
            return "text/html";
        if (strcmp(last_dot, ".html") == 0)
            return "text/html";
        if (strcmp(last_dot, ".ico") == 0)
            return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0)
            return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0)
            return "image/jpeg";
        if (strcmp(last_dot, ".js") == 0)
            return "application/javascript";
        if (strcmp(last_dot, ".json") == 0)
            return "application/json";
        if (strcmp(last_dot, ".png") == 0)
            return "image/png";
        if (strcmp(last_dot, ".pdf") == 0)
            return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0)
            return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0)
            return "text/plain";
    }

    return "application/octet-stream";
}
