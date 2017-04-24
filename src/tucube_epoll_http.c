#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <tucube/tucube_Module.h>
#include <tucube/tucube_ClData.h>
#include <libgonc/gonc_cast.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_ltostr.h>
#include "tucube_epoll_http.h"
#include "tucube_epoll_http_Parser.h"
#include "tucube_epoll_http_ResponseBody.h"

int tucube_tcp_epoll_Module_init(struct tucube_Module_Config* moduleConfig, struct tucube_Module_List* moduleList) {
    if(GONC_LIST_ELEMENT_NEXT(moduleConfig) == NULL)
        errx(EXIT_FAILURE, "tucube_epoll_http requires another module");

    struct tucube_Module* module = malloc(1 * sizeof(struct tucube_Module));
    GONC_LIST_ELEMENT_INIT(module);
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

    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->parserHeaderBufferCapacity = 1024;

    if(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserHeaderBufferCapacity") != NULL)
        GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->parserHeaderBufferCapacity = json_integer_value(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserHeaderBufferCapacity"));

    GONC_CAST(module->pointer,
            struct tucube_epoll_http_Module*)->parserBodyBufferCapacity = 1048576;

    if(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserBodyBufferCapacity") != NULL)
        GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->parserBodyBufferCapacity = json_integer_value(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserBodyBufferCapacity"));

    GONC_LIST_APPEND(moduleList, module);

    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_init(GONC_LIST_ELEMENT_NEXT(moduleConfig), moduleList) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_epoll_http_Module_init() failed", __FILE__, __LINE__);

    return 0;
}

int tucube_tcp_epoll_Module_tlInit(struct tucube_Module* module, struct tucube_Module_Config* moduleConfig) {
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_tlInit(GONC_LIST_ELEMENT_NEXT(module),
              GONC_LIST_ELEMENT_NEXT(moduleConfig));
    return 0;
}

int tucube_tcp_epoll_Module_clInit(struct tucube_Module* module, struct tucube_ClData_List* clDataList, int* clientSocket) {
    struct tucube_ClData* clData = malloc(1 * sizeof(struct tucube_ClData));
    GONC_LIST_ELEMENT_INIT(clData);
    clData->pointer = malloc(1 * sizeof(struct tucube_epoll_http_ClData));

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->clientSocket = clientSocket;
    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser = calloc(1, sizeof(struct tucube_epoll_http_Parser));

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->headerBufferCapacity =
              GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->parserHeaderBufferCapacity;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->bodyBufferCapacity =
              GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->parserBodyBufferCapacity;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->buffer =
              malloc(GONC_CAST(clData->pointer,
                   struct tucube_epoll_http_ClData*)->parser->headerBufferCapacity * sizeof(char));

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestStart = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestStart;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestMethod = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestMethod;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestUri = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestUri;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestProtocol = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestProtocol;

    GONC_CAST(clData->pointer,
            struct tucube_epoll_http_ClData*)->parser->onRequestScriptPath = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestScriptPath;

    GONC_CAST(clData->pointer,
            struct tucube_epoll_http_ClData*)->parser->onRequestContentType = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestContentType;

    GONC_CAST(clData->pointer,
            struct tucube_epoll_http_ClData*)->parser->onRequestContentLength = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestContentLength;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onGetRequestContentLength = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onGetRequestContentLength;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestHeaderField = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestHeaderField;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestHeaderValue = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestHeaderValue;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestHeadersFinish = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestHeadersFinish;

    GONC_CAST(clData->pointer,
            struct tucube_epoll_http_ClData*)->parser->onRequestBodyStart = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBodyStart;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestBody = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBody;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestBodyFinish = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBodyFinish;

    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->parser->onRequestFinish = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestFinish;
         
    GONC_CAST(clData->pointer,
         struct tucube_epoll_http_ClData*)->isKeepAlive = false;

    GONC_LIST_APPEND(clDataList, clData);

    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_clInit(GONC_LIST_ELEMENT_NEXT(module),
              clDataList, clientSocket);
    return 0;
}

static inline int tucube_epoll_http_readRequest(struct tucube_Module* module, struct tucube_ClData* clData) {
    ssize_t readSize;

    while((readSize = read(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket,
         tucube_epoll_http_Parser_getBufferPosition(GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->parser),
         tucube_epoll_http_Parser_getAvailableBufferSize(GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->parser))) > 0) {
        int result;
        if((result = tucube_epoll_http_Parser_parse(module, clData, GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->parser, readSize)) <= 0) {
            if(GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->parser->state == TUCUBE_EPOLL_HTTP_PARSER_ERROR) {
                warnx("%s: %u: Parser error", __FILE__, __LINE__);
                return -1;
            }
            break;
        }
    }
    if(readSize == -1) {
        if(errno == EAGAIN) {
//            warnx("%s: %u: Client socket EAGAIN", __FILE__, __LINE__);
            return 1;
        }
        else if (errno == EWOULDBLOCK) {
            warnx("%s: %u: Client socket EWOULDBLOCK", __FILE__, __LINE__);
            return 1;
        }
        else
            warn("%s: %u", __FILE__, __LINE__);
        return -1;
    }
    else if(readSize == 0) {
        warnx("%s: %u: Client socket has been closed", __FILE__, __LINE__);
        return -1;
    }
    
    if(GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->isKeepAlive)
        return 2;
    char* connectionHeaderValue;
    GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onGetRequestStringHeader(module, clData, "Connection", &connectionHeaderValue);
    if(strncasecmp(connectionHeaderValue, "Keep-Alive", sizeof("Keep-Alive")) == 0) {
        GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->isKeepAlive = true;
        free(connectionHeaderValue);
        return 2;
    }
    free(connectionHeaderValue);
    return 0; // request finsihed
}

static inline int tucube_epoll_http_writeCrlf(int clientSocket) {
    return write(clientSocket, "\r\n", sizeof("\r\n") - 1);
}

static inline int tucube_epoll_http_writeStatusCode(int clientSocket, int statusCode) {
    write(clientSocket, "HTTP/1.1", sizeof("HTTP/1.1") - 1);
    write(clientSocket, " ", sizeof(" ") - 1);

    char* statusCode_string;
    size_t statusCode_string_length = gonc_ltostr(statusCode, 10, &statusCode_string);
    write(clientSocket, statusCode_string, statusCode_string_length);
    free(statusCode_string);

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
    int result;
    if((result = GONC_CAST(module->pointer,
             struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseHeaderStart(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData)) <= 0)) {
        return result;
    }
    do {
        const char* headerField;
        size_t headerFieldSize;
        const char* headerValue;
        size_t headerValueSize;
        
        if((result = GONC_CAST(module->pointer,
             struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseHeader(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData), &headerField, &headerFieldSize, &headerValue, &headerValueSize)) == -1) {
            return -1;
        }
        tucube_epoll_http_writeHeader(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, headerField, headerFieldSize, headerValue, headerValueSize);
    }
    while(result == 1);
    return 0;
}

static inline int tucube_epoll_http_writeContentLength(int clientSocket, size_t bodySize) {
    char* bodySizeString;
    size_t bodySizeStringSize;
    bodySizeStringSize = gonc_ltostr(bodySize, 10, &bodySizeString);
    tucube_epoll_http_writeHeader(clientSocket, "Content-Length", sizeof("Content-Length") - 1, bodySizeString, bodySizeStringSize);
    free(bodySizeString);
    tucube_epoll_http_writeCrlf(clientSocket);
    return 0;
}

static inline int tucube_epoll_http_writeBody(struct tucube_Module* module, struct tucube_ClData* clData) {
    switch(GONC_CAST(module->pointer,
        struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseBodyStart(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData))) {
        case -1:
            return -1;
        case 0:
            tucube_epoll_http_writeCrlf(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket);
            return 0;
    }

    int result;
    struct tucube_epoll_http_ResponseBody body;
    switch(GONC_CAST(module->pointer,
        struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseBody(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData), &body)) {
        case -1:
            return -1;
        case 0:
            tucube_epoll_http_writeContentLength(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, body.size);
            switch(body.type) {
                case TUCUBE_EPOLL_HTTP_RESPONSE_BODY_STRING:
                    write(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, body.chars, body.size);
                    break;
                case TUCUBE_EPOLL_HTTP_RESPONSE_BODY_FILE:
                    sendfile(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, body.fd, NULL, body.size);
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
            tucube_epoll_http_writeHeader(*GONC_CAST(clData->pointer,
                 struct tucube_epoll_http_ClData*)->clientSocket, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1, "chunked", sizeof("chunked"));
            tucube_epoll_http_writeCrlf(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket);

            tucube_epoll_http_writeContentLength(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, body.size);

            write(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, body.chars, body.size);
            tucube_epoll_http_writeCrlf(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket);
            do {
                if((result = GONC_CAST(module->pointer,
                    struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseBody(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData), &body)) == -1) {
                    return -1;
                }
                tucube_epoll_http_writeContentLength(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, body.size);
                write(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, body.chars, body.size);
                tucube_epoll_http_writeCrlf(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket);
            }
            while(result == 1);
            tucube_epoll_http_writeCrlf(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket);
            return 0;
        default:
            return -1;
    }
}

static inline int tucube_epoll_http_writeResponse(struct tucube_Module* module, struct tucube_ClData* clData) {
    int statusCode;
    if(GONC_CAST(module->pointer,
        struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseStatusCode(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData), &statusCode) == -1) {
        return -1;
    }

    tucube_epoll_http_writeStatusCode(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, statusCode);

    if(tucube_epoll_http_writeHeaders(module, clData) == -1)
        return -1;

    if(tucube_epoll_http_writeBody(module, clData) == -1)
        return -1;

    return 0; 
}

int tucube_tcp_epoll_Module_service(struct tucube_Module* module, struct tucube_ClData* clData) {
    register int result = tucube_epoll_http_readRequest(module, clData);

    if(result != 0)
        return result;

    if(fcntl(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, F_SETFL, fcntl(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, F_GETFL, 0) & ~O_NONBLOCK) == -1)
        return -1;

    if(tucube_epoll_http_writeResponse(module, clData) == -1)
        return -1;

    if(fcntl(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, F_SETFL, fcntl(*GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket, F_GETFL, 0) | O_NONBLOCK) == -1)
        return -1;

    return 0;
}

int tucube_tcp_epoll_Module_clDestroy(struct tucube_Module* module, struct tucube_ClData* clData) {
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_clDestroy(GONC_LIST_ELEMENT_NEXT(module),
              GONC_LIST_ELEMENT_NEXT(clData));

    free(GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->parser->buffer);
    free(GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->parser);
    close(*(int*)GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket);
    *(int*)GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket = -1;
    free(clData->pointer);
    free(clData);

    return 0;
}

int tucube_tcp_epoll_Module_tlDestroy(struct tucube_Module* module) {
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_tlDestroy(GONC_LIST_ELEMENT_NEXT(module));

    return 0;
}

int tucube_tcp_epoll_Module_destroy(struct tucube_Module* module) {
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_destroy(GONC_LIST_ELEMENT_NEXT(module));

//    dlclose(module->dl_handle);
    free(module->pointer);
    free(module);
    return 0;
}
