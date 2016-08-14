#ifndef _TUCUBE_EPOLL_HTTP_PARSER_H
#define _TUCUBE_EPOLL_HTTP_PARSER_H

#include <tucube/tucube_module.h>
#include "../../tucube_tcp_epoll/src/tucube_tcp_epoll.h"

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
    TUCUBE_EPOLL_HTTP_PARSER_HEADER_END,
    TUCUBE_EPOLL_HTTP_PARSER_ERROR
};

struct tucube_epoll_http_parser
{
    enum tucube_epoll_http_parser_state state;
    char* buffer;
    ssize_t buffer_offset;
    ssize_t buffer_capacity;
    char* token;
    ssize_t token_size;

    int (*on_method)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);
    int (*on_uri)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);
    int (*on_version)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);
    int (*on_header_field)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);
    int (*on_header_value)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);
};

int tucube_epoll_http_parser_parse_message_header(struct tucube_module* module, struct tucube_tcp_epoll_cldata* cldata, struct tucube_epoll_http_parser* parser, ssize_t buffer_size);
int tucube_epoll_http_parser_parse_message_body(struct tucube_module* module, struct tucube_tcp_epoll_cldata* cldata, struct tucube_epoll_http_parser* parser, ssize_t buffer_size);

#endif
