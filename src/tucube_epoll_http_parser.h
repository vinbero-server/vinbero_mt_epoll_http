#ifndef _TUCUBE_EPOLL_HTTP_PARSER_H
#define _TUCUBE_EPOLL_HTTP_PARSER_H

#include <tucube/tucube_module.h>
#include <tucube/tucube_cldata.h>

enum tucube_epoll_http_parser_state
{
    TUCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_METHOD,
    TUCUBE_EPOLL_HTTP_PARSER_METHOD_END,
    TUCUBE_EPOLL_HTTP_PARSER_URI_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_URI,
    TUCUBE_EPOLL_HTTP_PARSER_URI_END,
    TUCUBE_EPOLL_HTTP_PARSER_VERSION_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_VERSION,
    TUCUBE_EPOLL_HTTP_PARSER_VERSION_END,
    TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD,
    TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END,
    TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE,
    TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END,
    TUCUBE_EPOLL_HTTP_PARSER_HEADERS_END,
    TUCUBE_EPOLL_HTTP_PARSER_ERROR
};

struct tucube_epoll_http_parser
{
    enum tucube_epoll_http_parser_state state;
    char* buffer;
    size_t buffer_offset;
    char* token;
    size_t token_offset;
};

int tucube_epoll_http_parser_parse_message_header(struct tucube_module* module, struct tucube_cldata* cldata, struct tucube_epoll_http_parser* parser, size_t buffer_size);
int tucube_epoll_http_parser_parse_message_body(struct tucube_module* module, struct tucube_cldata* cldata, struct tucube_epoll_http_parser* parser, size_t buffer_size);

#endif
