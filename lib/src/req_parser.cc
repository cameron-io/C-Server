#include "req_parser.hh"

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
