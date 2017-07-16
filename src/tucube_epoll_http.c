#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <gaio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gon_http_parser.h>
#include <tucube/tucube_Module.h>
#include <tucube/tucube_ClData.h>
#include <libgenc/genc_cast.h>
#include <libgenc/genc_list.h>
#include <libgenc/genc_ltostr.h>
#include <tucube/tucube_IBase.h>
#include <tucube/tucube_ICLocal.h>
#include <gaio.h>
#include "tucube_IHttp.h"
#include "tucube_epoll_http_ResponseBody.h"

struct tucube_epoll_http_Module {
    TUCUBE_IBASE_FUNCTION_POINTERS;
    TUCUBE_ICLOCAL_FUNCTION_POINTERS;
    TUCUBE_IHTTP_FUNCTION_POINTERS;

    size_t parserHeaderBufferCapacity;
    size_t parserBodyBufferCapacity;
};

struct tucube_epoll_http_ClData {
    struct gaio_Io* clientIo;
    struct gon_http_parser* parser;
    bool isKeepAlive;
};

TUCUBE_IBASE_FUNCTIONS;
TUCUBE_ICLOCAL_FUNCTIONS;

int tucube_IBase_init(struct tucube_Module_Config* moduleConfig, struct tucube_Module_List* moduleList, void* args[]) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    if(GENC_LIST_ELEMENT_NEXT(moduleConfig) == NULL)
        errx(EXIT_FAILURE, "tucube_epoll_http requires another module");

    struct tucube_Module* module = malloc(1 * sizeof(struct tucube_Module));
    GENC_LIST_ELEMENT_INIT(module);
    module->generic.pointer = malloc(1 * sizeof(struct tucube_epoll_http_Module));

    TUCUBE_MODULE_DLOPEN(module, moduleConfig);
    TUCUBE_IBASE_DLSYM(module, struct tucube_epoll_http_Module);
    TUCUBE_ICLOCAL_DLSYM(module, struct tucube_epoll_http_Module);
    TUCUBE_IHTTP_DLSYM(module, struct tucube_epoll_http_Module);

    TUCUBE_LOCAL_MODULE->parserHeaderBufferCapacity = 1024;

    if(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserHeaderBufferCapacity") != NULL)
        TUCUBE_LOCAL_MODULE->parserHeaderBufferCapacity = json_integer_value(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserHeaderBufferCapacity"));

    TUCUBE_LOCAL_MODULE->parserBodyBufferCapacity = 1048576;

    if(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserBodyBufferCapacity") != NULL)
        TUCUBE_LOCAL_MODULE->parserBodyBufferCapacity = json_integer_value(json_object_get(json_array_get(moduleConfig->json, 1), "tucube_epoll_http.parserBodyBufferCapacity"));

    GENC_LIST_APPEND(moduleList, module);

    if(TUCUBE_LOCAL_MODULE->tucube_IBase_init(GENC_LIST_ELEMENT_NEXT(moduleConfig), moduleList, (void*[]){NULL}) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_IBase_init() failed", __FILE__, __LINE__);

    return 0;
}

int tucube_IBase_tlInit(struct tucube_Module* module, struct tucube_Module_Config* moduleConfig, void* args[]) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    TUCUBE_LOCAL_MODULE->tucube_IBase_tlInit(GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(moduleConfig), (void*[]){NULL});
#undef TUCUBE_LOCAL_MODULE
    return 0;
}

int tucube_ICLocal_init(struct tucube_Module* module, struct tucube_ClData_List* clDataList, void* args[]) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLDATA GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)
#define TUCUBE_LOCAL_PARSER GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)->parser
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_ClData* clData = malloc(1 * sizeof(struct tucube_ClData));
    GENC_LIST_ELEMENT_INIT(clData);
    clData->generic.pointer = malloc(1 * sizeof(struct tucube_epoll_http_ClData));

    TUCUBE_LOCAL_CLDATA->clientIo = ((struct gaio_Io*)args[0]);

    TUCUBE_LOCAL_CLDATA->isKeepAlive = false;

    TUCUBE_LOCAL_PARSER = malloc(1 * sizeof(struct gon_http_parser));
    gon_http_parser_init(
        TUCUBE_LOCAL_PARSER,
        TUCUBE_LOCAL_MODULE->parserHeaderBufferCapacity,
        TUCUBE_LOCAL_MODULE->parserBodyBufferCapacity
    );
    TUCUBE_LOCAL_PARSER->onRequestStart = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestStart;
    TUCUBE_LOCAL_PARSER->onRequestMethod = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestMethod;
    TUCUBE_LOCAL_PARSER->onRequestUri = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestUri;
    TUCUBE_LOCAL_PARSER->onRequestProtocol = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestProtocol;
    TUCUBE_LOCAL_PARSER->onRequestScriptPath = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestScriptPath;
    TUCUBE_LOCAL_PARSER->onRequestContentType = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestContentType;
    TUCUBE_LOCAL_PARSER->onRequestContentLength = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestContentLength;

    TUCUBE_LOCAL_PARSER->onRequestHeaderField = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestHeaderField;
    TUCUBE_LOCAL_PARSER->onRequestHeaderValue = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestHeaderValue;
    TUCUBE_LOCAL_PARSER->onRequestHeadersFinish = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestHeadersFinish;
    TUCUBE_LOCAL_PARSER->onRequestBodyStart = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestBodyStart;
    TUCUBE_LOCAL_PARSER->onRequestBody = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestBody;
    TUCUBE_LOCAL_PARSER->onRequestBodyFinish = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestBodyFinish;
    TUCUBE_LOCAL_PARSER->onRequestFinish = TUCUBE_LOCAL_MODULE->tucube_IHttp_onRequestFinish;

    GENC_LIST_APPEND(clDataList, clData);
    TUCUBE_LOCAL_MODULE->tucube_ICLocal_init(GENC_LIST_ELEMENT_NEXT(module), clDataList, args);
    return 0;
#undef TUCUBE_LOCAL_MODULE
#undef TUCUBE_LOCAL_CLDATA
#undef TUCUBE_LOCAL_PARSER
}
static inline int tucube_epoll_http_readRequest(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLDATA GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)
#define TUCUBE_LOCAL_PARSER GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)->parser
#define TUCUBE_LOCAL_CLIENT_IO GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)->clientIo
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    ssize_t readSize;
    while((readSize = TUCUBE_LOCAL_CLIENT_IO->read(
              TUCUBE_LOCAL_CLIENT_IO,
              gon_http_parser_getBufferPosition(TUCUBE_LOCAL_PARSER),
              gon_http_parser_getAvailableBufferSize(TUCUBE_LOCAL_PARSER)
           )) > 0) {
        int result;
        if((result = gon_http_parser_parse(TUCUBE_LOCAL_PARSER, readSize, (void*[]){GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(clData)})) == -1) {
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
    if(TUCUBE_LOCAL_MODULE->tucube_IHttp_onGetRequestStringHeader(
              GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(clData), "Connection", &connectionHeaderValue
      ) != -1) {
        if(strncasecmp(connectionHeaderValue, "Keep-Alive", sizeof("Keep-Alive")) == 0) {
            warnx("%s: %u: Keep-Alive Connection", __FILE__, __LINE__);
            TUCUBE_LOCAL_CLDATA->isKeepAlive = false;
            gon_http_parser_reset(TUCUBE_LOCAL_PARSER);
            return 2;
        }
        TUCUBE_LOCAL_CLDATA->isKeepAlive = false;
    }
    return 0; // request finsihed
#undef TUCUBE_LOCAL_MODULE
#undef TUCUBE_LOCAL_CLDATA
#undef TUCUBE_LOCAL_PARSER
}

static inline int tucube_epoll_http_writeCrlf(struct gaio_Io* clientIo) {
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    return clientIo->write(clientIo, "\r\n", sizeof("\r\n") - 1);
}

static inline int tucube_epoll_http_writeStatusCode(struct gaio_Io* clientIo, int statusCode) {
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    clientIo->write(clientIo, "HTTP/1.1", sizeof("HTTP/1.1") - 1);
    clientIo->write(clientIo, " ", sizeof(" ") - 1);

    char* statusCodeString;
    size_t statusCodeStringLength = genc_ltostr(statusCode, 10, &statusCodeString);
    clientIo->write(clientIo, statusCodeString, statusCodeStringLength);
    free(statusCodeString);

    clientIo->write(clientIo, " ", sizeof(" ") - 1); // reason phrase is optional but blank space after status code is necessary

    tucube_epoll_http_writeCrlf(clientIo);
    return 0;
}

static inline int tucube_epoll_http_writeHeader(struct gaio_Io* clientIo, const char* headerField, size_t headerFieldSize, const char* headerValue, size_t headerValueSize) {
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
    clientIo->write(clientIo, headerField, headerFieldSize);
#pragma GCC diagnostic pop
    clientIo->write(clientIo, ": ", sizeof(": ") - 1);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
    clientIo->write(clientIo, headerValue, headerValueSize);
#pragma GCC diagnostic pop
    tucube_epoll_http_writeCrlf(clientIo);
    return 0;
}

static inline int tucube_epoll_http_writeHeaders(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLIENT_IO GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)->clientIo
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    int result;
    if((result = TUCUBE_LOCAL_MODULE->tucube_IHttp_onResponseHeaderStart(GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(clData)) <= 0)) {
        return result;
    }
    if(GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)->isKeepAlive)
        tucube_epoll_http_writeHeader(TUCUBE_LOCAL_CLIENT_IO, "Connection", sizeof("Connection") - 1, "Keep-Alive", sizeof("Keep-Alive") - 1);
    do {
        const char* headerField;
        size_t headerFieldSize;
        const char* headerValue;
        size_t headerValueSize;
        
        if((result = TUCUBE_LOCAL_MODULE->tucube_IHttp_onResponseHeader(GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(clData), &headerField, &headerFieldSize, &headerValue, &headerValueSize)) == -1) {
            return -1;
        }
        tucube_epoll_http_writeHeader(TUCUBE_LOCAL_CLIENT_IO, headerField, headerFieldSize, headerValue, headerValueSize);
    }
    while(result == 1);
    return 0;
