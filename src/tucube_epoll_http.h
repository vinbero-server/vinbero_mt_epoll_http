#ifndef _TUCUBE_EPOLL_HTTP_H
#define _TUCUBE_EPOLL_HTTP_H

#include <tucube/tucube_cldata.h>
#include "tucube_epoll_http_parser.h"

struct tucube_epoll_http_module
{
    int (*tucube_epoll_http_module_init)(struct tucube_module_args*, struct tucube_module_list*);
    int (*tucube_epoll_http_module_tlinit)(struct tucube_module*, struct tucube_module_args*);
    int (*tucube_epoll_http_module_clinit)(struct tucube_module*, struct tucube_cldata_list*, int*);

    int (*tucube_epoll_http_module_onRequestStart)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_module_onRequestMethod)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_onRequestUri)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_onRequestProtocol)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_onRequestScriptPath)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_onRequestContentType)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_onRequestContentLength)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_onGetRequestContentLength)(struct tucube_module*, struct tucube_cldata*, ssize_t*);

    int (*tucube_epoll_http_module_onRequestHeaderField)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_onRequestHeaderValue)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_onRequestHeadersFinish)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_module_onRequestBodyStart)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_module_onRequestBody)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_onRequestBodyFinish)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_module_onRequestFinish)(struct tucube_module*, struct tucube_cldata*);

    int (*tucube_epoll_http_module_onResponseStatusCode)(struct tucube_module*, struct tucube_cldata*, int*);
    int (*tucube_epoll_http_module_onResponseHeaderStart)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_module_onResponseHeader)(struct tucube_module*, struct tucube_cldata*, const char**, size_t*, const char**, size_t*);
    int (*tucube_epoll_http_module_onResponseBodyStart)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_module_onResponseBody)(struct tucube_module*, struct tucube_cldata*, const char**, size_t*);

    int (*tucube_epoll_http_module_cldestroy)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_module_tldestroy)(struct tucube_module*);
    int (*tucube_epoll_http_module_destroy)(struct tucube_module*);

    size_t parser_header_buffer_capacity;
    size_t parser_body_buffer_capacity;
};

struct tucube_epoll_http_cldata
{
    int* client_socket;
    struct tucube_epoll_http_parser* parser;
};

int tucube_tcp_epoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list);
int tucube_tcp_epoll_module_tlinit(struct tucube_module* module, struct tucube_module_args* module_args);
int tucube_tcp_epoll_module_clinit(struct tucube_module* module, struct tucube_cldata_list* cldata_list, int* client_socket);
int tucube_tcp_epoll_module_service(struct tucube_module* module, struct tucube_cldata*);
int tucube_tcp_epoll_module_cldestroy(struct tucube_module* module, struct tucube_cldata* cldata);
int tucube_tcp_epoll_module_tldestroy(struct tucube_module* module);
int tucube_tcp_epoll_module_destroy(struct tucube_module* module);

#endif
