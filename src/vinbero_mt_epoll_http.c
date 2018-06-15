#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <gaio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gon_http_parser.h>
#include <vinbero_common/vinbero_common_Error.h>
#include <vinbero_common/vinbero_common_Status.h>
#include <vinbero_common/vinbero_common_Call.h>
#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>
#include <vinbero_common/vinbero_common_ClData.h>
#include <vinbero_common/vinbero_common_Log.h>
#include <vinbero/vinbero_interface_MODULE.h>
#include <vinbero/vinbero_interface_TLOCAL.h>
#include <vinbero/vinbero_interface_CLOCAL.h>
#include <vinbero/vinbero_interface_CLSERVICE.h>
#include <libgenc/genc_args.h>
#include <libgenc/genc_Tree.h>
#include <libgenc/genc_uIntToNStr.h>
#include <gaio.h>
#include "vinbero_interface_HTTP.h"

struct vinbero_mt_epoll_http_Module {
    size_t parserHeaderBufferCapacity;
    size_t parserBodyBufferCapacity;
    struct gon_http_parser_callbacks parserCallbacks;
};

struct vinbero_mt_epoll_http_ClData {
    struct gaio_Io* clientIo;
    struct vinbero_interface_HTTP_Response* clientResponse;
    struct gon_http_parser* parser;
    bool isKeepAlive;
};

VINBERO_INTERFACE_MODULE_FUNCTIONS;
VINBERO_INTERFACE_TLOCAL_FUNCTIONS;
VINBERO_INTERFACE_CLOCAL_FUNCTIONS;
VINBERO_INTERFACE_CLSERVICE_FUNCTIONS;

int vinbero_interface_MODULE_init(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    vinbero_common_Module_init(module, "vinbero_mt_epoll_http", "0.0.1", true);
    module->localModule.pointer = calloc(1, sizeof(struct vinbero_mt_epoll_http_Module));
    struct vinbero_mt_epoll_http_Module* localModule = module->localModule.pointer;
/*
    if(GENC_TREE_NODE_CHILD_COUNT(module) != 1) {
        warnx("%s: %u: %s: this module requires only one module", __FILE__, __LINE__, __FUNCTION__);
        return -1;
    }
*/
    struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, 0);

/*
    struct vinbero_mt_epoll_http_interface;
    int ret;
    VINBERO_ITLOCAL_DLSYM(childinterface, childModule->dlHandle, &ret);
    if(ret == 1) {
        warnx("module %s doesn't satisfy ITLOCAL interface", childModule->id);
        return -1;
    }
    VINBERO_ICLOCAL_DLSYM(childinterface, childModule->dlHandle, &ret);
    if(ret == 1) {
        warnx("module %s doesn't satisfy ICLOCAL interface", childModule->id);
        return -1;
    }
    VINBERO_IHTTP_DLSYM(childinterface, childModule->dlHandle, &ret);
    if(ret == 1) {
        warnx("module %s doesn't satisfy IHTTP interface", childModule->id);
        return -1;
    }
*/
    int parserHeaderBufferCapacity = 0;
    int parserBodyBufferCapacity = 0;

    vinbero_common_Config_getInt(module->config, module, "vinbero_mt_epoll_http.parserHeaderBufferCapacity", &parserHeaderBufferCapacity, 1024 * 1024);
    localModule->parserHeaderBufferCapacity = parserHeaderBufferCapacity;
    vinbero_common_Config_getInt(module->config, module, "vinbero_mt_epoll_http.parserBodyBufferCapacity", &parserBodyBufferCapacity, 10 * 1024 * 1024);
    localModule->parserHeaderBufferCapacity = parserBodyBufferCapacity;


    #define LOAD_PARSER_CALLBACK(fName) do { \
        (localModule)->parserCallbacks.fName = NULL; \
        VINBERO_COMMON_DLSYM(&childModule->dlHandle, "vinbero_interface_HTTP_"#fName, &localModule->parserCallbacks.fName, &ret); \
        if(ret < 0) \
            VINBERO_COMMON_LOG_ERROR("dlsym(%s) failed", "vinbero_interface_HTTP_"#fName); \
    } while(0)
    LOAD_PARSER_CALLBACK(onRequestStart);
    LOAD_PARSER_CALLBACK(onRequestMethod);
    LOAD_PARSER_CALLBACK(onRequestUri);
    LOAD_PARSER_CALLBACK(onRequestVersionMajor);
    LOAD_PARSER_CALLBACK(onRequestVersionMinor);
    LOAD_PARSER_CALLBACK(onRequestScriptPath);
    LOAD_PARSER_CALLBACK(onRequestContentType);
    LOAD_PARSER_CALLBACK(onRequestContentLength);
    LOAD_PARSER_CALLBACK(onRequestHeaderField);
    LOAD_PARSER_CALLBACK(onRequestHeaderValue);
    LOAD_PARSER_CALLBACK(onRequestHeadersFinish);
    LOAD_PARSER_CALLBACK(onRequestBodyStart);
    LOAD_PARSER_CALLBACK(onRequestBody);
    LOAD_PARSER_CALLBACK(onRequestBodyFinish);
    #undef LOAD_PARSER_CALLBACK
    return 0;
}

