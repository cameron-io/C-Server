#ifndef REQ_PARSER_H
#define REQ_PARSER_H

#include <string>

std::string get_content_type(std::string path);

std::string parse_method(std::string req);

#endif