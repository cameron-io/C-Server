#include <iostream>
#include <string>
#include <string.h>
#include "req_parser.hh"
#include "req_handler.hh"
#include "res_content.hh"
#include "core.hh"

#define BASE_PATH "public"

char *serve_resource(const char *path);

char *handle_request(char *req)
{
    std::string method = parse_method(req);

    if (method == "GET")
    {
        char *path = req + 4;
        char *end_path = strstr(path, " ");
        if (!end_path)
        {
            return bad_request("Invalid path.");
        }
        else
        {
            // null terminate path
            *end_path = 0;
            return serve_resource(path);
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

char *serve_resource(const char *path)
{
    if (strcmp(path, "/") == 0)
        path = "/index.html";

    if (strlen(path) > 100)
    {
        return bad_request("Path size too large.");
    }

    if (strstr(path, ".."))
    {
        return bad_request("Path navigation not permitted.");
    }

    const int path_size = 128;
    char full_path[path_size];
    snprintf(full_path, path_size, "%s%s", BASE_PATH, path);

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
    const char *content_type = get_content_type(full_path);
    char *headers = set_headers("200 OK", content_type, content_length);

    char *file_buffer = read_file(fp);

    fclose(fp);

    char *response = strcat(headers, file_buffer);
    free(file_buffer);

    return response;
}
