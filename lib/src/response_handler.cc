#include <string.h>
#include <stdio.h>
#include "response_handler.hh"
#include "http_server.hh"

namespace ResHandler
{

#define BSIZE 1024

    std::string SetHeaders(int clientFd, std::string statusCode, std::string contentType, unsigned int contentLength);

    int SendOK(
        int clientFd,
        std::string contentType,
        std::string data)
    {
        std::string headers = SetHeaders(clientFd, "200 OK", contentType, data.length());
        std::string payload = headers + data;
        return send(clientFd, payload.c_str(), payload.length(), 0);
    }

    int SendNoContent(int clientFd)
    {
        std::string payload = SetHeaders(clientFd, "204 No Content", "text/plain", 0);
        return send(clientFd, payload.c_str(), payload.length(), 0);
    }

    int SendBadRequest(
        int clientFd,
        std::string data)
    {
        std::string headers = SetHeaders(clientFd, "400 Bad Request", "text/plain", data.length());
        std::string payload = headers + data;
        return send(clientFd, payload.c_str(), payload.length(), 0);
    }

    int SendNotFound(
        int clientFd,
        std::string data)
    {
        std::string headers = SetHeaders(clientFd, "404 Not Found", "text/plain", data.length());
        std::string payload = headers + data;
        return send(clientFd, payload.c_str(), payload.length(), 0);
    }

    int SendFile(
        int clientFd,
        FILE *fp,
        std::string contentType,
        size_t contentLength)
    {
        std::string headers = SetHeaders(clientFd, "200 OK", contentType, contentLength);
        int bytesSent = send(clientFd, headers.c_str(), headers.length(), 0);

        char buffer[BSIZE];
        int r = fread(buffer, 1, BSIZE, fp);
        while (r)
        {
            bytesSent += send(clientFd, buffer, r, 0);
            r = fread(buffer, 1, BSIZE, fp);
        }
        return bytesSent;
    }

    std::string SetHeaders(
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
        return buffer;
    }
}