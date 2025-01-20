#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

class RequestHandler {
public:
    static void handle(int clientFd, char* request);
private:
    static const char *get_content_type(const char *path);
    static void send400(int clientFd);
    static void send404(int clientFd);
    static void serveResource(
        int clientFd,
        const char *path);
};

#endif