#ifndef _TUCUBE_EPOLL_HTTP_PARSER_H
#define _TUCUBE_EPOLL_HTTP_PARSER_H

#include <tucube/tucube_module.h>
#include <tucube/tucube_cldata.h>

enum tucube_epoll_http_parser_state
{
    TUCUBE_EPOLL_HTTP_PARSER_HEADERS_BEGIN,
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
    TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_BODY,
    TUCUBE_EPOLL_HTTP_PARSER_BODY_END,
    TUCUBE_EPOLL_HTTP_PARSER_ERROR
};

struct tucube_epoll_http_parser
{
    enum tucube_epoll_http_parser_state state;
    size_t header_buffer_capacity;
    size_t body_buffer_capacity;
    char* buffer;
    size_t buffer_offset;
    char* token;
    size_t token_offset;
    ssize_t body_remainder;

    int (*on_method)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_uri)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_version)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_header_field)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_header_value)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_headers_finish)(struct tucube_module*, struct tucube_cldata*);
    int (*get_content_length)(struct tucube_module*, struct tucube_cldata*, ssize_t*);
    int (*on_body_chunk)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_body_finish)(struct tucube_module*, struct tucube_cldata*);
    int (*on_request_finish)(struct tucube_module*, struct tucube_cldata*);
};

char* tucube_epoll_http_parser_get_buffer_position(struct tucube_epoll_http_parser* parser);

size_t tucube_epoll_http_parser_get_buffer_size(struct tucube_epoll_http_parser* parser);

int tucube_epoll_http_parser_parse(struct tucube_module* module, struct tucube_cldata* cldata, struct tucube_epoll_http_parser* parser, ssize_t read_size);

#endif
