#ifndef RESPONSE_CONTENT_H
#define RESPONSE_CONTENT_H

#include <string>

std::string set_headers(std::string status_code, std::string content_type, unsigned int content_length);

std::string ok(
    std::string content_type,
    std::string data);

std::string no_content();

std::string bad_request(
    std::string data);

std::string not_found(
    std::string data);

std::string read_file(
    FILE *fp);

#endif