#undef TUCUBE_LOCAL_CLIENT_IO
#undef TUCUBE_LOCAL_MODULE
}

static inline int tucube_epoll_http_writeContentLength(struct gaio_Io* clientIo, size_t bodySize) {
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    char* bodySizeString;
    size_t bodySizeStringSize;
    bodySizeStringSize = genc_ltostr(bodySize, 10, &bodySizeString);
    tucube_epoll_http_writeHeader(clientIo, "Content-Length", sizeof("Content-Length") - 1, bodySizeString, bodySizeStringSize);
    free(bodySizeString);
    tucube_epoll_http_writeCrlf(clientIo);
    return 0;
}

static inline int tucube_epoll_http_writeBody(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLIENT_IO GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)->clientIo
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    switch(TUCUBE_LOCAL_MODULE->tucube_IHttp_onResponseBodyStart(GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(clData))) {
        case -1:
            return -1;
        case 0:
            tucube_epoll_http_writeCrlf(TUCUBE_LOCAL_CLIENT_IO);
            return 0;
    }

    int result;
    struct tucube_epoll_http_ResponseBody body;
    struct gaio_Io bodyIo;
    switch(TUCUBE_LOCAL_MODULE->tucube_IHttp_onResponseBody(GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(clData), &body)) {
        case -1:
            return -1;
        case 0:
            tucube_epoll_http_writeContentLength(TUCUBE_LOCAL_CLIENT_IO, body.size);
            switch(body.type) {
                case TUCUBE_EPOLL_HTTP_RESPONSE_BODY_STRING:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
                    TUCUBE_LOCAL_CLIENT_IO->write(TUCUBE_LOCAL_CLIENT_IO, body.chars, body.size);
#pragma GCC diagnostic pop
                    break;
                case TUCUBE_EPOLL_HTTP_RESPONSE_BODY_FILE:
		    GAIO_NOP_INIT(&bodyIo);
		    bodyIo.object.integer = body.fd;
		    bodyIo.fileno = gaio_Fd_fileno;
                    TUCUBE_LOCAL_CLIENT_IO->sendfile(TUCUBE_LOCAL_CLIENT_IO, &bodyIo, NULL, body.size);
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
            tucube_epoll_http_writeHeader(TUCUBE_LOCAL_CLIENT_IO, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1, "chunked", sizeof("chunked"));
            tucube_epoll_http_writeCrlf(TUCUBE_LOCAL_CLIENT_IO);

            tucube_epoll_http_writeContentLength(TUCUBE_LOCAL_CLIENT_IO, body.size);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
            TUCUBE_LOCAL_CLIENT_IO->write(TUCUBE_LOCAL_CLIENT_IO, body.chars, body.size);
#pragma GCC diagnostic pop
            tucube_epoll_http_writeCrlf(TUCUBE_LOCAL_CLIENT_IO);
            do {
                if((result = TUCUBE_LOCAL_MODULE->tucube_IHttp_onResponseBody(GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(clData), &body)) == -1) {
                    return -1;
                }
                tucube_epoll_http_writeContentLength(TUCUBE_LOCAL_CLIENT_IO, body.size);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
                TUCUBE_LOCAL_CLIENT_IO->write(TUCUBE_LOCAL_CLIENT_IO, body.chars, body.size);
#pragma GCC diagnostic pop
                tucube_epoll_http_writeCrlf(TUCUBE_LOCAL_CLIENT_IO);
            }
            while(result == 1);
            tucube_epoll_http_writeCrlf(TUCUBE_LOCAL_CLIENT_IO);
            return 0;
        default:
            return -1;
    }
#undef TUCUBE_LOCAL_CLIENT_IO
#undef TUCUBE_LOCAL_MODULE
}

