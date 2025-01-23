#include <thread>
#include "http_server.hh"
#include "request_handler.hh"

HANDLE hCloseSignal = NULL;

void HandleRequestAsync(SOCKET asock)
{
    std::thread clientThread(
        [asock]()
        {
            HttpServer::ReadRequest(asock);
        });
    clientThread.detach();
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

        printf("Listening on port: %d\n", DEFAULT_PORT);

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

                    HandleRequestAsync(asock);
                }
            }

            SetEvent(hCloseSignal);
        }
    }
    __finally
    {
        CloseHandle(hCloseSignal);
        CLOSESOCKET(asock);
        CLOSESOCKET(lsock);
        if (nStartup)
            WSACleanup();
    }

    return 0;
}