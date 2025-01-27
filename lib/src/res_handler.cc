#include <string.h>
#include <stdio.h>
#include "res_handler.hh"
#include "res_content.hh"
#include "http_server.hh"

int SendOK(
    int clientFd,
    std::string contentType,
    std::string data)
{
    std::string res_buffer = OK(contentType, data);
    return send(clientFd, res_buffer.c_str(), res_buffer.length(), 0);
}

int SendNoContent(int clientFd)
{
    std::string res_buffer = NoContent();
    return send(clientFd, res_buffer.c_str(), res_buffer.length(), 0);
}

int SendBadRequest(
    int clientFd,
    std::string data)
{
    std::string res_buffer = BadRequest(data);
    return send(clientFd, res_buffer.c_str(), res_buffer.length(), 0);
}

int SendNotFound(
    int clientFd,
    std::string data)
{
    std::string res_buffer = NotFound(data);
    return send(clientFd, res_buffer.c_str(), res_buffer.length(), 0);
}

int SendFile(
    int clientFd,
    FILE *fp,
    std::string contentType,
    size_t contentLength)
{
    std::string headers = SetHeaders("200 OK", contentType, contentLength);
    std::string file_buffer = ReadFile(fp);
    std::string res_buffer = headers + file_buffer;
    return send(clientFd, res_buffer.c_str(), res_buffer.length(), 0);
}
