#include <string.h>
#include "req_parser.h"
#include "req_handler.h"
#include "res_content.h"
#include "core.h"

#define BASE_PATH "public"
#define PATH_SIZE 128

char *serve_resource(const char *path);

char *handle_request(char *req)
{
    const char *method = parse_method(req);

    if (strcmp(method, "GET") == 0)
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
    else if (strcmp(method, "POST") == 0)
    {
        return no_content();
    }
    else if (strcmp(method, "OPTIONS") == 0)
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

    char full_path[PATH_SIZE];
    snprintf(full_path, PATH_SIZE, "%s%s", BASE_PATH, path);

#if defined(_WIN32)
    char *p = full_path;
    while (*p)
    {
        if (*p == '/')
            *p = '\\';
        ++p;
    }
#endif
    char file_buffer[BSIZE];
    if (read_file_contents(file_buffer, full_path))
        return not_found("Could not locate resource.");

    const char *content_type = get_content_type(full_path);
    char *response = ok(content_type, file_buffer);

    return response;
}
