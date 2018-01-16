#ifndef _VINBERO_IHTTP_H
#define _VINBERO_IHTTP_H

#include <vinbero/vinbero_Module.h>

struct vinbero_IHttp_Response {
    struct gaio_Io* io;
    struct vinbero_IHttp_Response_Methods* methods;
};

struct vinbero_IHttp_Response_Methods {
    int (*writeBytes)(struct vinbero_IHttp_Response* response, char* buffer, size_t bufferSize);
    int (*writeIo)(struct vinbero_IHttp_Response* response, struct gaio_Io* io, size_t writeSize);
    int (*writeCrLf)(struct vinbero_IHttp_Response* response);
    int (*writeVersion)(struct vinbero_IHttp_Response* response, int major, int minor);
    int (*writeStatusCode)(struct vinbero_IHttp_Response* response, int statusCode);
    int (*writeIntHeader)(struct vinbero_IHttp_Response* response, char* headerField, size_t headerFieldSize, int headerValue);
    int (*writeDoubleHeader)(struct vinbero_IHttp_Response* response, char* headerField, size_t headerFieldSize, double headerValue);
    int (*writeStringHeader)(struct vinbero_IHttp_Response* response, char* headerField, size_t headerFieldSize, char* headerValue, size_t headerValueSize);
    int (*writeStringBody)(struct vinbero_IHttp_Response* response, char* stringBody, size_t stringBodySize);
    int (*writeIoBody)(struct vinbero_IHttp_Response* response, struct gaio_Io* ioBody, size_t ioBodySize);
    int (*writeChunkedBodyStart)(struct vinbero_IHttp_Response* response);
    int (*writeChunkedBody)(struct vinbero_IHttp_Response* response, char* stringBody, size_t stringBodySize);
    int (*writeChunkedBodyEnd)(struct vinbero_IHttp_Response* response);
};

#define VINBERO_IHTTP_FUNCTIONS                                                                                                                            \
int vinbero_IHttp_onRequestStart(void* args[]);                                                                                                            \
int vinbero_IHttp_onRequestMethod(char* token, ssize_t tokenSize, void* args[]);                                                                           \
int vinbero_IHttp_onRequestUri(char* token, ssize_t tokenSize, void* args[]);                                                                              \
int vinbero_IHttp_onRequestVersionMajor(int major, void* args[]);                                                                                          \
int vinbero_IHttp_onRequestVersionMinor(int minor, void* args[]);                                                                                          \
int vinbero_IHttp_onRequestScriptPath(char* token, ssize_t tokenSize, void* args[]);                                                                       \
int vinbero_IHttp_onRequestContentType(char* token, ssize_t tokenSize, void* args[]);                                                                      \
int vinbero_IHttp_onRequestContentLength(char* token, ssize_t tokenSize, void* args[]);                                                                    \
int vinbero_IHttp_onRequestHeaderField(char* token, ssize_t tokenSize, void* args[]);                                                                      \
int vinbero_IHttp_onRequestHeaderValue(char* token, ssize_t tokenSize, void* args[]);                                                                      \
int vinbero_IHttp_onRequestHeadersFinish(void* args[]);                                                                                                    \
int vinbero_IHttp_onRequestBodyStart(void* args[]);                                                                                                        \
int vinbero_IHttp_onRequestBody(char* token, ssize_t tokenSize, void* args[]);                                                                             \
int vinbero_IHttp_onRequestBodyFinish(void* args[]);                                                                                                       \
int vinbero_IHttp_onGetRequestContentLength(struct vinbero_Module* module, struct vinbero_ClData* clData, ssize_t* contentLength);                           \
int vinbero_IHttp_onGetRequestIntHeader(struct vinbero_Module* module, struct vinbero_ClData* clData, const char* headerField, int* headerValue);            \
int vinbero_IHttp_onGetRequestDoubleHeader(struct vinbero_Module* module, struct vinbero_ClData* clData, const char* headerField, double* headerValue);      \
int vinbero_IHttp_onGetRequestStringHeader(struct vinbero_Module* module, struct vinbero_ClData* clData, const char* headerField, const char** headerValue); \
int vinbero_IHttp_onRequestFinish(struct vinbero_Module*, struct vinbero_ClData*, void* args[])

#define VINBERO_IHTTP_FUNCTION_POINTERS                                                                                 \
int (*vinbero_IHttp_onRequestStart)(void*[]);                                                                           \
int (*vinbero_IHttp_onRequestMethod)(char*, ssize_t, void*[]);                                                          \
int (*vinbero_IHttp_onRequestUri)(char*, ssize_t, void*[]);                                                             \
int (*vinbero_IHttp_onRequestVersionMajor)(int, void*[]);                                                               \
int (*vinbero_IHttp_onRequestVersionMinor)(int, void*[]);                                                               \
int (*vinbero_IHttp_onRequestScriptPath)(char*, ssize_t, void*[]);                                                      \
int (*vinbero_IHttp_onRequestContentType)(char*, ssize_t, void*[]);                                                     \
int (*vinbero_IHttp_onRequestContentLength)(char*, ssize_t, void*[]);                                                   \
int (*vinbero_IHttp_onRequestHeaderField)(char*, ssize_t, void*[]);                                                     \
int (*vinbero_IHttp_onRequestHeaderValue)(char*, ssize_t, void*[]);                                                     \
int (*vinbero_IHttp_onRequestHeadersFinish)(void*[]);                                                                   \
int (*vinbero_IHttp_onRequestBodyStart)(void*[]);                                                                       \
int (*vinbero_IHttp_onRequestBody)(char*, ssize_t, void*[]);                                                            \
int (*vinbero_IHttp_onRequestBodyFinish)(void*[]);                                                                      \
int (*vinbero_IHttp_onGetRequestContentLength)(struct vinbero_Module*, struct vinbero_ClData*, ssize_t*);                 \
int (*vinbero_IHttp_onGetRequestIntHeader)(struct vinbero_Module*, struct vinbero_ClData*, const char*, int*);            \
int (*vinbero_IHttp_onGetRequestDoubleHeader)(struct vinbero_Module*, struct vinbero_ClData*, const char*, double*);      \
int (*vinbero_IHttp_onGetRequestStringHeader)(struct vinbero_Module*, struct vinbero_ClData*, const char*, const char**); \
int (*vinbero_IHttp_onRequestFinish)(struct vinbero_Module*, struct vinbero_ClData*, void*[])

#define VINBERO_IHTTP_DLSYM(interface, dlHandle, errorVariable)                                       \
do {                                                                                                 \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestStart, errorVariable);            \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestMethod, errorVariable);           \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestUri, errorVariable);              \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestVersionMajor, errorVariable);     \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestVersionMinor, errorVariable);     \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestScriptPath, errorVariable);       \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestContentType, errorVariable);      \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestContentLength, errorVariable);    \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestHeaderField, errorVariable);      \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestHeaderValue, errorVariable);      \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestHeadersFinish, errorVariable);    \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestBodyStart, errorVariable);        \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestBody, errorVariable);             \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestBodyFinish, errorVariable);       \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onGetRequestContentLength, errorVariable); \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onGetRequestIntHeader, errorVariable);     \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onGetRequestDoubleHeader, errorVariable);  \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onGetRequestStringHeader, errorVariable);  \
    if(*errorVariable == 1) break;                                                                    \
    VINBERO_MODULE_DLSYM(interface, dlHandle, vinbero_IHttp_onRequestFinish, errorVariable);           \
    if(*errorVariable == 1) break;                                                                    \
} while(0)

#endif
