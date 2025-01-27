#ifndef RESPONSE_CONTENT_H
#define RESPONSE_CONTENT_H

#include <string>

std::string set_headers(std::string statusCode, std::string contentType, unsigned int contentLength);

std::string ok(
    std::string contentType,
    std::string data);

std::string no_content();

std::string bad_request(
    std::string data);

std::string not_found(
    std::string data);

std::string read_file(
    FILE *fp);

#endif