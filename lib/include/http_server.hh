#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

constexpr int MAX_CLIENTS = 10;
constexpr int PORT = 8080;

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

class HttpServer
{
public:
    SOCKET serverFd;

    HttpServer()
    {
        this->createSocket();
        this->bindSocket();
        this->listenSocket();
    }
    ~HttpServer()
    {
        CLOSESOCKET(serverFd);
    }

    SOCKET acceptConnection();

private:
    SOCKET createSocket();
    void bindSocket();
    void listenSocket();
};

#endif