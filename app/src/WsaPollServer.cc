#ifdef _IA64_
#pragma warning(disable : 4311)
#pragma warning(disable : 4312)
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>

#include "request_handler.hh"

#define ERR(e) \
    printf("%s:%s failed: %d [%s@%ld]\n", __FUNCTION__, e, WSAGetLastError(), __FILE__, __LINE__)

#define CLOSESOCK(s)         \
    if (INVALID_SOCKET != s) \
    {                        \
        closesocket(s);      \
        s = INVALID_SOCKET;  \
    }

#define DEFAULT_WAIT 30000

#define WS_VER 0x0202

#define DEFAULT_PORT 8080

#define TST_MSG "HTTP/1.1 200 OK\r\n"    \
                "Connection: close\r\n"  \
                "Content-Length: 12\r\n" \
                "\r\n"                   \
                "Hello there!"

HANDLE hCloseSignal = NULL;

int handleRequest(SOCKET asock, char *buf)
{
    return ReqHandler::Handle(asock, buf);
}

int __cdecl main()
{
    WSADATA wsd;
    INT nStartup = 0,
        nErr = 0,
        ret = 0;
    SOCKET lsock = INVALID_SOCKET,
           asock = INVALID_SOCKET;
    SOCKADDR_STORAGE addr = {0};
    WSAPOLLFD fdarray = {0};
    ULONG uNonBlockingMode = 1;
    CHAR buf[MAX_PATH] = {0};
    DWORD dwThreadId = 0;

    __try
    {
        nErr = WSAStartup(WS_VER, &wsd);
        if (nErr)
        {
            WSASetLastError(nErr);
            ERR("WSAStartup");
            __leave;
        }
        else
            nStartup++;

        if (NULL == (hCloseSignal = CreateEvent(NULL,
                                                TRUE,
                                                FALSE,
                                                NULL)))
        {
            ERR("CreateEvent");
            __leave;
        }

        addr.ss_family = AF_INET;
        INETADDR_SETANY((SOCKADDR *)&addr);
        SS_PORT((SOCKADDR *)&addr) = htons(DEFAULT_PORT);

        if (INVALID_SOCKET == (lsock = socket(AF_INET,
                                              SOCK_STREAM,
                                              0)))
        {
            ERR("socket");
            __leave;
        }

        if (SOCKET_ERROR == ioctlsocket(lsock,
                                        FIONBIO,
                                        &uNonBlockingMode))
        {
            ERR("FIONBIO");
            __leave;
        }

        if (SOCKET_ERROR == bind(lsock,
                                 (SOCKADDR *)&addr,
                                 sizeof(addr)))
        {
            ERR("bind");
            __leave;
        }

        if (SOCKET_ERROR == listen(lsock, 1))
        {
            ERR("listen");
            __leave;
        }

        // Call WSAPoll for readability of listener (accepted)
        while (1)
        {
            fdarray.fd = lsock;
            fdarray.events = POLLRDNORM;

            if (SOCKET_ERROR == (ret = WSAPoll(&fdarray,
                                               1,
                                               DEFAULT_WAIT)))
            {
                ERR("WSAPoll");
                __leave;
            }

            if (ret)
            {
                if (fdarray.revents & POLLRDNORM)
                {
                    printf("Main: Connection established.\n");

                    if (INVALID_SOCKET == (asock = accept(lsock,
                                                          NULL,
                                                          NULL)))
                    {
                        ERR("accept");
                        __leave;
                    }

                    while (SOCKET_ERROR == (ret = recv(asock,
                                                       buf,
                                                       sizeof(buf),
                                                       0)))
                    {
                        if (WSAEWOULDBLOCK == WSAGetLastError())
                        {
                            continue;
                        }
                        else if (WSAECONNRESET == WSAGetLastError())
                        {
                            printf("Client disconnected.\n");
                        }
                        else
                        {
                            ERR("recv");
                            __leave;
                        }
                    }
                    printf("Main: recvd %d bytes\n", ret);
                }
            }

            // Call WSAPoll for writeability of accepted

            fdarray.fd = asock;
            fdarray.events = POLLWRNORM;

            if (SOCKET_ERROR == (ret = WSAPoll(&fdarray,
                                               1,
                                               DEFAULT_WAIT)))
            {
                /*
                    Handle disconnects before Writing Response
                */
                if (WSAENOTSOCK == WSAGetLastError())
                {
                    printf("Client disconnected.\n");
                    continue;
                }
                ERR("WSAPoll");
                __leave;
            }

            if (ret)
            {
                if (fdarray.revents & POLLWRNORM)
                {
                    /*
                        Instructs client to close the connection upon
                        completing HTTP transaction.
                    */
                    int bytesSent = handleRequest(asock, buf);
                    printf("Main: sent %d bytes\n", bytesSent);
                }
            }

            SetEvent(hCloseSignal);
        }
    }
    __finally
    {
        CloseHandle(hCloseSignal);
        CLOSESOCK(asock);
        CLOSESOCK(lsock);
        if (nStartup)
            WSACleanup();
    }

    return 0;
}