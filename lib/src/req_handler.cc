#include <iostream>
#include <string>
#include <string.h>
#include "req_parser.hh"
#include "req_handler.hh"
#include "res_content.hh"
#include "core.hh"

#define BASE_PATH "public"

std::string serve_resource(std::string path);

std::string handle_request(std::string req)
{
    std::string method = parse_method(req);

    if (method == "GET")
    {
        std::string path = req.substr(4, req.length() - 4);
        size_t end_index = path.find(" ");
        if (end_index == std::string::npos)
        {
            return bad_request("Invalid path.");
        }
        else
        {
            // null terminate path
            path[end_index] = 0;
            return serve_resource(path.c_str());
        }
    }
    else if (method == "POST")
    {
        return no_content();
    }
    else if (method == "OPTIONS")
    {
        // Handle CORS pre-flight
        return no_content();
    }
    return bad_request("Unsupported Request.");
}

std::string serve_resource(std::string path)
{
    if (path == "/")
        path = "/index.html";

    if (path.length() > 100)
    {
        return bad_request("Path size too large.");
    }

    if (path.find("..") != std::string::npos)
    {
        return bad_request("Path navigation not permitted.");
    }

    const int path_size = 128;
    char full_path[path_size];
    snprintf(full_path, path_size, "%s%s", BASE_PATH, path.c_str());

#if defined(_WIN32)
    char *p = full_path;
    while (*p)
    {
        if (*p == '/')
            *p = '\\';
        ++p;
    }
#endif

    FILE *fp = fopen(full_path, "rb");
    if (!fp)
    {
        return not_found("Could not locate resource.");
    }

    // Read File Contents
    fseek(fp, 0L, SEEK_END);
    size_t content_length = ftell(fp);
    rewind(fp);

    // Send File Contents
    std::string content_type = get_content_type(full_path);
    std::string headers = set_headers("200 ok", content_type, content_length);

    std::string file_buffer = read_file(fp);

    fclose(fp);

    return headers + file_buffer;
}
