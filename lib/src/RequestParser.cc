#include "RequestParser.hh"

std::string RequestParser::ParseContentType(std::string path)
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

std::string RequestParser::ParseMethod(std::string req)
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