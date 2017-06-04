#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <gon_http_parser.h>
#include <tucube/tucube_Module.h>
#include <tucube/tucube_ClData.h>
#include <libgon_c/gon_c_cast.h>
#include <libgon_c/gon_c_list.h>
#include <libgon_c/gon_c_ltostr.h>
#include "tucube_epoll_http.h"
#include "tucube_epoll_http_ResponseBody.h"

int tucube_tcp_epoll_Module_init(struct tucube_Module_Config* moduleConfig, struct tucube_Module_List* moduleList) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
    if(GON_C_LIST_ELEMENT_NEXT(moduleConfig) == NULL)
        errx(EXIT_FAILURE, "tucube_epoll_http requires another module");

    struct tucube_Module* module = malloc(1 * sizeof(struct tucube_Module));
    GON_C_LIST_ELEMENT_INIT(module);
    module->pointer = malloc(1 * sizeof(struct tucube_epoll_http_Module));

    TUCUBE_MODULE_DLOPEN(module, moduleConfig);

    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_init);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_tlInit);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_clInit);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestStart);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestMethod);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestUri);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestProtocol);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestScriptPath);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestContentType);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestContentLength);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestHeaderField);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestHeaderValue);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestHeadersFinish);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestBodyStart);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestBody);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestBodyFinish);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestFinish);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onGetRequestContentLength);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onGetRequestIntHeader);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onGetRequestDoubleHeader);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onGetRequestStringHeader);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseStatusCode);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseHeaderStart);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseHeader);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseBodyStart);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseBody);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_clDestroy);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_tlDestroy);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_destroy);

    TUCUBE_LOCAL_MODULE->parserHeaderBufferCapacity = 1024;

    if(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserHeaderBufferCapacity") != NULL)
        TUCUBE_LOCAL_MODULE->parserHeaderBufferCapacity = json_integer_value(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserHeaderBufferCapacity"));

    TUCUBE_LOCAL_MODULE->parserBodyBufferCapacity = 1048576;

    if(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserBodyBufferCapacity") != NULL)
        TUCUBE_LOCAL_MODULE->parserBodyBufferCapacity = json_integer_value(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserBodyBufferCapacity"));

    GON_C_LIST_APPEND(moduleList, module);

    if(TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_init(GON_C_LIST_ELEMENT_NEXT(moduleConfig), moduleList) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_epoll_http_Module_init() failed", __FILE__, __LINE__);

    return 0;
#undef TUCUBE_LOCAL_MODULE
}

int tucube_tcp_epoll_Module_tlInit(struct tucube_Module* module, struct tucube_Module_Config* moduleConfig) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
    TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_tlInit(GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(moduleConfig));
    return 0;
#undef TUCUBE_LOCAL_MODULE
}

int tucube_tcp_epoll_Module_clInit(struct tucube_Module* module, struct tucube_ClData_List* clDataList, int* clientSocket) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLDATA GON_C_CAST(clData->pointer, struct tucube_epoll_http_ClData*)
 
    struct tucube_ClData* clData = malloc(1 * sizeof(struct tucube_ClData));
    GON_C_LIST_ELEMENT_INIT(clData);
    clData->pointer = malloc(1 * sizeof(struct tucube_epoll_http_ClData));

    TUCUBE_LOCAL_CLDATA->clientSocket = clientSocket;
    TUCUBE_LOCAL_CLDATA->isKeepAlive = false;

    TUCUBE_LOCAL_CLDATA->parser = malloc(1 * sizeof(struct gon_http_parser));
    gon_http_parser_init(
        TUCUBE_LOCAL_CLDATA->parser,
        TUCUBE_LOCAL_MODULE->parserHeaderBufferCapacity,
        TUCUBE_LOCAL_MODULE->parserBodyBufferCapacity
    );
    TUCUBE_LOCAL_CLDATA->parser->onRequestStart = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestStart;
    TUCUBE_LOCAL_CLDATA->parser->onRequestMethod = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestMethod;
    TUCUBE_LOCAL_CLDATA->parser->onRequestUri = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestUri;
    TUCUBE_LOCAL_CLDATA->parser->onRequestProtocol = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestProtocol;
    TUCUBE_LOCAL_CLDATA->parser->onRequestScriptPath = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestScriptPath;
    TUCUBE_LOCAL_CLDATA->parser->onRequestContentType = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestContentType;
    TUCUBE_LOCAL_CLDATA->parser->onRequestContentLength = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestContentLength;

    TUCUBE_LOCAL_CLDATA->parser->onRequestHeaderField = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestHeaderField;
    TUCUBE_LOCAL_CLDATA->parser->onRequestHeaderValue = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestHeaderValue;
    TUCUBE_LOCAL_CLDATA->parser->onRequestHeadersFinish = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestHeadersFinish;
    TUCUBE_LOCAL_CLDATA->parser->onRequestBodyStart = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestBodyStart;
    TUCUBE_LOCAL_CLDATA->parser->onRequestBody = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestBody;
    TUCUBE_LOCAL_CLDATA->parser->onRequestBodyFinish = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestBodyFinish;
    TUCUBE_LOCAL_CLDATA->parser->onRequestFinish = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onRequestFinish;

    GON_C_LIST_APPEND(clDataList, clData);

    TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_clInit(GON_C_LIST_ELEMENT_NEXT(module), clDataList, clientSocket);
    return 0;
#undef TUCUBE_LOCAL_CLDATA
#undef TUCUBE_LOCAL_MODULE
}

static inline int tucube_epoll_http_readRequest(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLDATA GON_C_CAST(clData->pointer, struct tucube_epoll_http_ClData*)
    ssize_t readSize;

    while((readSize = gon_http_parser_read(
               TUCUBE_LOCAL_CLDATA->parser,
               TUCUBE_LOCAL_CLDATA->clientSocket)) > 0) {
        int result;
        if((result = gon_http_parser_parse(TUCUBE_LOCAL_CLDATA->parser, readSize, (void*[]){GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(clData)})) == -1) {
            warnx("%s: %u: Parser error", __FILE__, __LINE__);
            return -1;
        } else if(result == 0)
            break;
    }
    if(readSize == -1) {
        if(errno == EAGAIN) {
//            warnx("%s: %u: Client socket EAGAIN", __FILE__, __LINE__);
            return 1;
        } else if(errno == EWOULDBLOCK) {
            warnx("%s: %u: Client socket EWOULDBLOCK", __FILE__, __LINE__);
            return 1;
        } else
            warn("%s: %u", __FILE__, __LINE__);
        return -1;
    }
    else if(readSize == 0) {
        warnx("%s: %u: Client socket has been closed", __FILE__, __LINE__);
        return -1;
    }
    
    const char* connectionHeaderValue;
    if(TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onGetRequestStringHeader(
              GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(clData), "Connection", &connectionHeaderValue
      ) != -1) {
        if(strncasecmp(connectionHeaderValue, "Keep-Alive", sizeof("Keep-Alive")) == 0) {
            warnx("%s: %u: Keep-Alive Connection", __FILE__, __LINE__);
            TUCUBE_LOCAL_CLDATA->isKeepAlive = false;
            gon_http_parser_reset(TUCUBE_LOCAL_CLDATA->parser);
            return 2;
        }
        TUCUBE_LOCAL_CLDATA->isKeepAlive = false;
    }
    return 0; // request finsihed
#undef TUCUBE_LOCAL_CLDATA
#undef TUCUBE_LOCAL_MODULE
}

static inline int tucube_epoll_http_writeCrlf(int clientSocket) {
    return write(clientSocket, "\r\n", sizeof("\r\n") - 1);
}

static inline int tucube_epoll_http_writeStatusCode(int clientSocket, int statusCode) {
    write(clientSocket, "HTTP/1.1", sizeof("HTTP/1.1") - 1);
    write(clientSocket, " ", sizeof(" ") - 1);

    char* statusCodeString;
    size_t statusCodeStringLength = gon_c_ltostr(statusCode, 10, &statusCodeString);
    write(clientSocket, statusCodeString, statusCodeStringLength);
    free(statusCodeString);

    write(clientSocket, " ", sizeof(" ") - 1); // reason phrase is optional but blank space after status code is necessary

    tucube_epoll_http_writeCrlf(clientSocket);
    return 0;
}

static inline int tucube_epoll_http_writeHeader(int clientSocket, const char* headerField, size_t headerFieldSize, const char* headerValue, size_t headerValueSize) {
    write(clientSocket, headerField, headerFieldSize);
    write(clientSocket, ": ", sizeof(": ") - 1);
    write(clientSocket, headerValue, headerValueSize);
    tucube_epoll_http_writeCrlf(clientSocket);
    return 0;
}

static inline int tucube_epoll_http_writeHeaders(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLDATA GON_C_CAST(clData->pointer, struct tucube_epoll_http_ClData*)
    int result;
    if((result = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onResponseHeaderStart(GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(clData)) <= 0)) {
        return result;
    }
    if(GON_C_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->isKeepAlive)
        tucube_epoll_http_writeHeader(*TUCUBE_LOCAL_CLDATA->clientSocket, "Connection", sizeof("Connection") - 1, "Keep-Alive", sizeof("Keep-Alive") - 1);
    do {
        const char* headerField;
        size_t headerFieldSize;
        const char* headerValue;
        size_t headerValueSize;
        
        if((result = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onResponseHeader(GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(clData), &headerField, &headerFieldSize, &headerValue, &headerValueSize)) == -1) {
            return -1;
        }
        tucube_epoll_http_writeHeader(*TUCUBE_LOCAL_CLDATA->clientSocket, headerField, headerFieldSize, headerValue, headerValueSize);
    }
    while(result == 1);
    return 0;
#undef TUCUBE_LOCAL_CLDATA
#undef TUCUBE_LOCAL_MODULE
}

static inline int tucube_epoll_http_writeContentLength(int clientSocket, size_t bodySize) {
    char* bodySizeString;
    size_t bodySizeStringSize;
    bodySizeStringSize = gon_c_ltostr(bodySize, 10, &bodySizeString);
    tucube_epoll_http_writeHeader(clientSocket, "Content-Length", sizeof("Content-Length") - 1, bodySizeString, bodySizeStringSize);
    free(bodySizeString);
    tucube_epoll_http_writeCrlf(clientSocket);
    return 0;
}

static inline int tucube_epoll_http_writeBody(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLDATA GON_C_CAST(clData->pointer, struct tucube_epoll_http_ClData*)
    switch(TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onResponseBodyStart(GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(clData))) {
        case -1:
            return -1;
        case 0:
            tucube_epoll_http_writeCrlf(*TUCUBE_LOCAL_CLDATA->clientSocket);
            return 0;
    }

    int result;
    struct tucube_epoll_http_ResponseBody body;
    switch(TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onResponseBody(GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(clData), &body)) {
        case -1:
            return -1;
        case 0:
            tucube_epoll_http_writeContentLength(*TUCUBE_LOCAL_CLDATA->clientSocket, body.size);
            switch(body.type) {
                case TUCUBE_EPOLL_HTTP_RESPONSE_BODY_STRING:
                    write(*TUCUBE_LOCAL_CLDATA->clientSocket, body.chars, body.size);
                    break;
                case TUCUBE_EPOLL_HTTP_RESPONSE_BODY_FILE:
                    sendfile(*TUCUBE_LOCAL_CLDATA->clientSocket, body.fd, NULL, body.size);
                    break;
                default:
                    return -1;
            }
            return 0;
        case 1:
            if(body.type != TUCUBE_EPOLL_HTTP_RESPONSE_BODY_STRING) {
                if(body.type == TUCUBE_EPOLL_HTTP_RESPONSE_BODY_FILE)
                    warnx("%s: %u", __FILE__, __LINE__);
                return -1;
            }
            tucube_epoll_http_writeHeader(*TUCUBE_LOCAL_CLDATA->clientSocket, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1, "chunked", sizeof("chunked"));
            tucube_epoll_http_writeCrlf(*TUCUBE_LOCAL_CLDATA->clientSocket);

            tucube_epoll_http_writeContentLength(*TUCUBE_LOCAL_CLDATA->clientSocket, body.size);

            write(*TUCUBE_LOCAL_CLDATA->clientSocket, body.chars, body.size);
            tucube_epoll_http_writeCrlf(*TUCUBE_LOCAL_CLDATA->clientSocket);
            do {
                if((result = TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onResponseBody(GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(clData), &body)) == -1) {
                    return -1;
                }
                tucube_epoll_http_writeContentLength(*TUCUBE_LOCAL_CLDATA->clientSocket, body.size);
                write(*TUCUBE_LOCAL_CLDATA->clientSocket, body.chars, body.size);
                tucube_epoll_http_writeCrlf(*TUCUBE_LOCAL_CLDATA->clientSocket);
            }
            while(result == 1);
            tucube_epoll_http_writeCrlf(*TUCUBE_LOCAL_CLDATA->clientSocket);
            return 0;
        default:
            return -1;
    }
#undef TUCUBE_LOCAL_CLDATA
#undef TUCUBE_LOCAL_MODULE
}

static inline int tucube_epoll_http_writeResponse(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLDATA GON_C_CAST(clData->pointer, struct tucube_epoll_http_ClData*)
    int statusCode;
    if(TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_onResponseStatusCode(GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(clData), &statusCode) == -1) {
        return -1;
    }

    tucube_epoll_http_writeStatusCode(*TUCUBE_LOCAL_CLDATA->clientSocket, statusCode);

    if(tucube_epoll_http_writeHeaders(module, clData) == -1)
        return -1;

    if(tucube_epoll_http_writeBody(module, clData) == -1)
        return -1;

    return 0; 
#undef TUCUBE_LOCAL_CLDATA
#undef TUCUBE_LOCAL_MODULE
}

int tucube_tcp_epoll_Module_service(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLDATA GON_C_CAST(clData->pointer, struct tucube_epoll_http_ClData*)
    register int result = tucube_epoll_http_readRequest(module, clData);

    if(result != 0 && result != 2)
        return result;

    if(fcntl(*TUCUBE_LOCAL_CLDATA->clientSocket, F_SETFL, fcntl(*TUCUBE_LOCAL_CLDATA->clientSocket, F_GETFL, 0) & ~O_NONBLOCK) == -1)
        return -1;

    if(tucube_epoll_http_writeResponse(module, clData) == -1)
        return -1;

    if(fcntl(*TUCUBE_LOCAL_CLDATA->clientSocket, F_SETFL, fcntl(*TUCUBE_LOCAL_CLDATA->clientSocket, F_GETFL, 0) | O_NONBLOCK) == -1)
        return -1;
        
    if(result == 2)
        return 1;

    return 0;
#undef TUCUBE_LOCAL_CLDATA
#undef TUCUBE_LOCAL_MODULE
}

int tucube_tcp_epoll_Module_clDestroy(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLDATA GON_C_CAST(clData->pointer, struct tucube_epoll_http_ClData*)
    TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_clDestroy(GON_C_LIST_ELEMENT_NEXT(module), GON_C_LIST_ELEMENT_NEXT(clData));
    free(TUCUBE_LOCAL_CLDATA->parser->buffer);
    free(TUCUBE_LOCAL_CLDATA->parser);
    close(*(int*)TUCUBE_LOCAL_CLDATA->clientSocket);
    *(int*)TUCUBE_LOCAL_CLDATA->clientSocket = -1;
    free(clData->pointer);
    free(clData);
    return 0;
#undef TUCUBE_LOCAL_CLDATA
#undef TUCUBE_LOCAL_MODULE
}

int tucube_tcp_epoll_Module_tlDestroy(struct tucube_Module* module) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
    TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_tlDestroy(GON_C_LIST_ELEMENT_NEXT(module));
    return 0;
#undef TUCUBE_LOCAL_MODULE
}

int tucube_tcp_epoll_Module_destroy(struct tucube_Module* module) {
#define TUCUBE_LOCAL_MODULE GON_C_CAST(module->pointer, struct tucube_epoll_http_Module*)
    TUCUBE_LOCAL_MODULE->tucube_epoll_http_Module_destroy(GON_C_LIST_ELEMENT_NEXT(module));
//    dlclose(module->dl_handle);
    free(module->pointer);
    free(module);
    return 0;
#undef TUCUBE_LOCAL_MODULE
}
