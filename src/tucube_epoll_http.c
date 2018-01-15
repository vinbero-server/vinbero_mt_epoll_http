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
#include <libgenc/genc_args.h>
#include <libgenc/genc_Tree.h>
#include <libgenc/genc_uIntToNStr.h>
#include <tucube/tucube_ITLocal.h>
#include <tucube/tucube_IBasic.h>
#include <tucube/tucube_ICLocal.h>
#include <gaio.h>
#include "tucube_IHttp.h"

struct tucube_epoll_http_Module {
    size_t parserHeaderBufferCapacity;
    size_t parserBodyBufferCapacity;
    struct gon_http_parser_callbacks parserCallbacks;
};

struct tucube_epoll_http_Interface {
    TUCUBE_ITLOCAL_FUNCTION_POINTERS;
    TUCUBE_ICLOCAL_FUNCTION_POINTERS;
    TUCUBE_IHTTP_FUNCTION_POINTERS;
};

struct tucube_epoll_http_ClData {
    struct gaio_Io* clientIo;
    struct tucube_IHttp_Response* clientResponse;
    struct gon_http_parser* parser;
    bool isKeepAlive;
};

TUCUBE_IMODULE_FUNCTIONS;
TUCUBE_ICLOCAL_FUNCTIONS;

int tucube_IModule_init(struct tucube_Module* module, struct tucube_Config* config, void* args[]) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    module->name = "tucube_epoll_http";
    module->version = "0.0.1";
    module->localModule.pointer = malloc(1 * sizeof(struct tucube_epoll_http_Module));
    struct tucube_epoll_http_Module* localModule = module->localModule.pointer;
    if(GENC_TREE_NODE_CHILD_COUNT(module) != 1) {
        warnx("%s: %u: %s: this module requires only one module", __FILE__, __LINE__, __FUNCTION__);
        return -1;
    }
    struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, 0);
    childModule->interface = malloc(1 * sizeof(struct tucube_epoll_http_Interface));
    struct tucube_epoll_http_Interface* childInterface = childModule->interface;
    int errorVariable;
    TUCUBE_ITLOCAL_DLSYM(childInterface, childModule->dlHandle, &errorVariable);
    if(errorVariable == 1) {
        warnx("module %s doesn't satisfy ITLOCAL interface", childModule->id);
        return -1;
    }
    TUCUBE_ICLOCAL_DLSYM(childInterface, childModule->dlHandle, &errorVariable);
    if(errorVariable == 1) {
        warnx("module %s doesn't satisfy ICLOCAL interface", childModule->id);
        return -1;
    }
    TUCUBE_IHTTP_DLSYM(childInterface, childModule->dlHandle, &errorVariable);
    if(errorVariable == 1) {
        warnx("module %s doesn't satisfy IHTTP interface", childModule->id);
        return -1;
    }

    TUCUBE_CONFIG_GET(config, module, "tucube_epoll_http.parserHeaderBufferCapacity", integer, &localModule->parserHeaderBufferCapacity, 1024 * 1024);
    TUCUBE_CONFIG_GET(config, module, "tucube_epoll_http.parserBodyBufferCapacity", integer, &localModule->parserBodyBufferCapacity, 10 * 1024 * 1024);

    localModule->parserCallbacks.onRequestStart = childInterface->tucube_IHttp_onRequestStart;
    localModule->parserCallbacks.onRequestMethod = childInterface->tucube_IHttp_onRequestMethod;
    localModule->parserCallbacks.onRequestUri = childInterface->tucube_IHttp_onRequestUri;
    localModule->parserCallbacks.onRequestVersionMajor = childInterface->tucube_IHttp_onRequestVersionMajor;
    localModule->parserCallbacks.onRequestVersionMinor = childInterface->tucube_IHttp_onRequestVersionMinor;
    localModule->parserCallbacks.onRequestScriptPath = childInterface->tucube_IHttp_onRequestScriptPath;
    localModule->parserCallbacks.onRequestContentType = childInterface->tucube_IHttp_onRequestContentType;
    localModule->parserCallbacks.onRequestContentLength = childInterface->tucube_IHttp_onRequestContentLength;

    localModule->parserCallbacks.onRequestHeaderField = childInterface->tucube_IHttp_onRequestHeaderField;
    localModule->parserCallbacks.onRequestHeaderValue = childInterface->tucube_IHttp_onRequestHeaderValue;
    localModule->parserCallbacks.onRequestHeadersFinish = childInterface->tucube_IHttp_onRequestHeadersFinish;
    localModule->parserCallbacks.onRequestBodyStart = childInterface->tucube_IHttp_onRequestBodyStart;
    localModule->parserCallbacks.onRequestBody = childInterface->tucube_IHttp_onRequestBody;
    localModule->parserCallbacks.onRequestBodyFinish = childInterface->tucube_IHttp_onRequestBodyFinish;
    return 0;
}

