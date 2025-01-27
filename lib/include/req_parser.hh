#ifndef REQ_PARSER_H
#define REQ_PARSER_H

const char *get_content_type(const char *path);

const char *parse_method(char *req);

#endif