int vinbero_interface_MODULE_rInit(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    return 0;
}

int vinbero_interface_TLOCAL_init(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        VINBERO_COMMON_CALL(TLOCAL, init, childModule, &ret, childModule);
        if(ret < 0)
            return ret;
    }
    return 0;
}

int vinbero_interface_TLOCAL_rInit(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    return 0;
}

static int vinbero_mt_epoll_http_writeBytes(struct vinbero_interface_HTTP_Response* response, char* bytes, size_t bytesSize) {
    response->io->methods->write(response->io, bytes, bytesSize);
    return 0;
}

static int vinbero_mt_epoll_http_writeIo(struct vinbero_interface_HTTP_Response* response, struct gaio_Io* io, size_t bytesSize) {
    response->io->methods->sendfile(response->io, io, NULL, bytesSize);
    return 0;
}

static int vinbero_mt_epoll_http_writeCrLf(struct vinbero_interface_HTTP_Response* response) {
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return 0;
}

static int vinbero_mt_epoll_http_writeVersion(struct vinbero_interface_HTTP_Response* response, int major, int minor) {
    response->io->methods->write(response->io, "HTTP/", sizeof("HTTP/") - 1);

    char* majorString;
    size_t majorStringSize;
    majorStringSize = genc_uIntToNStr(major, 10, &majorString);
    response->io->methods->write(response->io, majorString, majorStringSize);
    free(majorString);

    response->io->methods->write(response->io, ".", sizeof(".") - 1);

    char* minorString;
    size_t minorStringSize;
    minorStringSize = genc_uIntToNStr(minor, 10, &minorString);
    response->io->methods->write(response->io, minorString, minorStringSize);
    free(minorString);

    response->io->methods->write(response->io, " ", sizeof(" ") - 1);

    return 0;
}

