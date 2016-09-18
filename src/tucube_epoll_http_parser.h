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
    TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI,
    TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI_END,
    TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL,
    TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_END,
    TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH,
    TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH_END,
    TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE,
    TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE_END,
    TUCUBE_EPOLL_HTTP_PARSER_CONTENT_LENGTH_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_CONTENT_LENGTH,
    TUCUBE_EPOLL_HTTP_PARSER_CONTENT_LENGTH_END,
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
    size_t buffer_size;
    char* token;
    size_t token_offset;
    ssize_t body_remainder;

    int (*on_request_start)(struct tucube_module*, struct tucube_cldata*);
    int (*on_method)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_request_uri)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_protocol)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_script_path)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_content_type)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*on_content_length)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
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
