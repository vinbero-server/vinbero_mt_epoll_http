#ifndef _TUCUBE_EPOLL_HTTP_H
#define _TUCUBE_EPOLL_HTTP_H

#include <stdbool.h>
#include <gon_http_parser.h>
#include <tucube/tucube_Module.h>
#include <tucube/tucube_ClData.h>
#include "tucube_epoll_http_ResponseBody.h"

struct tucube_epoll_http_Module {
    int (*tucube_epoll_http_Module_init)(struct tucube_Module_Config*, struct tucube_Module_List*);
    int (*tucube_epoll_http_Module_tlInit)(struct tucube_Module*, struct tucube_Module_Config*);
    int (*tucube_epoll_http_Module_clInit)(struct tucube_Module*, struct tucube_ClData_List*, int*);

    int (*tucube_epoll_http_Module_onRequestStart)(void* args[]);
    int (*tucube_epoll_http_Module_onRequestMethod)(char*, ssize_t, void* args[]);
    int (*tucube_epoll_http_Module_onRequestUri)(char*, ssize_t, void* args[]);
    int (*tucube_epoll_http_Module_onRequestProtocol)(char*, ssize_t, void* args[]);
    int (*tucube_epoll_http_Module_onRequestScriptPath)(char*, ssize_t, void* args[]);
    int (*tucube_epoll_http_Module_onRequestContentType)(char*, ssize_t, void* args[]);
    int (*tucube_epoll_http_Module_onRequestContentLength)(char*, ssize_t, void* args[]);
 
    int (*tucube_epoll_http_Module_onRequestHeaderField)(char*, ssize_t, void* args[]);
    int (*tucube_epoll_http_Module_onRequestHeaderValue)(char*, ssize_t, void* args[]);
    int (*tucube_epoll_http_Module_onRequestHeadersFinish)(void* args[]);
    int (*tucube_epoll_http_Module_onRequestBodyStart)(void* args[]);
    int (*tucube_epoll_http_Module_onRequestBody)(char*, ssize_t, void* args[]);
    int (*tucube_epoll_http_Module_onRequestBodyFinish)(void* args[]);
    int (*tucube_epoll_http_Module_onRequestFinish)(void* args[]);
    
    int (*tucube_epoll_http_Module_onGetRequestContentLength)(struct tucube_Module*, struct tucube_ClData*, ssize_t*);
    int (*tucube_epoll_http_Module_onGetRequestIntHeader)(struct tucube_Module*, struct tucube_ClData*, const char* headerField, int* headerValue);
    int (*tucube_epoll_http_Module_onGetRequestDoubleHeader)(struct tucube_Module*, struct tucube_ClData*, const char* headerField, double* headerValue);
    int (*tucube_epoll_http_Module_onGetRequestStringHeader)(struct tucube_Module*, struct tucube_ClData*, const char* headerField, const char** headerValue);

    int (*tucube_epoll_http_Module_onResponseStatusCode)(struct tucube_Module*, struct tucube_ClData*, int*);
    int (*tucube_epoll_http_Module_onResponseHeaderStart)(struct tucube_Module*, struct tucube_ClData*);
    int (*tucube_epoll_http_Module_onResponseHeader)(struct tucube_Module*, struct tucube_ClData*, const char**, size_t*, const char**, size_t*);
    int (*tucube_epoll_http_Module_onResponseBodyStart)(struct tucube_Module*, struct tucube_ClData*);
    int (*tucube_epoll_http_Module_onResponseBody)(struct tucube_Module*, struct tucube_ClData*, struct tucube_epoll_http_ResponseBody*);

    int (*tucube_epoll_http_Module_clDestroy)(struct tucube_Module*, struct tucube_ClData*);
    int (*tucube_epoll_http_Module_tlDestroy)(struct tucube_Module*);
    int (*tucube_epoll_http_Module_destroy)(struct tucube_Module*);

    size_t parserHeaderBufferCapacity;
    size_t parserBodyBufferCapacity;
};

struct tucube_epoll_http_ClData {
    int* clientSocket;
    struct gon_http_parser* parser;
    bool isKeepAlive;
};

int tucube_tcp_epoll_module_init(struct tucube_Module_Config* moduleConfig, struct tucube_Module_List* moduleList);
int tucube_tcp_epoll_module_tlInit(struct tucube_Module* module, struct tucube_Module_Config* moduleConfig);
int tucube_tcp_epoll_module_clInit(struct tucube_Module* module, struct tucube_ClData_List* clDataList, int* clientSocket);
int tucube_tcp_epoll_module_service(struct tucube_Module* module, struct tucube_ClData*);
int tucube_tcp_epoll_module_clDestroy(struct tucube_Module* module, struct tucube_ClData* cldata);
int tucube_tcp_epoll_module_tlDestroy(struct tucube_Module* module);
int tucube_tcp_epoll_module_destroy(struct tucube_Module* module);

#endif
