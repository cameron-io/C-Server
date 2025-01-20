#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

constexpr int MAX_CLIENTS = 10;
constexpr int PORT = 8080;

class HttpServer
{
public:
    int serverFd;

    HttpServer()
    {
        this->createSocket();
        this->bindSocket();
        this->listenSocket();
    }
    ~HttpServer()
    {
        close(serverFd);
    }

    int acceptConnection();

private:
    int createSocket();
    void bindSocket();
    void listenSocket();
};

#endif