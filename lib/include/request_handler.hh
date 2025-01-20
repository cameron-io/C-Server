#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

class RequestHandler
{
public:
    static void handle(int clientFd, char *request);

private:
    static const char *getContentType(const char *path);
    static void serveResource(
        int clientFd,
        const char *path);
};

#endif