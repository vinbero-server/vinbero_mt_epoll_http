#ifndef _TUCUBE_IHTTP_H
#define _TUCUBE_IHTTP_H

#include <tucube/tucube_Module.h>

#define TUCUBE_IHTTP_FUNCTIONS                                                                                                                                                                       \
int tucube_IHttp_onRequestStart(void* args[]);                                                                                                                                                       \
int tucube_IHttp_onRequestMethod(char* token, ssize_t tokenSize, void* args[]);                                                                                                                      \
int tucube_IHttp_onRequestUri(char* token, ssize_t tokenSize, void* args[]);                                                                                                                         \
int tucube_IHttp_onRequestProtocol(char* token, ssize_t tokenSize, void* args[]);                                                                                                                    \
int tucube_IHttp_onRequestScriptPath(char* token, ssize_t tokenSize, void* args[]);                                                                                                                  \
int tucube_IHttp_onRequestContentType(char* token, ssize_t tokenSize, void* args[]);                                                                                                                 \
int tucube_IHttp_onRequestContentLength(char* token, ssize_t tokenSize, void* args[]);                                                                                                               \
int tucube_IHttp_onRequestHeaderField(char* token, ssize_t tokenSize, void* args[]);                                                                                                                 \
int tucube_IHttp_onRequestHeaderValue(char* token, ssize_t tokenSize, void* args[]);                                                                                                                 \
int tucube_IHttp_onRequestHeadersFinish(void* args[]);                                                                                                                                               \
int tucube_IHttp_onRequestBodyStart(void* args[]);                                                                                                                                                   \
int tucube_IHttp_onRequestBody(char* token, ssize_t tokenSize, void* args[]);                                                                                                                        \
int tucube_IHttp_onRequestBodyFinish(void* args[]);                                                                                                                                                  \
int tucube_IHttp_onRequestFinish(void* args[]);                                                                                                                                                      \
int tucube_IHttp_onGetRequestContentLength(struct tucube_Module* module, struct tucube_ClData* clData, ssize_t* contentLength);                                                                      \
int tucube_IHttp_onGetRequestIntHeader(struct tucube_Module* module, struct tucube_ClData* clData, const char* headerField, int* headerValue);                                                       \
int tucube_IHttp_onGetRequestDoubleHeader(struct tucube_Module* module, struct tucube_ClData* clData, const char* headerField, double* headerValue);                                                 \
int tucube_IHttp_onGetRequestStringHeader(struct tucube_Module* module, struct tucube_ClData* clData, const char* headerField, const char** headerValue);                                            \
int tucube_IHttp_onResponseStatusCode(struct tucube_Module* module, struct tucube_ClData* clData, int* statusCode);                                                                                  \
int tucube_IHttp_onResponseHeaderStart(struct tucube_Module* module, struct tucube_ClData* clData);                                                                                                  \
int tucube_IHttp_onResponseHeader(struct tucube_Module* module, struct tucube_ClData* clData, const char** headerField, size_t* headerFieldSize, const char** headerValue, size_t* headerValueSize); \
int tucube_IHttp_onResponseBodyStart(struct tucube_Module* module, struct tucube_ClData* clData);                                                                                                    \
int tucube_IHttp_onResponseBody(struct tucube_Module* module, struct tucube_ClData* clData, struct tucube_epoll_http_ResponseBody* responseBody)

#define TUCUBE_IHTTP_FUNCTION_POINTERS                                                                                            \
int (*tucube_IHttp_onRequestStart)(void*[]);                                                                                      \
int (*tucube_IHttp_onRequestMethod)(char*, ssize_t, void*[]);                                                                     \
int (*tucube_IHttp_onRequestUri)(char*, ssize_t, void*[]);                                                                        \
int (*tucube_IHttp_onRequestProtocol)(char*, ssize_t, void*[]);                                                                   \
int (*tucube_IHttp_onRequestScriptPath)(char*, ssize_t, void*[]);                                                                 \
int (*tucube_IHttp_onRequestContentType)(char*, ssize_t, void*[]);                                                                \
int (*tucube_IHttp_onRequestContentLength)(char*, ssize_t, void*[]);                                                              \
int (*tucube_IHttp_onRequestHeaderField)(char*, ssize_t, void*[]);                                                                \
int (*tucube_IHttp_onRequestHeaderValue)(char*, ssize_t, void*[]);                                                                \
int (*tucube_IHttp_onRequestHeadersFinish)(void*[]);                                                                              \
int (*tucube_IHttp_onRequestBodyStart)(void*[]);                                                                                  \
int (*tucube_IHttp_onRequestBody)(char*, ssize_t, void*[]);                                                                       \
int (*tucube_IHttp_onRequestBodyFinish)(void*[]);                                                                                 \
int (*tucube_IHttp_onRequestFinish)(void*[]);                                                                                     \
int (*tucube_IHttp_onGetRequestContentLength)(struct tucube_Module*, struct tucube_ClData*, ssize_t*);                            \
int (*tucube_IHttp_onGetRequestIntHeader)(struct tucube_Module*, struct tucube_ClData*, const char*, int*);                       \
int (*tucube_IHttp_onGetRequestDoubleHeader)(struct tucube_Module*, struct tucube_ClData*, const char*, double*);                 \
int (*tucube_IHttp_onGetRequestStringHeader)(struct tucube_Module*, struct tucube_ClData*, const char*, const char**);            \
int (*tucube_IHttp_onResponseStatusCode)(struct tucube_Module*, struct tucube_ClData*, int*);                                     \
int (*tucube_IHttp_onResponseHeaderStart)(struct tucube_Module*, struct tucube_ClData*);                                          \
int (*tucube_IHttp_onResponseHeader)(struct tucube_Module*, struct tucube_ClData*, const char**, size_t*, const char**, size_t*); \
int (*tucube_IHttp_onResponseBodyStart)(struct tucube_Module*, struct tucube_ClData*);                                            \
int (*tucube_IHttp_onResponseBody)(struct tucube_Module*, struct tucube_ClData*, struct tucube_epoll_http_ResponseBody*)

#define TUCUBE_IHTTP_DLSYM(module, modulePointerType)                                   \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestStart);            \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestMethod);           \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestUri);              \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestProtocol);         \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestScriptPath);       \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestContentType);      \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestContentLength);    \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestHeaderField);      \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestHeaderValue);      \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestHeadersFinish);    \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestBodyStart);        \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestBody);             \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestBodyFinish);       \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onRequestFinish);           \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onGetRequestContentLength); \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onGetRequestIntHeader);     \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onGetRequestDoubleHeader);  \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onGetRequestStringHeader);  \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onResponseStatusCode);      \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onResponseHeaderStart);     \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onResponseHeader);          \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onResponseBodyStart);       \
TUCUBE_MODULE_DLSYM(module, modulePointerType, tucube_IHttp_onResponseBody);

#endif
