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
        size_t endIndex = path.find(" ");
        if (endIndex == std::string::npos)
        {
            return bad_request("Invalid path.");
        }
        else
        {
            // null terminate path
            path[endIndex] = 0;
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
        return not_found("Could not locate resource.");
    }

    // Read File Contents
    fseek(fp, 0L, SEEK_END);
    size_t contentLength = ftell(fp);
    rewind(fp);

    // Send File Contents
    std::string contentType = get_content_type(fullPath);
    std::string headers = set_headers("200 ok", contentType, contentLength);

    std::string file_buffer = read_file(fp);

    fclose(fp);

    return headers + file_buffer;
}

std::string get_content_type(std::string path)
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