#ifndef _TUCUBE_EPOLL_HTTP_PARSER_H
#define _TUCUBE_EPOLL_HTTP_PARSER_H

#include <stdbool.h>
#include <tucube/tucube_Module.h>
#include <tucube/tucube_ClData.h>

enum tucube_epoll_http_Parser_state {
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

struct tucube_epoll_http_Parser {
    enum tucube_epoll_http_Parser_state state;
    size_t headerBufferCapacity;
    size_t bodyBufferCapacity;
    char* buffer;
    size_t bufferOffset;
    size_t bufferSize;
    char* token;
    size_t tokenOffset;
    ssize_t bodyRemainder;
    bool isKeepAlive;
};

int tucube_epoll_http_Parser_init(struct tucube_epoll_http_Parser* parser, size_t headerBufferCapacity, size_t bodyBufferCapacity);

int tucube_epoll_http_Parser_reset(struct tucube_epoll_http_Parser* parser);

char* tucube_epoll_http_Parser_getBufferPosition(struct tucube_epoll_http_Parser* parser);

size_t tucube_epoll_http_Parser_getAvailableBufferSize(struct tucube_epoll_http_Parser* parser);

int tucube_epoll_http_Parser_parse(struct tucube_Module* module, struct tucube_ClData* clData, struct tucube_epoll_http_Parser* parser, ssize_t readSize);

#endif
