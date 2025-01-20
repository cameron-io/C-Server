#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

constexpr int MAX_CLIENTS = 10;
constexpr int PORT = 8080;

class HttpServer {
public:
    HttpServer() {}

    int createSocket();
    void bindSocket();
    void listenSocket();
    int acceptConnection();
private:
    int serverFd;
};

#endif