static int vinbero_mt_epoll_http_writeStatusCode(struct vinbero_interface_HTTP_Response* response, int statusCode) {
    char* statusCodeString;
    size_t statusCodeStringSize;
    statusCodeStringSize = genc_uIntToNStr(statusCode, 10, &statusCodeString);
    response->io->methods->write(response->io, statusCodeString, statusCodeStringSize);
    free(statusCodeString);

    response->io->methods->write(response->io, " \r\n", sizeof(" \r\n") - 1);

    return 0;
}
static int vinbero_mt_epoll_http_writeIntHeader(struct vinbero_interface_HTTP_Response* response, char* headerField, size_t headerFieldSize, int headerValue) {
    response->io->methods->write(response->io, headerField, headerFieldSize);
    response->io->methods->write(response->io, ": ", sizeof(": ") - 1);
    char* headerValueString;
    size_t headerValueStringSize;
    headerValueStringSize = genc_uIntToNStr(headerValue, 10, &headerValueString);
    response->io->methods->write(response->io, headerValueString, headerValueStringSize);
    free(headerValueString);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);

    return 0;
}
static int vinbero_mt_epoll_http_writeDoubleHeader(struct vinbero_interface_HTTP_Response* response, char* headerField, size_t headerFieldSize, double headerValue) {
    // not implemented yet
    return 0;
}
static int vinbero_mt_epoll_http_writeStringHeader(struct vinbero_interface_HTTP_Response* response, char* headerField, size_t headerFieldSize, char* headerValue, size_t headerValueSize) {
    response->io->methods->write(response->io, headerField, headerFieldSize);
    response->io->methods->write(response->io, ": ", sizeof(": ") - 1);
    response->io->methods->write(response->io, headerValue, headerValueSize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return 0;
}

static int vinbero_mt_epoll_http_writeStringBody(struct vinbero_interface_HTTP_Response* response, char* stringBody, size_t stringBodySize) {
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    response->io->methods->write(response->io, stringBody, stringBodySize);
    return 0;
}

static int vinbero_mt_epoll_http_writeIoBody(struct vinbero_interface_HTTP_Response* response, struct gaio_Io* ioBody, size_t ioBodySize) {
    vinbero_mt_epoll_http_writeIntHeader(response, "Content-Length", sizeof("Content-Length") - 1, ioBodySize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    response->io->methods->sendfile(response->io, ioBody, NULL, ioBodySize);
    return 0;
}

static int vinbero_mt_epoll_http_writeChunkedBodyStart(struct vinbero_interface_HTTP_Response* response) {
    vinbero_mt_epoll_http_writeStringHeader(response, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1, "chunked", sizeof("chunked") - 1);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return 0;
}

static int vinbero_mt_epoll_http_writeChunkedBody(struct vinbero_interface_HTTP_Response* response, char* stringBody, size_t stringBodySize) {
    char* stringBodySizeString;
    size_t stringBodySizeStringSize;
    stringBodySizeStringSize = genc_uIntToNStr(stringBodySize, 16, &stringBodySizeString);
    response->io->methods->write(response->io, stringBodySizeString, stringBodySizeStringSize);
    free(stringBodySizeString);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    response->io->methods->write(response->io, stringBody, stringBodySize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return 0;
}

static int vinbero_mt_epoll_http_writeChunkedBodyEnd(struct vinbero_interface_HTTP_Response* response) {
    response->io->methods->write(response->io, "0\r\n", sizeof("0\r\n") - 1);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return 0;
}

int vinbero_interface_CLOCAL_init(struct vinbero_common_Module* module, struct vinbero_common_ClData* clData, void* args[]) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    struct vinbero_mt_epoll_http_Module* localModule = module->localModule.pointer;
    clData->generic.pointer = malloc(1 * sizeof(struct vinbero_mt_epoll_http_ClData));
    struct vinbero_mt_epoll_http_ClData* localClData = clData->generic.pointer;
    localClData->clientIo = args[0];

    localClData->isKeepAlive = false;
    localClData->parser = malloc(1 * sizeof(struct gon_http_parser));
    gon_http_parser_init(
        localClData->parser,
        localModule->parserHeaderBufferCapacity,
        localModule->parserBodyBufferCapacity
    );
    localClData->parser->callbacks = &(localModule->parserCallbacks);

    localClData->clientResponse = malloc(sizeof(struct vinbero_interface_HTTP_Response));
    localClData->clientResponse->methods = malloc(sizeof(struct vinbero_interface_HTTP_Response_Methods));
    localClData->clientResponse->io = localClData->clientIo;
    localClData->clientResponse->methods->writeBytes = vinbero_mt_epoll_http_writeBytes;
    localClData->clientResponse->methods->writeIo = vinbero_mt_epoll_http_writeIo;
    localClData->clientResponse->methods->writeCrLf = vinbero_mt_epoll_http_writeCrLf;
    localClData->clientResponse->methods->writeVersion = vinbero_mt_epoll_http_writeVersion;
    localClData->clientResponse->methods->writeStatusCode = vinbero_mt_epoll_http_writeStatusCode;
    localClData->clientResponse->methods->writeIntHeader = vinbero_mt_epoll_http_writeIntHeader;
    localClData->clientResponse->methods->writeDoubleHeader = vinbero_mt_epoll_http_writeDoubleHeader;
    localClData->clientResponse->methods->writeStringHeader = vinbero_mt_epoll_http_writeStringHeader;
    localClData->clientResponse->methods->writeStringBody = vinbero_mt_epoll_http_writeStringBody;
    localClData->clientResponse->methods->writeIoBody = vinbero_mt_epoll_http_writeIoBody;
    localClData->clientResponse->methods->writeChunkedBodyStart = vinbero_mt_epoll_http_writeChunkedBodyStart;
    localClData->clientResponse->methods->writeChunkedBody = vinbero_mt_epoll_http_writeChunkedBody;
    localClData->clientResponse->methods->writeChunkedBodyEnd = vinbero_mt_epoll_http_writeChunkedBodyEnd;

    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct vinbero_common_ClData* childClData = &GENC_TREE_NODE_GET_CHILD(clData, index);
        VINBERO_COMMON_CALL(CLOCAL, init, childModule, &ret, childModule, childClData, GENC_ARGS(localClData->clientResponse, NULL));
        if(ret < 0)
            return ret;
    }

    return 0;
}

static inline int vinbero_mt_epoll_http_readRequest(struct vinbero_common_Module* module, struct vinbero_common_ClData* clData) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    struct vinbero_mt_epoll_http_Module* localModule = module->localModule.pointer;
    struct vinbero_mt_epoll_http_ClData* localClData = clData->generic.pointer;
    struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, 0);
    struct vinbero_common_ClData* childClData = &GENC_TREE_NODE_GET_CHILD(clData, 0);
    ssize_t readSize;
    while((readSize = localClData->clientIo->methods->read(
              localClData->clientIo,
              gon_http_parser_getBufferPosition(localClData->parser),
              gon_http_parser_getAvailableBufferSize(localClData->parser)
           )) > 0) {
        int ret;
        if((ret = gon_http_parser_parse(localClData->parser, readSize, GENC_ARGS(childModule, childClData))) == -1) {
            VINBERO_COMMON_LOG_ERROR("Parser error");
            return VINBERO_COMMON_ERROR_UNKNOWN;
        } else if(ret == 0)
            break;
    }
    if(readSize == -1) {
        if(errno == EAGAIN) {
            VINBERO_COMMON_LOG_DEBUG("Client socket EAGAIN");
            return VINBERO_COMMON_ERROR_AGAIN;
        } else if(errno == EWOULDBLOCK) {
            VINBERO_COMMON_LOG_DEBUG("Client socket EWOULDBLOCK");
            return VINBERO_COMMON_ERROR_AGAIN;
        } else
            VINBERO_COMMON_LOG_ERROR("Client socket read() error");
        return VINBERO_COMMON_ERROR_IO;
    }
    else if(readSize == 0) {
        VINBERO_COMMON_LOG_ERROR("Client socket has been closed");
        return VINBERO_COMMON_ERROR_IO;
    }
    
    const char* connectionHeaderValue;
    VINBERO_COMMON_CALL(HTTP, onGetRequestStringHeader, childModule, &ret, childModule, childClData, "Connection", &connectionHeaderValue);
    if(ret != -1) {
        if(strncasecmp(connectionHeaderValue, "Keep-Alive", sizeof("Keep-Alive")) == 0) {
            VINBERO_COMMON_LOG_DEBUG("Keep-Alive Connection");
            localClData->isKeepAlive = false;
            gon_http_parser_reset(localClData->parser);
            return VINBERO_COMMON_STATUS_CONTINUE; // request finished but this is keep-alive
        }
        localClData->isKeepAlive = false;
    }
    return 0; // request finsihed
}