static inline int tucube_epoll_http_writeResponse(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_CLIENT_IO GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)->clientIo
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    int statusCode;
    if(TUCUBE_LOCAL_MODULE->tucube_IHttp_onResponseStatusCode(GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(clData), &statusCode) == -1) {
        return -1;
    }

    tucube_epoll_http_writeStatusCode(TUCUBE_LOCAL_CLIENT_IO, statusCode);

    if(tucube_epoll_http_writeHeaders(module, clData) == -1)
        return -1;

    if(tucube_epoll_http_writeBody(module, clData) == -1)
        return -1;

    return 0; 
#undef TUCUBE_LOCAL_CLIENT_IO
#undef TUCUBE_LOCAL_MODULE
}

int tucube_IClService_call(struct tucube_Module* module, struct tucube_ClData* clData, void* args[]) {
#define TUCUBE_LOCAL_CLIENT_IO GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)->clientIo
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    register int result = tucube_epoll_http_readRequest(module, clData);
    if(result != 0 && result != 2)
        return result;
    if(TUCUBE_LOCAL_CLIENT_IO->fcntl(TUCUBE_LOCAL_CLIENT_IO, F_SETFL, TUCUBE_LOCAL_CLIENT_IO->fcntl(TUCUBE_LOCAL_CLIENT_IO, F_GETFL, 0) & ~O_NONBLOCK) == -1)
        return -1;
    if(tucube_epoll_http_writeResponse(module, clData) == -1)
        return -1;

    if(TUCUBE_LOCAL_CLIENT_IO->fcntl(TUCUBE_LOCAL_CLIENT_IO, F_SETFL, TUCUBE_LOCAL_CLIENT_IO->fcntl(TUCUBE_LOCAL_CLIENT_IO, F_GETFL, 0) | O_NONBLOCK) == -1)
        return -1;
    if(result == 2)
        return 1;

    return 0;
#undef TUCUBE_LOCAL_CLIENT_IO
}

