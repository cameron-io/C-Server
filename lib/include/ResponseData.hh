#pragma once

#include <string>

#define BSIZE 1024

namespace ResponseData
{
    std::string SetHeaders(
        std::string statusCode,
        std::string contentType,
        unsigned int contentLength);

    std::string OK(
        std::string contentType,
        int contentLength,
        std::string data);

    std::string NoContent();

    std::string BadRequest(std::string data);

    std::string NotFound(std::string data);

    int ReadFileContents(
        char *destination,
        std::string fullPath);
}