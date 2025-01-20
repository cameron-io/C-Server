#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include "request_handler.hh"
#include "response_handler.hh"

#define BASE_PATH "static"

std::string RequestHandler::parseMethod(std::string r)
{
    std::string method;
    if (r.substr(0, 3) == "GET")
        method = "GET";
    else if (r.substr(0, 3) == "PUT")
        method = "PUT";
    else if (r.substr(0, 4) == "POST")
        method = "POST";
    else if (r.substr(0, 6) == "DELETE")
        method = "DELETE";
    else if (r.substr(0, 7) == "OPTIONS")
        method = "OPTIONS";
    return method;
}

void RequestHandler::handle(int clientFd, char *request)
{
    std::string method = parseMethod(request);

    if (method == "GET")
    {
        std::string r = request;
        std::string path = r.substr(4, r.length() - 4);
        size_t endIndex = path.find(" ");
        if (endIndex == std::string::npos)
        {
            ResponseHandler::sendBadRequest(clientFd, "Invalid path.");
        }
        else
        {
            // null terminate path
            path[endIndex] = 0;
            serveResource(clientFd, path.c_str());
        }
    }
    else if (method == "POST")
    {
        ResponseHandler::sendNoContent(clientFd);
    }
    else if (method == "OPTIONS")
    {
        // Handle CORS pre-flight
        ResponseHandler::sendNoContent(clientFd);
    }
    else
    {
        ResponseHandler::sendBadRequest(clientFd, "Unsupported Request.");
    }
}

void RequestHandler::serveResource(int clientFd, std::string path)
{
    if (path == "/")
        path = "/index.html";

    if (path.length() > 100)
    {
        ResponseHandler::sendBadRequest(clientFd, "Path size too large.");
        return;
    }

    if (path.find("..") != std::string::npos)
    {
        ResponseHandler::sendBadRequest(clientFd, "Path navigation not permitted.");
        return;
    }

    char fullPath[128];
    sprintf(fullPath, "%s%s", BASE_PATH, path.c_str());

    FILE *fp = fopen(fullPath, "rb");
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
    const char *contentType = getContentType(fullPath);
    ResponseHandler::sendFile(clientFd, fp, contentType, contentLength);

    fclose(fp);
}

const char *RequestHandler::getContentType(const char *path)
{
    const char *lastDot = strrchr(path, '.');
    if (lastDot)
    {
        if (strcmp(lastDot, ".css") == 0)
            return "text/css";
        if (strcmp(lastDot, ".csv") == 0)
            return "text/csv";
        if (strcmp(lastDot, ".gif") == 0)
            return "image/gif";
        if (strcmp(lastDot, ".htm") == 0)
            return "text/html";
        if (strcmp(lastDot, ".html") == 0)
            return "text/html";
        if (strcmp(lastDot, ".ico") == 0)
            return "image/x-icon";
        if (strcmp(lastDot, ".jpeg") == 0)
            return "image/jpeg";
        if (strcmp(lastDot, ".jpg") == 0)
            return "image/jpeg";
        if (strcmp(lastDot, ".js") == 0)
            return "application/javascript";
        if (strcmp(lastDot, ".json") == 0)
            return "application/json";
        if (strcmp(lastDot, ".png") == 0)
            return "image/png";
        if (strcmp(lastDot, ".pdf") == 0)
            return "application/pdf";
        if (strcmp(lastDot, ".svg") == 0)
            return "image/svg+xml";
        if (strcmp(lastDot, ".txt") == 0)
            return "text/plain";
    }

    return "application/octet-stream";
}
