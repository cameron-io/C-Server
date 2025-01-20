#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>

class RequestHandler
{
public:
    static void handle(int clientFd, char *request);

private:
    static std::string parseMethod(std::string r);
    static const char *getContentType(const char *path);
    static void serveResource(
        int clientFd,
        std::string path);
};

#endif