#ifndef _TCPCUBE_EPOLL_HTTP_PARSER_H
#define _TCPCUBE_EPOLL_HTTP_PARSER_H

enum tcpcube_epoll_http_parser_state
{
    TCPCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN,
    TCPCUBE_EPOLL_HTTP_PARSER_METHOD,
    TCPCUBE_EPOLL_HTTP_PARSER_METHOD_END,
    TCPCUBE_EPOLL_HTTP_PARSER_URL_BEGIN,
    TCPCUBE_EPOLL_HTTP_PARSER_URL,
    TCPCUBE_EPOLL_HTTP_PARSER_URL_END,
    TCPCUBE_EPOLL_HTTP_PARSER_VERSION_BEGIN,
    TCPCUBE_EPOLL_HTTP_PARSER_VERSION,
    TCPCUBE_EPOLL_HTTP_PARSER_VERSION_END,
    TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN,
    TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD,
    TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END,
    TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN,
    TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE,
    TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END,
    TCPCUBE_EPOLL_HTTP_PARSER_HEADER_END,
    TCPCUBE_EPOLL_HTTP_PARSER_ERROR
};

struct tcpcube_epoll_http_parser
{
    enum tcpcube_epoll_http_parser_state state;
    char* buffer;
    ssize_t buffer_offset;
    ssize_t buffer_capacity;
    char* token;
    ssize_t token_offset;
};

int tcpcube_epoll_http_parser_parse_message_header(struct tcpcube_epoll_http_parser* parser, ssize_t buffer_size);
int tcpcube_epoll_http_parser_parse_message_body(struct tcpcube_epoll_http_parser* parser, ssize_t buffer_size);

#endif
