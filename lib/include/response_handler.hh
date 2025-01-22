#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <iostream>
#include <string>

void send_ok(
    int clientFd,
    std::string contentType,
    std::string data);

void send_no_content(int clientFd);

void send_bad_request(
    int clientFd,
    std::string data);

void send_not_found(
    int clientFd,
    std::string data);

void send_file(
    int clientFd,
    FILE *fp,
    std::string contentType,
    size_t contentLength);

#endif