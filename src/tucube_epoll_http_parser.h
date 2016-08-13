#ifndef _TUCUBE_EPOLL_HTTP_PARSER_H
#define _TUCUBE_EPOLL_HTTP_PARSER_H

enum tucube_epoll_http_parser_state
{
    TUCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_METHOD,
    TUCUBE_EPOLL_HTTP_PARSER_METHOD_END,
    TUCUBE_EPOLL_HTTP_PARSER_URL_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_URL,
    TUCUBE_EPOLL_HTTP_PARSER_URL_END,
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
    ssize_t token_offset;
};

int tucube_epoll_http_parser_parse_message_header(struct tucube_epoll_http_parser* parser, ssize_t buffer_size);
int tucube_epoll_http_parser_parse_message_body(struct tucube_epoll_http_parser* parser, ssize_t buffer_size);

#endif
