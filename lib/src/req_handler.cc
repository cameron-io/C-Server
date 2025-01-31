#include <stdio.h>
#include "req_parser.hh"
#include "req_handler.hh"
#include "res_content.hh"

#define BASE_PATH "public"
#define PATH_SIZE 128

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
    if (-1 == (content_length = read_file_contents(file_buffer, full_path)))
        return not_found("Could not locate resource.");

    std::string content_type = get_content_type(full_path);
    std::string response = ok(content_type, content_length, file_buffer);

    return response;
}
