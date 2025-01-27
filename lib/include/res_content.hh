#ifndef RESPONSE_CONTENT_H
#define RESPONSE_CONTENT_H

#include <string>

std::string SetHeaders(std::string statusCode, std::string contentType, unsigned int contentLength);

std::string OK(
    std::string contentType,
    std::string data);

std::string NoContent();

std::string BadRequest(
    std::string data);

std::string NotFound(
    std::string data);

std::string ReadFile(
    FILE *fp);

#endif