#include <iostream>
#include <string>
#include <string.h>
#include "req_handler.hh"
#include "res_handler.hh"
#include "core.hh"

#define BASE_PATH "public"

std::string ParseMethod(std::string request);
std::string GetContentType(std::string path);
int ServeResource(SOCKET clientFd, std::string path);

int HandleRequest(SOCKET clientFd, std::string req)
{
    std::string method = ParseMethod(req);

    if (method == "GET")
    {
        std::string path = req.substr(4, req.length() - 4);
        size_t endIndex = path.find(" ");
        if (endIndex == std::string::npos)
        {
            return SendBadRequest(clientFd, "Invalid path.");
        }
        else
        {
            // null terminate path
            path[endIndex] = 0;
            ServeResource(clientFd, path.c_str());
        }
    }
    else if (method == "POST")
    {
        return SendNoContent(clientFd);
    }
    else if (method == "OPTIONS")
    {
        // Handle CORS pre-flight
        return SendNoContent(clientFd);
    }
    return SendBadRequest(clientFd, "Unsupported Request.");
}

int ServeResource(SOCKET clientFd, std::string path)
{
    if (path == "/")
        path = "/index.html";

    if (path.length() > 100)
    {
        return SendBadRequest(clientFd, "Path size too large.");
    }

    if (path.find("..") != std::string::npos)
    {
        return SendBadRequest(clientFd, "Path navigation not permitted.");
    }

    const int pathSize = 128;
    char fullPath[pathSize];
    snprintf(fullPath, pathSize, "%s%s", BASE_PATH, path.c_str());

#if defined(_WIN32)
    char *p = fullPath;
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
        return SendNotFound(clientFd, "Could not locate resource.");
    }

    // Read File Contents
    fseek(fp, 0L, SEEK_END);
    size_t contentLength = ftell(fp);
    rewind(fp);

    // Send File Contents
    std::string contentType = GetContentType(fullPath);
    int bytesSent = SendFile(clientFd, fp, contentType.c_str(), contentLength);

    fclose(fp);

    return bytesSent;
}

std::string GetContentType(std::string path)
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

std::string ParseMethod(std::string req)
{
    std::string method;
    if (req.substr(0, 3) == "GET")
        method = "GET";
    else if (req.substr(0, 3) == "PUT")
        method = "PUT";
    else if (req.substr(0, 4) == "POST")
        method = "POST";
    else if (req.substr(0, 6) == "DELETE")
        method = "DELETE";
    else if (req.substr(0, 7) == "OPTIONS")
        method = "OPTIONS";
    return method;
}