#ifndef _TUCUBE_EPOLL_HTTP_PARSER_H
#define _TUCUBE_EPOLL_HTTP_PARSER_H

#include <limits.h>
#include <stdbool.h>
#include <tucube/tucube_Module.h>
#include <tucube/tucube_ClData.h>

enum tucube_epoll_http_Parser_state {
    TUCUBE_EPOLL_HTTP_PARSER_HEADERS_BEGIN,
    TUCUBE_EPOLL_HTTP_PARSER_METHOD,
    TUCUBE_EPOLL_HTTP_PARSER_METHOD_END,
    TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI,
    TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI_END,
    TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL,
    TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_END,
    TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH,
    TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH_END,
    TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE,
    TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE_END,
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
    int (*parse)(struct tucube_epoll_http_Parser*, ssize_t readSize);
    size_t headerBufferCapacity;
    size_t bodyBufferCapacity;
    char* buffer;
    size_t bufferOffset;
    size_t bufferSize;
    char* token;
    size_t tokenOffset;
    ssize_t bodyRemainder;
    ssize_t contentLength;

    int (*onRequestStart)(void* args[]);
    int (*onRequestMethod)(char*, ssize_t, void* args[]);
    int (*onRequestUri)(char*, ssize_t, void* args[]);
    int (*onRequestProtocol)(char*, ssize_t, void* args[]);
    int (*onRequestScriptPath)(char*, ssize_t, void* args[]);
    int (*onRequestContentType)(char*, ssize_t, void* args[]);
    int (*onRequestContentLength)(char*, ssize_t, void* args[]);

    int (*onRequestHeaderField)(char*, ssize_t, void* args[]);
    int (*onRequestHeaderValue)(char*, ssize_t, void* args[]);
    int (*onRequestHeadersFinish)(void* args[]);
    int (*onRequestBodyStart)(void* args[]);
    int (*onRequestBody)(char*, ssize_t, void* args[]);
    int (*onRequestBodyFinish)(void* args[]);
    int (*onRequestFinish)(void* args[]);
};

int tucube_epoll_http_Parser_init(struct tucube_epoll_http_Parser* parser, size_t headerBufferCapacity, size_t bodyBufferCapacity);

int tucube_epoll_http_Parser_reset(struct tucube_epoll_http_Parser* parser);

int tucube_epoll_http_Parser_read(struct tucube_epoll_http_Parser* parser, int* clientSocket);

int tucube_epoll_http_Parser_parse(struct tucube_epoll_http_Parser* parser, ssize_t readSize, void* args[]);

#endif
