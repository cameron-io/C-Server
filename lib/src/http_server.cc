#include <iostream>
#include <string.h>
#include <stdexcept>
#include "http_server.hh"
#include "request_handler.hh"

SOCKET serverFd;

void http_server_create_socket();
void http_server_bind_socket(SOCKET serverFd);
void http_server_listen_socket();

SOCKET http_server_init()
{
    http_server_create_socket();
    http_server_bind_socket(serverFd);
    http_server_listen_socket();
    return serverFd;
}

SOCKET http_server_get_socket()
{
    return serverFd;
}

void http_server_stop()
{
    CLOSESOCKET(serverFd);
}

void http_server_create_socket()
{
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (!ISVALIDSOCKET(serverFd))
    {
        throw std::runtime_error("Failed to create socket.");
    }
}

void http_server_bind_socket(SOCKET serverFd)
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

void http_server_listen_socket()
{
    if (listen(serverFd, MAX_CLIENTS) == -1)
    {
        throw std::runtime_error("Failed to listen.");
    }
}

SOCKET http_server_accept_connection()
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    return accept(serverFd, (struct sockaddr *)&clientAddress, &clientAddressLength);
}

void http_server_read_request(SOCKET clientFd)
{
    char input_buffer[1024];

    while (true)
    {
        int bytesRead = recv(clientFd, input_buffer, sizeof(input_buffer), 0);
        if (bytesRead <= 0)
        {
            break;
        }
        request_handler_handle(clientFd, input_buffer);
    }

    CLOSESOCKET(clientFd);
}
