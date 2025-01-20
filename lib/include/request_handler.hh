#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>

class RequestHandler
{
public:
    static void handle(int clientFd, std::string request);

private:
    static std::string parseMethod(std::string r);
    static std::string getContentType(std::string path);
    static void serveResource(
        int clientFd,
        std::string path);
};

#endif