int tucube_ICLocal_destroy(struct tucube_Module* module, struct tucube_ClData* clData) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
#define TUCUBE_LOCAL_PARSER GENC_CAST(clData->generic.pointer, struct tucube_epoll_http_ClData*)->parser
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    TUCUBE_LOCAL_MODULE->tucube_ICLocal_destroy(GENC_LIST_ELEMENT_NEXT(module), GENC_LIST_ELEMENT_NEXT(clData));
    free(TUCUBE_LOCAL_PARSER->buffer);
    free(TUCUBE_LOCAL_PARSER);
    free(clData->generic.pointer);
    free(clData);
    return 0;
#undef TUCUBE_LOCAL_MODULE
#undef TUCUBE_LOCAL_PARSER
}

int tucube_IBase_tlDestroy(struct tucube_Module* module) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    TUCUBE_LOCAL_MODULE->tucube_IBase_tlDestroy(GENC_LIST_ELEMENT_NEXT(module));
    return 0;
#undef TUCUBE_LOCAL_MODULE
}

int tucube_IBase_destroy(struct tucube_Module* module) {
#define TUCUBE_LOCAL_MODULE GENC_CAST(module->generic.pointer, struct tucube_epoll_http_Module*)
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    TUCUBE_LOCAL_MODULE->tucube_IBase_destroy(GENC_LIST_ELEMENT_NEXT(module));
//    dlclose(module->dl_handle);
    free(module->generic.pointer);
    free(module);
    return 0;
#undef TUCUBE_LOCAL_MODULE
}
