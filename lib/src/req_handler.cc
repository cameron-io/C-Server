#include <iostream>
#include <string>
#include <string.h>
#include "req_handler.hh"
#include "res_content.hh"
#include "core.hh"

#define BASE_PATH "public"

std::string parse_method(std::string request);
std::string get_content_type(std::string path);
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

std::string get_content_type(std::string path)
{
    size_t last_dot_index = path.rfind('.');
    if (last_dot_index != std::string::npos)
    {
        std::string last_dot = path.substr(last_dot_index, path.length() - last_dot_index);

        if (last_dot == ".css")
            return "text/css";
        if (last_dot == ".csv")
            return "text/csv";
        if (last_dot == ".gif")
            return "image/gif";
        if (last_dot == ".htm")
            return "text/html";
        if (last_dot == ".html")
            return "text/html";
        if (last_dot == ".ico")
            return "image/x-icon";
        if (last_dot == ".jpeg")
            return "image/jpeg";
        if (last_dot == ".jpg")
            return "image/jpeg";
        if (last_dot == ".js")
            return "application/javascript";
        if (last_dot == ".json")
            return "application/json";
        if (last_dot == ".png")
            return "image/png";
        if (last_dot == ".pdf")
            return "application/pdf";
        if (last_dot == ".svg")
            return "image/svg+xml";
        if (last_dot == ".txt")
            return "text/plain";
    }

    return "application/octet-stream";
}

std::string parse_method(std::string req)
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