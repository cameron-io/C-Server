#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

constexpr int MAX_CLIENTS = 10;
constexpr int PORT = 8080;

class Server {
public:
    static int createSocket();
    static void bindSocket(int serverFd);
    static void listenSocket(int serverFd);
    static int acceptConnection(int serverFd);
};

#endif