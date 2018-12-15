#ifndef _VINBERO_INTERFACE_HTTP_H
#define _VINBERO_INTERFACE_HTTP_H

#include <vinbero_common/vinbero_common_Module.h>
#include <vinbero_common/vinbero_common_ClModule.h>

struct vinbero_interface_HTTP_Response {
    struct vinbero_common_ClModule* clModule;
    struct gaio_Io* io;
    struct vinbero_interface_HTTP_Response_Methods* methods;
};

struct vinbero_interface_HTTP_Response_Methods {
    int
    (*writeBytes)
    (struct vinbero_interface_HTTP_Response* response, const char* buffer,
     size_t bufferSize);

    int
    (*writeIo)
    (struct vinbero_interface_HTTP_Response* response, struct gaio_Io* io,
     size_t writeSize);

    int
    (*writeCrLf)
    (struct vinbero_interface_HTTP_Response* response);

    int
    (*writeVersion)
    (struct vinbero_interface_HTTP_Response* response, int major, int minor);

    int
    (*writeStatusCode)
    (struct vinbero_interface_HTTP_Response* response, int statusCode);

    int
    (*writeIntHeader)
    (struct vinbero_interface_HTTP_Response* response, const char* headerField,
     size_t headerFieldSize, int headerValue);

    int
    (*writeDoubleHeader)
    (struct vinbero_interface_HTTP_Response* response, const char* headerField,
     size_t headerFieldSize, double headerValue);

    int
    (*writeStringHeader)
    (struct vinbero_interface_HTTP_Response* response, const char* headerField,
     size_t headerFieldSize, const char* headerValue, size_t headerValueSize);

    int
    (*writeStringBody)
    (struct vinbero_interface_HTTP_Response* response, const char* stringBody,
     size_t stringBodySize);

    int
    (*writeIoBody)
    (struct vinbero_interface_HTTP_Response* response, struct gaio_Io* ioBody,
     size_t ioBodySize);

    int
    (*writeChunkedBodyStart)
    (struct vinbero_interface_HTTP_Response* response);

    int
    (*writeChunkedBody)
    (struct vinbero_interface_HTTP_Response* response, const char* stringBody,
     size_t stringBodySize); 

    int
    (*writeChunkedBodyEnd)
    (struct vinbero_interface_HTTP_Response* response);
};

#define VINBERO_INTERFACE_HTTP_FUNCTIONS \
int \
vinbero_interface_HTTP_onRequestStart \
(struct vinbero_common_ClModule* clModule); \
\
int \
vinbero_interface_HTTP_onRequestMethod \
(struct vinbero_common_ClModule* clModule, \
 const char* token, size_t tokenSize); \
\
int \
vinbero_interface_HTTP_onRequestUri \
(struct vinbero_common_ClModule* clModule, \
 const char* token, size_t tokenSize); \
\
int \
vinbero_interface_HTTP_onRequestVersionMajor \
(struct vinbero_common_ClModule* clModule, \
 int major); \
\
int \
vinbero_interface_HTTP_onRequestVersionMinor \
(struct vinbero_common_ClModule* clModule, \
 int minor); \
\
int \
vinbero_interface_HTTP_onRequestContentLength \
(struct vinbero_common_ClModule* clModule, \
 size_t contentLength); \
\
int \
vinbero_interface_HTTP_onRequestContentType \
(struct vinbero_common_ClModule* clModule, \
 const char* token, size_t tokenSize); \
\
int \
vinbero_interface_HTTP_onRequestScriptPath \
(struct vinbero_common_ClModule* clModule, \
 const char* token, size_t tokenSize); \
\
int \
vinbero_interface_HTTP_onRequestKeepAlive \
(struct vinbero_common_ClModule* clModule, \
 bool isKeepAlive); \
\
int \
vinbero_interface_HTTP_onRequestHeaderField \
(struct vinbero_common_ClModule* clModule, \
 const char* token, size_t tokenSize); \
\
int \
vinbero_interface_HTTP_onRequestHeaderValue \
(struct vinbero_common_ClModule* clModule, \
 const char* token, size_t tokenSize); \
\
int \
vinbero_interface_HTTP_onRequestHeadersFinish \
(struct vinbero_common_ClModule* clModule); \
\
int \
vinbero_interface_HTTP_onRequestBodyStart \
(struct vinbero_common_ClModule* clModule); \
\
int \
vinbero_interface_HTTP_onRequestBody \
(struct vinbero_common_ClModule* clModule, \
 const char* token, size_t tokenSize); \