int vinbero_interface_CLSERVICE_call(struct vinbero_common_Module* module, struct vinbero_common_ClData* clData, void* args[]) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    struct vinbero_mt_epoll_http_Module* localModule = module->localModule.pointer;
    struct vinbero_mt_epoll_http_ClData* localClData = clData->generic.pointer;
    struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, 0);
    struct vinbero_common_ClData* childClData = &GENC_TREE_NODE_GET_CHILD(clData, 0);

    ret = vinbero_mt_epoll_http_readRequest(module, clData);
    if(ret != 0 && ret != VINBERO_COMMON_STATUS_CONTINUE)
        return ret;

    if(localClData->clientIo->methods->fcntl(localClData->clientIo, F_SETFL, localClData->clientIo->methods->fcntl(localClData->clientIo, F_GETFL, 0) & ~O_NONBLOCK) == -1)
        return VINBERO_COMMON_ERROR_UNKNOWN;

    VINBERO_COMMON_CALL(HTTP, onRequestFinish, childModule, &ret, childModule, childClData, args);
    if(ret < 0)
        return ret;

    if(localClData->clientIo->methods->fcntl(localClData->clientIo, F_SETFL, localClData->clientIo->methods->fcntl(localClData->clientIo, F_GETFL, 0) | O_NONBLOCK) == -1)
        return VINBERO_COMMON_ERROR_UNKNOWN;
    if(ret == VINBERO_COMMON_STATUS_CONTINUE)
        return VINBERO_COMMON_ERROR_AGAIN;

    return 0;
}

int vinbero_interface_CLOCAL_destroy(struct vinbero_common_Module* module, struct vinbero_common_ClData* clData) {
    VINBERO_COMMON_LOG_TRACE2();
    struct vinbero_mt_epoll_http_Module* localModule = module->localModule.pointer;
    struct vinbero_mt_epoll_http_ClData* localClData = clData->generic.pointer;
    free(localClData->clientResponse->methods);
    free(localClData->clientResponse);
    free(localClData->parser->buffer);
    free(localClData->parser);
    free(clData->generic.pointer);
    return 0;
}

int vinbero_interface_TLOCAL_destroy(struct vinbero_common_Module* module) {
    int ret;
    VINBERO_COMMON_LOG_TRACE2();
    struct vinbero_mt_epoll_http_Module* localModule = module->localModule.pointer;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        VINBERO_COMMON_CALL(TLOCAL, destroy, childModule, &ret, childModule);
        if(ret < 0)
            return ret;
    }
    return 0;
}

int vinbero_interface_TLOCAL_rDestroy(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    return 0;
}

int vinbero_interface_MODULE_destroy(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    return 0;
}

int vinbero_interface_MODULE_rDestroy(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    free(module->localModule.pointer);
    return 0;
}
