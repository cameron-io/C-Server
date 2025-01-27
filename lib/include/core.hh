#ifndef CORE_H
#define CORE_H

constexpr int MAX_CLIENTS = 10;
constexpr int PORT = 8080;

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
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
#define CLOSESOCKET(s)       \
    if (INVALID_SOCKET != s) \
    {                        \
        closesocket(s);      \
        s = INVALID_SOCKET;  \
    }
#define GETSOCKETERRNO() (WSAGetLastError())

#define ERR(e) \
    printf("%s:%s failed: %d [%s@%ld]\n", __FUNCTION__, e, WSAGetLastError(), __FILE__, __LINE__)

#define DEFAULT_WAIT 30000

#define WS_VER 0x0202

#define DEFAULT_PORT 8080

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

#endif