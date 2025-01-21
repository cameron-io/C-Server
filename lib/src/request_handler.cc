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

void RequestHandler::handle(int clientFd, std::string r)
{
    std::string method = parseMethod(r);

    if (method == "GET")
    {
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

    const int pathSize = 128;
    char fullPath[pathSize];
    snprintf(fullPath, pathSize, "%s%s", BASE_PATH, path.c_str());

#if defined(_WIN32)
    char *p = full_path;
    while (*p)
    {
        if (*p == '/')
            *p = '\\';
        ++p;
    }
#endif

    FILE *fp = fopen(fullPath, "rb");
    if (!fp)
    {
        ResponseHandler::sendNotFound(clientFd, "Could not locate resource.");
        return;
    }

    // Read File Contents
    fseek(fp, 0L, SEEK_END);
    size_t contentLength = ftell(fp);
    rewind(fp);

    // Send File Contents
    std::string contentType = getContentType(fullPath);
    ResponseHandler::sendFile(clientFd, fp, contentType.c_str(), contentLength);

    fclose(fp);
}

std::string RequestHandler::getContentType(std::string path)
{
    size_t lastDotIndex = path.rfind('.');
    if (lastDotIndex != std::string::npos)
    {
        std::string lastDot = path.substr(lastDotIndex, path.length() - lastDotIndex);

        if (lastDot == ".css")
            return "text/css";
        if (lastDot == ".csv")
            return "text/csv";
        if (lastDot == ".gif")
            return "image/gif";
        if (lastDot == ".htm")
            return "text/html";
        if (lastDot == ".html")
            return "text/html";
        if (lastDot == ".ico")
            return "image/x-icon";
        if (lastDot == ".jpeg")
            return "image/jpeg";
        if (lastDot == ".jpg")
            return "image/jpeg";
        if (lastDot == ".js")
            return "application/javascript";
        if (lastDot == ".json")
            return "application/json";
        if (lastDot == ".png")
            return "image/png";
        if (lastDot == ".pdf")
            return "application/pdf";
        if (lastDot == ".svg")
            return "image/svg+xml";
        if (lastDot == ".txt")
            return "text/plain";
    }

    return "application/octet-stream";
}
