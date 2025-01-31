#include <stdio.h>
#include "req_parser.hh"
#include "req_handler.hh"
#include "res_data.hh"

#define BASE_PATH "public"
#define PATH_SIZE 128

namespace
{
    std::string ServeResource(std::string path)
    {
        if (path == "/")
            path = "/index.html";

        if (path.length() > 100)
        {
            return ResponseData::BadRequest("Path size too large.");
        }

        if (path.find("..") != std::string::npos)
        {
            return ResponseData::BadRequest("Path navigation not permitted.");
        }

        char full_path[PATH_SIZE];
        snprintf(full_path, PATH_SIZE, "%s%s", BASE_PATH, path.c_str());

#if defined(_WIN32)
        std::string p = full_path;
        while (*p)
        {
            if (*p == '/')
                *p = '\\';
            ++p;
        }
#endif
        char file_buffer[BSIZE];
        int content_length;
        if (-1 == (content_length = ResponseData::ReadFileContents(file_buffer, full_path)))
            return ResponseData::NotFound("Could not locate resource.");

        std::string content_type = RequestParser::ParseContentType(full_path);
        std::string response = ResponseData::OK(content_type, content_length, file_buffer);

        return response;
    }
}

std::string RequestHandler::Handle(std::string req)
{
    std::string method = RequestParser::ParseMethod(req);

    if (method == "GET")
    {
        std::string path = req.substr(4, req.length() - 4);
        size_t end_index = path.find(" ");
        if (end_index == std::string::npos)
        {
            return ResponseData::BadRequest("Invalid path.");
        }
        else
        {
            // null terminate path
            path[end_index] = 0;
            return ServeResource(path.c_str());
        }
    }
    else if (method == "POST")
    {
        return ResponseData::NoContent();
    }
    else if (method == "OPTIONS")
    {
        // Handle CORS pre-flight
        return ResponseData::NoContent();
    }
    return ResponseData::BadRequest("Unsupported Request.");
}
