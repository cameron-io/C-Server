#include "RequestParser.hh"
#include "RequestHandler.hh"
#include "ResponseData.hh"

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
        std::string fullPath = BASE_PATH + path;

#if defined(_WIN32)
        std::string p = fullPath;
        while (*p)
        {
            if (*p == '/')
                *p = '\\';
            ++p;
        }
#endif
        char fileBuffer[BSIZE];
        int contentLength;
        if (-1 == (contentLength = ResponseData::ReadFileContents(fileBuffer, fullPath)))
            return ResponseData::NotFound("Could not locate resource.");

        std::string contentType = RequestParser::ParseContentType(fullPath);
        std::string response = ResponseData::OK(contentType, contentLength, fileBuffer);

        return response;
    }
}

std::string RequestHandler::Handle(std::string req)
{
    std::string method = RequestParser::ParseMethod(req);

    if (method == "GET")
    {
        std::string path = req.substr(4, req.length() - 4);
        size_t endIndex = path.find(" ");
        if (endIndex == std::string::npos)
        {
            return ResponseData::BadRequest("Invalid path.");
        }
        else
        {
            // null terminate path
            path[endIndex] = 0;
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