\
int \
vinbero_interface_HTTP_onRequestBodyFinish \
(struct vinbero_common_ClModule* clModule); \
\
int \
vinbero_interface_HTTP_onGetRequestIntHeader \
(struct vinbero_common_ClModule* clModule, \
 const char* headerField, int* headerValue); \
\
int \
vinbero_interface_HTTP_onGetRequestDoubleHeader \
(struct vinbero_common_ClModule* clModule, \
 const char* headerField, double* headerValue); \
\
int \
vinbero_interface_HTTP_onGetRequestStringHeader \
(struct vinbero_common_ClModule* clModule, \
 const char* headerField, const char** headerValue); \
\
int \
vinbero_interface_HTTP_onGetRequestContentLength \
(struct vinbero_common_ClModule* clModule, \
size_t* contentLength); \
\
int \
vinbero_interface_HTTP_onRequestFinish \
(struct vinbero_common_ClModule* clModule)

#define VINBERO_INTERFACE_HTTP_FUNCTION_POINTERS \
int \
(*vinbero_interface_HTTP_onRequestStart) \
(struct vinbero_common_ClModule*); \
\
int \
(*vinbero_interface_HTTP_onRequestMethod) \
(struct vinbero_common_ClModule*, const char*, size_t); \
\
int \
(*vinbero_interface_HTTP_onRequestUri) \
(struct vinbero_common_ClModule*, const char*, size_t); \
\
int \
(*vinbero_interface_HTTP_onRequestVersionMajor) \
(struct vinbero_common_ClModule*, int); \
\
int \
(*vinbero_interface_HTTP_onRequestVersionMinor) \
(struct vinbero_common_ClModule*, int); \
\
int \
(*vinbero_interface_HTTP_onRequestHeaderField) \
(struct vinbero_common_ClModule*, const char*, size_t); \
\
int \
(*vinbero_interface_HTTP_onRequestHeaderValue) \
(struct vinbero_common_ClModule*, const char*, size_t); \
\
int \
(*vinbero_interface_HTTP_onRequestContentLength) \
(struct vinbero_common_ClModule*, size_t); \
\
int \
(*vinbero_interface_HTTP_onRequestContentType) \
(struct vinbero_common_ClModule*, const char*, size_t); \
\
int \
(*vinbero_interface_HTTP_onRequestScriptPath) \
(struct vinbero_common_ClModule*, const char*, size_t); \
\
int \
(*vinbero_interface_HTTP_onRequestKeepAlive) \
(struct vinbero_common_ClModule*, bool); \
\
int \
(*vinbero_interface_HTTP_onRequestHeadersFinish) \
(struct vinbero_common_ClModule*); \
\
int \
(*vinbero_interface_HTTP_onRequestBodyStart) \
(struct vinbero_common_ClModule*); \
int \
(*vinbero_interface_HTTP_onRequestBody) \
(struct vinbero_common_ClModule*, const char*, size_t); \
\
int \
(*vinbero_interface_HTTP_onRequestBodyFinish) \
(struct vinbero_common_ClModule*); \
\
int \
(*vinbero_interface_HTTP_onGetRequestIntHeader) \
(struct vinbero_common_ClModule*, const char*, int*); \
\
int \
(*vinbero_interface_HTTP_onGetRequestDoubleHeader) \
(struct vinbero_common_ClModule*, const char*, double*); \
\
int \
(*vinbero_interface_HTTP_onGetRequestStringHeader) \
(struct vinbero_common_ClModule*, const char*, const char**); \
\
int \
(*vinbero_interface_HTTP_onGetRequestContentLength) \
(struct vinbero_common_ClModule*, size_t*); \
\
int \
(*vinbero_interface_HTTP_onRequestFinish) \
(struct vinbero_common_ClModule*)

struct vinbero_interface_HTTP {
    VINBERO_INTERFACE_HTTP_FUNCTION_POINTERS;
};

#define VINBERO_INTERFACE_HTTP_DLSYM(interface, dlHandle, ret) do { \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestStart, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestMethod, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestUri, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestVersionMajor, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestVersionMinor, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestContentLength, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestContentType, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestScriptPath, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestKeepAlive, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestHeaderField, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestHeaderValue, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestHeadersFinish, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestBodyStart, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestBody, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestBodyFinish, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onGetRequestIntHeader, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onGetRequestDoubleHeader, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onGetRequestStringHeader, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onGetRequestContentLength, ret); \
    if(*ret < 0) break; \
    VINBERO_COMMON_MODULE_DLSYM \
    (interface, dlHandle, vinbero_interface_HTTP_onRequestFinish, ret); \
    if(*ret < 0) break; \
} while(0)

#endif
