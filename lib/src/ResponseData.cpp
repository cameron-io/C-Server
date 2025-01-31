#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ResponseData.hpp"

std::string ResponseData::OK(
    std::string contentType,
    int contentLength,
    std::string data)
{
    std::string headers = SetHeaders("200 OK", contentType, (unsigned int)contentLength);
    return headers + data;
}

std::string ResponseData::NoContent()
{
    std::string buffer = SetHeaders("204 No Content", "text/plain", 0);
    return buffer;
}

std::string ResponseData::BadRequest(std::string data)
{
    std::string headers = SetHeaders("400 Bad Request", "text/plain", data.length());
    return headers + data;
}

std::string ResponseData::NotFound(std::string data)
{
    std::string headers = SetHeaders("404 Not Found", "text/plain", data.length());
    return headers + data;
}

int ResponseData::ReadFileContents(
    char *buffer,
    std::string fullPath)
{
    FILE *fp = fopen(fullPath.c_str(), "rb");
    if (!fp)
        return -1;

    fseek(fp, 0L, SEEK_END);
    size_t contentLength = ftell(fp);
    rewind(fp);

    int r = fread(buffer, 1, BSIZE, fp);
    while (r)
    {
        r = fread(buffer + r, 1, BSIZE, fp);
    }
    fclose(fp);

    return contentLength;
}

std::string ResponseData::SetHeaders(
    std::string statusCode,
    std::string contentType,
    unsigned int contentLength)
{
    std::string cors_headers =
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, PUT, POST, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: *\r\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "Access-Control-Max-Age: 86400\r\n";

    std::string buffer =
        "HTTP/1.1 " + statusCode + "\r\n" +
        "Content-Length: " + std::to_string(contentLength) + "\r\n" +
        "Content-Type: " + contentType + "\r\n" +
        "Connection: close\r\n" +
        cors_headers +
        "\r\n";

    return buffer;
}