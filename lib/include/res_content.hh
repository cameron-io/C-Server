#pragma once

#include <string>

#define BSIZE 1024

std::string set_headers(
    std::string status_code,
    std::string content_type,
    unsigned int content_length);

std::string ok(
    std::string content_type,
    int content_length,
    std::string data);

std::string no_content();

std::string bad_request(std::string data);

std::string not_found(std::string data);

int read_file_contents(
    char *destination,
    std::string full_path);
