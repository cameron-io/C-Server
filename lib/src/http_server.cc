#include <iostream>
#include <string.h>
#include <stdexcept>
#include "http_server.hh"
#include "req_handler.hh"

namespace HttpServer
{

    SOCKET serverFd;

    void CreateSocket();
    void BindSocket();
    void ListenSocket();

    SOCKET Init()
    {
        CreateSocket();
        BindSocket();
        ListenSocket();
        return serverFd;
    }

    SOCKET GetSocket()
    {
        return serverFd;
    }

    void Stop()
    {
        CLOSESOCKET(serverFd);
    }

    void CreateSocket()
    {
        serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (!ISVALIDSOCKET(serverFd))
        {
            throw std::runtime_error("Failed to create socket.");
        }
    }

    void BindSocket()
    {
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(PORT);
        if (bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            throw std::runtime_error("Failed to bind socket.");
        }
    }

    void ListenSocket()
    {
        if (listen(serverFd, MAX_CLIENTS) == -1)
        {
            throw std::runtime_error("Failed to listen.");
        }
    }

    SOCKET AcceptConnection()
    {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        return accept(serverFd, (struct sockaddr *)&clientAddress, &clientAddressLength);
    }

    void ReadRequest(SOCKET clientFd)
    {
        char input_buffer[1024];

        while (true)
        {
            int bytesRead = recv(clientFd, input_buffer, sizeof(input_buffer), 0);
#ifdef _WIN32

            if (SOCKET_ERROR == bytesRead)
            {
                if (WSAEWOULDBLOCK == WSAGetLastError())
                {
                    continue;
                }
                else if (WSAECONNRESET == WSAGetLastError())
                {
                    printf("Client: connection reset.\n");
                    break;
                }
                else if (WSAECONNABORTED == WSAGetLastError())
                {
                    printf("Client: connection aborted.\n");
                    break;
                }
                else
                {
                    ERR("recv");
                    break;
                }
            }
            printf("Client: recvd %d bytes\n", bytesRead);

#endif
            if (bytesRead <= 0)
            {
                break;
            }
            int bytesSent = HandleRequest(clientFd, input_buffer);
            printf("Client: sent %d bytes\n", bytesSent);
        }

        CLOSESOCKET(clientFd);
    }
}