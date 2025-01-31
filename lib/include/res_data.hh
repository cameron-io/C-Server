#pragma once

#include <string>

#define BSIZE 1024

namespace ResponseData
{
    std::string SetHeaders(
        std::string status_code,
        std::string content_type,
        unsigned int content_length);

    std::string OK(
        std::string content_type,
        int content_length,
        std::string data);

    std::string NoContent();

    std::string BadRequest(std::string data);

    std::string NotFound(std::string data);

    int ReadFileContents(
        char *destination,
        std::string full_path);
}