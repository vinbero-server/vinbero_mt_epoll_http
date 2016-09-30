#ifndef _TUCUBE_EPOLL_HTTP_H
#define _TUCUBE_EPOLL_HTTP_H

#include <tucube/tucube_cldata.h>
#include "tucube_epoll_http_Parser.h"

struct tucube_epoll_http_Module
{
    int (*tucube_epoll_http_Module_init)(struct tucube_module_args*, struct tucube_module_list*);
    int (*tucube_epoll_http_Module_tlinit)(struct tucube_module*, struct tucube_module_args*);
    int (*tucube_epoll_http_Module_clinit)(struct tucube_module*, struct tucube_cldata_list*, int*);

    int (*tucube_epoll_http_Module_onRequestStart)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_Module_onRequestMethod)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_Module_onRequestUri)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_Module_onRequestProtocol)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_Module_onRequestScriptPath)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_Module_onRequestContentType)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_Module_onRequestContentLength)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_Module_onGetRequestContentLength)(struct tucube_module*, struct tucube_cldata*, ssize_t*);

    int (*tucube_epoll_http_Module_onRequestHeaderField)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_Module_onRequestHeaderValue)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_Module_onRequestHeadersFinish)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_Module_onRequestBodyStart)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_Module_onRequestBody)(struct tucube_module*, struct tucube_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_Module_onRequestBodyFinish)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_Module_onRequestFinish)(struct tucube_module*, struct tucube_cldata*);

    int (*tucube_epoll_http_Module_onResponseStatusCode)(struct tucube_module*, struct tucube_cldata*, int*);
    int (*tucube_epoll_http_Module_onResponseHeaderStart)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_Module_onResponseHeader)(struct tucube_module*, struct tucube_cldata*, const char**, size_t*, const char**, size_t*);
    int (*tucube_epoll_http_Module_onResponseBodyStart)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_Module_onResponseBody)(struct tucube_module*, struct tucube_cldata*, const char**, size_t*);

    int (*tucube_epoll_http_Module_cldestroy)(struct tucube_module*, struct tucube_cldata*);
    int (*tucube_epoll_http_Module_tldestroy)(struct tucube_module*);
    int (*tucube_epoll_http_Module_destroy)(struct tucube_module*);

    size_t parserHeaderBufferCapacity;
    size_t parserBodyBufferCapacity;
};

struct tucube_epoll_http_ClientLocalData {
    int* clientSocket;
    struct tucube_epoll_http_Parser* parser;
};

int tucube_tcp_epoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list);
int tucube_tcp_epoll_module_tlinit(struct tucube_module* module, struct tucube_module_args* module_args);
int tucube_tcp_epoll_module_clinit(struct tucube_module* module, struct tucube_cldata_list* cldata_list, int* client_socket);
int tucube_tcp_epoll_module_service(struct tucube_module* module, struct tucube_cldata*);
int tucube_tcp_epoll_module_cldestroy(struct tucube_module* module, struct tucube_cldata* cldata);
int tucube_tcp_epoll_module_tldestroy(struct tucube_module* module);
int tucube_tcp_epoll_module_destroy(struct tucube_module* module);

#endif
