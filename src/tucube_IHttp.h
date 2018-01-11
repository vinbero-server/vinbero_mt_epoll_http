#ifndef _TUCUBE_IHTTP_H
#define _TUCUBE_IHTTP_H

#include <tucube/tucube_Module.h>

struct tucube_IHttp_Response {
    struct gaio_Io* io;
    struct tucube_IHttp_Response_Methods* methods;
};

struct tucube_IHttp_Response_Methods {
    int (*writeBytes)(struct tucube_IHttp_Response* response, char* buffer, size_t bufferSize);
    int (*writeIo)(struct tucube_IHttp_Response* response, struct gaio_Io* io, size_t writeSize);
    int (*writeCrLf)(struct tucube_IHttp_Response* response);
    int (*writeVersion)(struct tucube_IHttp_Response* response, int major, int minor);
    int (*writeStatusCode)(struct tucube_IHttp_Response* response, int statusCode);
    int (*writeIntHeader)(struct tucube_IHttp_Response* response, char* headerField, size_t headerFieldSize, int headerValue);
    int (*writeDoubleHeader)(struct tucube_IHttp_Response* response, char* headerField, size_t headerFieldSize, double headerValue);
    int (*writeStringHeader)(struct tucube_IHttp_Response* response, char* headerField, size_t headerFieldSize, char* headerValue, size_t headerValueSize);
    int (*writeStringBody)(struct tucube_IHttp_Response* response, char* stringBody, size_t stringBodySize);
    int (*writeIoBody)(struct tucube_IHttp_Response* response, struct gaio_Io* ioBody, size_t ioBodySize);
    int (*writeChunkedBodyStart)(struct tucube_IHttp_Response* response);
    int (*writeChunkedBody)(struct tucube_IHttp_Response* response, char* stringBody, size_t stringBodySize);
    int (*writeChunkedBodyEnd)(struct tucube_IHttp_Response* response);
};

#define TUCUBE_IHTTP_FUNCTIONS                                                                                                                            \
int tucube_IHttp_onRequestStart(void* args[]);                                                                                                            \
int tucube_IHttp_onRequestMethod(char* token, ssize_t tokenSize, void* args[]);                                                                           \
int tucube_IHttp_onRequestUri(char* token, ssize_t tokenSize, void* args[]);                                                                              \
int tucube_IHttp_onRequestVersionMajor(int major, void* args[]);                                                                                          \
int tucube_IHttp_onRequestVersionMinor(int minor, void* args[]);                                                                                          \
int tucube_IHttp_onRequestScriptPath(char* token, ssize_t tokenSize, void* args[]);                                                                       \
int tucube_IHttp_onRequestContentType(char* token, ssize_t tokenSize, void* args[]);                                                                      \
int tucube_IHttp_onRequestContentLength(char* token, ssize_t tokenSize, void* args[]);                                                                    \
int tucube_IHttp_onRequestHeaderField(char* token, ssize_t tokenSize, void* args[]);                                                                      \
int tucube_IHttp_onRequestHeaderValue(char* token, ssize_t tokenSize, void* args[]);                                                                      \
int tucube_IHttp_onRequestHeadersFinish(void* args[]);                                                                                                    \
int tucube_IHttp_onRequestBodyStart(void* args[]);                                                                                                        \
int tucube_IHttp_onRequestBody(char* token, ssize_t tokenSize, void* args[]);                                                                             \
int tucube_IHttp_onRequestBodyFinish(void* args[]);                                                                                                       \
int tucube_IHttp_onGetRequestContentLength(struct tucube_Module* module, struct tucube_ClData* clData, ssize_t* contentLength);                           \
int tucube_IHttp_onGetRequestIntHeader(struct tucube_Module* module, struct tucube_ClData* clData, const char* headerField, int* headerValue);            \
int tucube_IHttp_onGetRequestDoubleHeader(struct tucube_Module* module, struct tucube_ClData* clData, const char* headerField, double* headerValue);      \
int tucube_IHttp_onGetRequestStringHeader(struct tucube_Module* module, struct tucube_ClData* clData, const char* headerField, const char** headerValue); \
int tucube_IHttp_onRequestFinish(struct tucube_Module*, struct tucube_ClData*, void* args[])

#define TUCUBE_IHTTP_FUNCTION_POINTERS                                                                                 \
int (*tucube_IHttp_onRequestStart)(void*[]);                                                                           \
int (*tucube_IHttp_onRequestMethod)(char*, ssize_t, void*[]);                                                          \
int (*tucube_IHttp_onRequestUri)(char*, ssize_t, void*[]);                                                             \
int (*tucube_IHttp_onRequestVersionMajor)(int, void*[]);                                                               \
int (*tucube_IHttp_onRequestVersionMinor)(int, void*[]);                                                               \
int (*tucube_IHttp_onRequestScriptPath)(char*, ssize_t, void*[]);                                                      \
int (*tucube_IHttp_onRequestContentType)(char*, ssize_t, void*[]);                                                     \
int (*tucube_IHttp_onRequestContentLength)(char*, ssize_t, void*[]);                                                   \
int (*tucube_IHttp_onRequestHeaderField)(char*, ssize_t, void*[]);                                                     \
int (*tucube_IHttp_onRequestHeaderValue)(char*, ssize_t, void*[]);                                                     \
int (*tucube_IHttp_onRequestHeadersFinish)(void*[]);                                                                   \
int (*tucube_IHttp_onRequestBodyStart)(void*[]);                                                                       \
int (*tucube_IHttp_onRequestBody)(char*, ssize_t, void*[]);                                                            \
int (*tucube_IHttp_onRequestBodyFinish)(void*[]);                                                                      \
int (*tucube_IHttp_onGetRequestContentLength)(struct tucube_Module*, struct tucube_ClData*, ssize_t*);                 \
int (*tucube_IHttp_onGetRequestIntHeader)(struct tucube_Module*, struct tucube_ClData*, const char*, int*);            \
int (*tucube_IHttp_onGetRequestDoubleHeader)(struct tucube_Module*, struct tucube_ClData*, const char*, double*);      \
int (*tucube_IHttp_onGetRequestStringHeader)(struct tucube_Module*, struct tucube_ClData*, const char*, const char**); \
int (*tucube_IHttp_onRequestFinish)(struct tucube_Module*, struct tucube_ClData*, void*[])

#define TUCUBE_IHTTP_DLSYM(interface, dlHandle, errorVariable)                                       \
do {                                                                                                 \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestStart, errorVariable);            \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestMethod, errorVariable);           \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestUri, errorVariable);              \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestVersionMajor, errorVariable);     \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestVersionMinor, errorVariable);     \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestScriptPath, errorVariable);       \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestContentType, errorVariable);      \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestContentLength, errorVariable);    \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestHeaderField, errorVariable);      \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestHeaderValue, errorVariable);      \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestHeadersFinish, errorVariable);    \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestBodyStart, errorVariable);        \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestBody, errorVariable);             \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestBodyFinish, errorVariable);       \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onGetRequestContentLength, errorVariable); \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onGetRequestIntHeader, errorVariable);     \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onGetRequestDoubleHeader, errorVariable);  \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onGetRequestStringHeader, errorVariable);  \
    if(*errorVariable == 1) break;                                                                    \
    TUCUBE_MODULE_DLSYM(interface, dlHandle, tucube_IHttp_onRequestFinish, errorVariable);           \
    if(*errorVariable == 1) break;                                                                    \
} while(0)

#endif
