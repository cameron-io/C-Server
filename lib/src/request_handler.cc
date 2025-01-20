#include <iostream>
#include <unistd.h>
#include <string.h>
#include "request_handler.hh"
#include "response_handler.hh"

#define BASE_PATH "static"

void RequestHandler::handle(int clientFd, char *request)
{
    if (strncmp("GET /", request, 5))
    {
        ResponseHandler::sendBadRequest(clientFd);
    }
    else
    {
        char *path = request + 4;
        char *end_path = strstr(path, " ");
        if (!end_path)
        {
            ResponseHandler::sendBadRequest(clientFd);
        }
        else
        {
            // null terminate path
            *end_path = 0;
            serveResource(clientFd, path);
        }
    }
}

void RequestHandler::serveResource(
    int clientFd,
    const char *path)
{
    if (strcmp(path, "/") == 0)
        path = "/index.html";

    if (strlen(path) > 100)
    {
        ResponseHandler::sendBadRequest(clientFd);
        return;
    }

    if (strstr(path, ".."))
    {
        ResponseHandler::sendNotFound(clientFd);
        return;
    }

    char full_path[128];
    sprintf(full_path, "%s%s", BASE_PATH, path);

    FILE *fp = fopen(full_path, "rb");
    if (!fp)
    {
        ResponseHandler::sendNotFound(clientFd);
        return;
    }

    // Read File Contents
    fseek(fp, 0L, SEEK_END);
    size_t contentLength = ftell(fp);
    rewind(fp);

    // Send File Contents
    const char *contentType = getContentType(full_path);
    ResponseHandler::sendFile(clientFd, fp, contentType, contentLength);

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