int tucube_IModule_rInit(struct tucube_Module* module, struct tucube_Config* config, void* args[]) {
    return 0;
}

int tucube_ITLocal_init(struct tucube_Module* module, struct tucube_Config* config, void* args[]) {
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct tucube_epoll_http_Interface* childInterface = childModule->interface;
        if(childInterface->tucube_ITLocal_init(childModule, config, args) == -1) {
            warnx("tucube_ITLocal_init() failed at module %s", childModule->id);
            return -1;
        }
    }
    return 0;
}

int tucube_ITLocal_rInit(struct tucube_Module* module, struct tucube_Config* config, void* args[]) {
    warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    return 0;
}

static int tucube_epoll_http_writeBytes(struct tucube_IHttp_Response* response, char* bytes, size_t bytesSize) {
    response->io->methods->write(response->io, bytes, bytesSize);
    return 0;
}

static int tucube_epoll_http_writeIo(struct tucube_IHttp_Response* response, struct gaio_Io* io, size_t bytesSize) {
    response->io->methods->sendfile(response->io, io, NULL, bytesSize);
    return 0;
}

static int tucube_epoll_http_writeCrLf(struct tucube_IHttp_Response* response) {
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return 0;
}

static int tucube_epoll_http_writeVersion(struct tucube_IHttp_Response* response, int major, int minor) {
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

static int tucube_epoll_http_writeStatusCode(struct tucube_IHttp_Response* response, int statusCode) {
    char* statusCodeString;
    size_t statusCodeStringSize;
    statusCodeStringSize = genc_uIntToNStr(statusCode, 10, &statusCodeString);
    response->io->methods->write(response->io, statusCodeString, statusCodeStringSize);
    free(statusCodeString);

    response->io->methods->write(response->io, " \r\n", sizeof(" \r\n") - 1);

    return 0;
}
static int tucube_epoll_http_writeIntHeader(struct tucube_IHttp_Response* response, char* headerField, size_t headerFieldSize, int headerValue) {
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
static int tucube_epoll_http_writeDoubleHeader(struct tucube_IHttp_Response* response, char* headerField, size_t headerFieldSize, double headerValue) {
    // not implemented yet
    return 0;
}
static int tucube_epoll_http_writeStringHeader(struct tucube_IHttp_Response* response, char* headerField, size_t headerFieldSize, char* headerValue, size_t headerValueSize) {
    response->io->methods->write(response->io, headerField, headerFieldSize);
    response->io->methods->write(response->io, ": ", sizeof(": ") - 1);
    response->io->methods->write(response->io, headerValue, headerValueSize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return 0;
}

static int tucube_epoll_http_writeStringBody(struct tucube_IHttp_Response* response, char* stringBody, size_t stringBodySize) {
    tucube_epoll_http_writeIntHeader(response, "Content-Length", sizeof("Content-Length") - 1, stringBodySize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    response->io->methods->write(response->io, stringBody, stringBodySize);
    return 0;
}

static int tucube_epoll_http_writeIoBody(struct tucube_IHttp_Response* response, struct gaio_Io* ioBody, size_t ioBodySize) {
    tucube_epoll_http_writeIntHeader(response, "Content-Length", sizeof("Content-Length") - 1, ioBodySize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    response->io->methods->sendfile(response->io, ioBody, NULL, ioBodySize);
    return 0;
}

static int tucube_epoll_http_writeChunkedBodyStart(struct tucube_IHttp_Response* response) {
    tucube_epoll_http_writeStringHeader(response, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1, "chunked", sizeof("chunked") - 1);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return 0;
}

static int tucube_epoll_http_writeChunkedBody(struct tucube_IHttp_Response* response, char* stringBody, size_t stringBodySize) {
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

static int tucube_epoll_http_writeChunkedBodyEnd(struct tucube_IHttp_Response* response) {
    response->io->methods->write(response->io, "0\r\n", sizeof("0\r\n") - 1);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return 0;
}

int tucube_ICLocal_init(struct tucube_Module* module, struct tucube_ClData* clData, void* args[]) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_epoll_http_Module* localModule = module->localModule.pointer;
    clData->generic.pointer = malloc(1 * sizeof(struct tucube_epoll_http_ClData));
    struct tucube_epoll_http_ClData* localClData = clData->generic.pointer;
    localClData->clientIo = args[0];

    localClData->isKeepAlive = false;
    localClData->parser = malloc(1 * sizeof(struct gon_http_parser));
    gon_http_parser_init(
        localClData->parser,
        localModule->parserHeaderBufferCapacity,
        localModule->parserBodyBufferCapacity
    );
    localClData->parser->callbacks = &(localModule->parserCallbacks);

    localClData->clientResponse = malloc(sizeof(struct tucube_IHttp_Response));
    localClData->clientResponse->methods = malloc(sizeof(struct tucube_IHttp_Response_Methods));
    localClData->clientResponse->io = localClData->clientIo;
    localClData->clientResponse->methods->writeBytes = tucube_epoll_http_writeBytes;
    localClData->clientResponse->methods->writeIo = tucube_epoll_http_writeIo;
    localClData->clientResponse->methods->writeCrLf = tucube_epoll_http_writeCrLf;
    localClData->clientResponse->methods->writeVersion = tucube_epoll_http_writeVersion;
    localClData->clientResponse->methods->writeStatusCode = tucube_epoll_http_writeStatusCode;
    localClData->clientResponse->methods->writeIntHeader = tucube_epoll_http_writeIntHeader;
    localClData->clientResponse->methods->writeDoubleHeader = tucube_epoll_http_writeDoubleHeader;
    localClData->clientResponse->methods->writeStringHeader = tucube_epoll_http_writeStringHeader;
    localClData->clientResponse->methods->writeStringBody = tucube_epoll_http_writeStringBody;
    localClData->clientResponse->methods->writeIoBody = tucube_epoll_http_writeIoBody;
    localClData->clientResponse->methods->writeChunkedBodyStart = tucube_epoll_http_writeChunkedBodyStart;
    localClData->clientResponse->methods->writeChunkedBody = tucube_epoll_http_writeChunkedBody;
    localClData->clientResponse->methods->writeChunkedBodyEnd = tucube_epoll_http_writeChunkedBodyEnd;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct tucube_ClData* childClData = &GENC_TREE_NODE_GET_CHILD(clData, index);
        struct tucube_epoll_http_Interface* childInterface = childModule->interface;
        childInterface->tucube_ICLocal_init(childModule, childClData, GENC_ARGS(localClData->clientResponse, NULL));
    }

    return 0;
}

static inline int tucube_epoll_http_readRequest(struct tucube_Module* module, struct tucube_ClData* clData) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_epoll_http_Module* localModule = module->localModule.pointer;
    struct tucube_epoll_http_ClData* localClData = clData->generic.pointer;
    struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, 0);
    struct tucube_ClData* childClData = &GENC_TREE_NODE_GET_CHILD(clData, 0);
    struct tucube_epoll_http_Interface* childInterface = childModule->interface;

    ssize_t readSize;
    while((readSize = localClData->clientIo->methods->read(
              localClData->clientIo,
              gon_http_parser_getBufferPosition(localClData->parser),
              gon_http_parser_getAvailableBufferSize(localClData->parser)
           )) > 0) {
        int result;
        if((result = gon_http_parser_parse(localClData->parser, readSize, GENC_ARGS(childModule, childClData))) == -1) {
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
    if(childInterface->tucube_IHttp_onGetRequestStringHeader(childModule, childClData, "Connection", &connectionHeaderValue) != -1) {
        if(strncasecmp(connectionHeaderValue, "Keep-Alive", sizeof("Keep-Alive")) == 0) {
            warnx("%s: %u: Keep-Alive Connection", __FILE__, __LINE__);
            localClData->isKeepAlive = false;
            gon_http_parser_reset(localClData->parser);
            return 2; // request finished but this is keep-alive
        }
        localClData->isKeepAlive = false;
    }
    return 0; // request finsihed
}

int tucube_IClService_call(struct tucube_Module* module, struct tucube_ClData* clData, void* args[]) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_epoll_http_Module* localModule = module->localModule.pointer;
    struct tucube_epoll_http_ClData* localClData = clData->generic.pointer;
    struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, 0);
    struct tucube_ClData* childClData = &GENC_TREE_NODE_GET_CHILD(clData, 0);
    struct tucube_epoll_http_Interface* childInterface = childModule->interface;

    register int result = tucube_epoll_http_readRequest(module, clData);
    if(result != 0 && result != 2)
        return result;
    if(localClData->clientIo->methods->fcntl(localClData->clientIo, F_SETFL, localClData->clientIo->methods->fcntl(localClData->clientIo, F_GETFL, 0) & ~O_NONBLOCK) == -1)
        return -1;
    if(childInterface->tucube_IHttp_onRequestFinish(childModule, childClData, args) == -1)
        return -1;
    if(localClData->clientIo->methods->fcntl(localClData->clientIo, F_SETFL, localClData->clientIo->methods->fcntl(localClData->clientIo, F_GETFL, 0) | O_NONBLOCK) == -1)
        return -1;
    if(result == 2)
        return 1;

    return 0;
}

int tucube_ICLocal_destroy(struct tucube_Module* module, struct tucube_ClData* clData) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_epoll_http_Module* localModule = module->localModule.pointer;
    struct tucube_epoll_http_ClData* localClData = clData->generic.pointer;
    free(localClData->clientResponse->methods);
    free(localClData->clientResponse);
    free(localClData->parser->buffer);
    free(localClData->parser);
    free(clData->generic.pointer);
    return 0;
}

int tucube_ITLocal_destroy(struct tucube_Module* module) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    struct tucube_epoll_http_Module* localModule = module->localModule.pointer;
    GENC_TREE_NODE_FOR_EACH_CHILD(module, index) {
        struct tucube_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, index);
        struct tucube_epoll_http_Interface* childInterface = childModule->interface;
        if(childInterface->tucube_ITLocal_destroy(childModule) == -1) {
            warnx("tucube_ITLocal_init() failed at module %s", childModule->id);
            return -1;
        }
    }
    return 0;
}

int tucube_ITLocal_rDestroy(struct tucube_Module* module) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    return 0;
}

int tucube_IModule_destroy(struct tucube_Module* module) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    return 0;
}

int tucube_IModule_rDestroy(struct tucube_Module* module) {
warnx("%s: %u: %s", __FILE__, __LINE__, __FUNCTION__);
    free(module->localModule.pointer);
//    dlclose(module->dl_handle);
    return 0;
}
