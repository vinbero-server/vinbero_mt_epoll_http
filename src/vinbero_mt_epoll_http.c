#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <gaio.h>
#include <http_parser.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vinbero_common/vinbero_common_Error.h>
#include <vinbero_common/vinbero_common_Status.h>
#include <vinbero_common/vinbero_common_Call.h>
#include <vinbero_common/vinbero_common_Config.h>
#include <vinbero_common/vinbero_common_Module.h>
#include <vinbero_common/vinbero_common_ClModule.h>
#include <vinbero_common/vinbero_common_Log.h>
#include <vinbero/vinbero_interface_MODULE.h>
#include <vinbero/vinbero_interface_TLOCAL.h>
#include <vinbero/vinbero_interface_CLOCAL.h>
#include <vinbero/vinbero_interface_CLSERVICE.h>
#include <libgenc/genc_Nstr_cat.h>
#include <libgenc/genc_Tree.h>
#include <libgenc/genc_Uint_toNstr.h>
#include <gaio.h>
#include "vinbero_mt_epoll_http_Version.h"
#include "vinbero_interface_HTTP.h"

struct vinbero_mt_epoll_http_Module {
    struct vinbero_interface_HTTP childInterface;
    struct http_parser_settings parserCallbacks;
    struct vinbero_interface_HTTP_Response_Methods responseMethods;
    size_t parserBufferCapacity;
};

struct vinbero_mt_epoll_http_ClModule {
    struct gaio_Io* clientIo;
    struct vinbero_interface_HTTP_Response clientResponse;
    http_parser parser;
    bool isKeepAlive;
};

struct vinbero_mt_epoll_http_ParserData {
    struct vinbero_common_ClModule* clModule;
    char* buffer;
    size_t bufferCapacity;
    size_t bufferSize;
    bool wasURLFinished;
    bool wasHeaderValueBefore;
    bool isFirstBodyChunk;
    bool isMessageCompleted;
};

VINBERO_INTERFACE_MODULE_FUNCTIONS;
VINBERO_INTERFACE_TLOCAL_FUNCTIONS;
VINBERO_INTERFACE_CLOCAL_FUNCTIONS;
VINBERO_INTERFACE_CLSERVICE_FUNCTIONS;

static int
vinbero_mt_epoll_http_on_message_begin(http_parser* parser) {
    struct vinbero_mt_epoll_http_ParserData* parserData = parser->data;

/*    parserData->buffer
    = malloc(parserData->bufferCapacity);
    parserData->bodyBuffer
    = malloc(parserData->bodyBufferCapacity);

    if you call malloc() here,
    This may result in allocating memory too frequently
    when the request is keep-alive. But I am not sure
    whether this callback is called multiple times in keep-alive request.
*/
    parserData->bufferSize = 0;
    parserData->wasURLFinished = false;
    parserData->wasHeaderValueBefore = false;
    parserData->isFirstBodyChunk = true;
    parserData->isMessageCompleted = false;

    struct vinbero_mt_epoll_http_Module* localModule = parserData->clModule->tlModule->module->localModule.pointer;
    struct vinbero_common_ClModule* childClModule = &GENC_TREE_NODE_GET_CHILD(parserData->clModule, 0);


    return localModule->childInterface
           .vinbero_interface_HTTP_onRequestStart
            (childClModule)
            == VINBERO_COMMON_STATUS_SUCCESS ? 0 : -1;
}

static int
vinbero_mt_epoll_http_on_url
(http_parser* parser, const char* at, size_t length) {
    struct vinbero_mt_epoll_http_ParserData* parserData = parser->data;
    if(genc_Nstr_cat
       (parserData->buffer, parserData->bufferCapacity,
        &parserData->bufferSize, at, length) == -1) return VINBERO_COMMON_ERROR_NO_SPACE;
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int
vinbero_mt_epoll_http_on_header_field
(http_parser* parser, const char* at, size_t length) {
    int ret;
    struct vinbero_mt_epoll_http_ParserData* parserData = parser->data;
    struct vinbero_mt_epoll_http_Module* localModule = parserData->clModule->tlModule->module->localModule.pointer;
    struct vinbero_common_ClModule* childClModule = &GENC_TREE_NODE_GET_CHILD(parserData->clModule, 0);

    if(parserData->wasURLFinished == false) {
        parserData->wasURLFinished = true;
        ret = localModule->childInterface
              .vinbero_interface_HTTP_onRequestUri
               (childClModule, (const char*)parserData->buffer, parserData->bufferSize);
        parserData->bufferSize = 0;
        if(ret < VINBERO_COMMON_STATUS_SUCCESS)
            return ret;
    }

    if(parserData->wasHeaderValueBefore == true) {
        parserData->wasHeaderValueBefore = false;
        ret = localModule->childInterface
               .vinbero_interface_HTTP_onRequestHeaderValue
                (childClModule, (const char*)parserData->buffer, parserData->bufferSize);
        parserData->bufferSize = 0;
        if(ret < VINBERO_COMMON_STATUS_SUCCESS)
            return ret;
    }
    if(genc_Nstr_cat
       (parserData->buffer, parserData->bufferCapacity,
        &parserData->bufferSize, at, length) == -1) return VINBERO_COMMON_ERROR_NO_SPACE;

    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int
vinbero_mt_epoll_http_on_header_value(http_parser* parser, const char* at, size_t length) {
    int ret;
    struct vinbero_mt_epoll_http_ParserData* parserData = parser->data;
    struct vinbero_mt_epoll_http_Module* localModule = parserData->clModule->tlModule->module->localModule.pointer;
    struct vinbero_common_ClModule* childClModule = &GENC_TREE_NODE_GET_CHILD(parserData->clModule, 0);

    if(parserData->wasHeaderValueBefore == false) {
        parserData->wasHeaderValueBefore = true;
        ret = localModule->childInterface
               .vinbero_interface_HTTP_onRequestHeaderField
                (childClModule, (const char*)parserData->buffer, parserData->bufferSize);
        parserData->bufferSize = 0;
        if(ret < VINBERO_COMMON_STATUS_SUCCESS)
            return ret;
    }

    if(genc_Nstr_cat
       (parserData->buffer, parserData->bufferCapacity,
        &parserData->bufferSize, at, length) == -1) return VINBERO_COMMON_ERROR_NO_SPACE;

    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int
vinbero_mt_epoll_http_on_headers_complete(http_parser* parser) {
    int ret;
    struct vinbero_mt_epoll_http_ParserData* parserData = parser->data;
    struct vinbero_mt_epoll_http_Module* localModule = parserData->clModule->tlModule->module->localModule.pointer;
    struct vinbero_mt_epoll_http_ClModule* localClModule = parserData->clModule->localClModule.pointer;
    struct vinbero_common_ClModule* childClModule = &GENC_TREE_NODE_GET_CHILD(parserData->clModule, 0);

    if(parserData->wasHeaderValueBefore == true) {
        parserData->wasHeaderValueBefore = false;
        ret = localModule->childInterface
               .vinbero_interface_HTTP_onRequestHeaderValue
                (childClModule, (const char*)parserData->buffer, parserData->bufferSize);
        parserData->bufferSize = 0;
        if(ret < VINBERO_COMMON_STATUS_SUCCESS)
            return ret;
    } 

    localClModule->isKeepAlive = http_should_keep_alive(parser) != 0;
    ret = localModule->childInterface
           .vinbero_interface_HTTP_onRequestKeepAlive
            (childClModule, localClModule->isKeepAlive);

    ret = localModule->childInterface
    .vinbero_interface_HTTP_onRequestMethod(childClModule,
     http_method_str(parser->method), strlen(http_method_str(parser->method)));
    if(ret < VINBERO_COMMON_STATUS_SUCCESS)
        return ret;

    ret = localModule->childInterface
    .vinbero_interface_HTTP_onRequestVersionMajor(childClModule, parser->http_major);
    if(ret < VINBERO_COMMON_STATUS_SUCCESS)
        return ret;

    ret = localModule->childInterface
    .vinbero_interface_HTTP_onRequestVersionMinor(childClModule, parser->http_minor);
    if(ret < VINBERO_COMMON_STATUS_SUCCESS)
        return ret;

    return localModule->childInterface
           .vinbero_interface_HTTP_onRequestHeadersFinish(childClModule);
}

static int
vinbero_mt_epoll_http_on_body(http_parser* parser, const char* at, size_t length) {
    struct vinbero_mt_epoll_http_ParserData* parserData = parser->data;
    struct vinbero_mt_epoll_http_Module* localModule = parserData->clModule->tlModule->module->localModule.pointer;
    struct vinbero_common_ClModule* childClModule = &GENC_TREE_NODE_GET_CHILD(parserData->clModule, 0);

    if(parserData->isFirstBodyChunk == true) {
        parserData->isFirstBodyChunk = false;
        return localModule->childInterface
               .vinbero_interface_HTTP_onRequestBodyStart(childClModule);
    }
    if(http_body_is_final(parser)) {
        return localModule->childInterface
               .vinbero_interface_HTTP_onRequestBodyFinish(childClModule);
    }
    return localModule->childInterface
           .vinbero_interface_HTTP_onRequestBody(childClModule, at, length);

}
// onRequestBodyFinish is useless;
// because any cleanup stuff you want to do is available at onRequestFinish
static int
vinbero_mt_epoll_http_on_message_complete(http_parser* parser) {
    int ret;
    struct vinbero_mt_epoll_http_ParserData* parserData = parser->data;
    struct vinbero_mt_epoll_http_Module* localModule = parserData->clModule->tlModule->module->localModule.pointer;
    struct vinbero_mt_epoll_http_ClModule* localClModule = parserData->clModule->localClModule.pointer;
    struct vinbero_common_ClModule* childClModule = &GENC_TREE_NODE_GET_CHILD(parserData->clModule, 0);
    parserData->isMessageCompleted = true;
    struct gaio_Io* clientIo = localClModule->clientIo;
    if(fcntl(clientIo->methods->fileno(clientIo), F_SETFL, fcntl(clientIo->methods->fileno(clientIo), F_GETFL, 0) & ~O_NONBLOCK) == -1)
        return VINBERO_COMMON_ERROR_UNKNOWN;
    childClModule->arg = localClModule->clientIo;
    ret = localModule->childInterface.vinbero_interface_HTTP_onRequestFinish(childClModule);
    if(ret < VINBERO_COMMON_STATUS_SUCCESS) {
        VINBERO_COMMON_LOG_ERROR("vinbero_interface_HTTP_onRequestFinish() Failed");
        return ret;
    }

    if(fcntl(clientIo->methods->fileno(clientIo), F_SETFL, fcntl(clientIo->methods->fileno(clientIo), F_GETFL, 0) | O_NONBLOCK) == -1)
        return VINBERO_COMMON_ERROR_UNKNOWN;

   return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeBytes(struct vinbero_interface_HTTP_Response* response, const char* bytes, size_t bytesSize) {
    response->io->methods->write(response->io, (void*)bytes, bytesSize);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeIo(struct vinbero_interface_HTTP_Response* response, struct gaio_Io* io, size_t bytesSize) {
    response->io->methods->sendfile(response->io, io, NULL, bytesSize);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeCrLf(struct vinbero_interface_HTTP_Response* response) {
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeVersion(struct vinbero_interface_HTTP_Response* response, int major, int minor) {
    response->io->methods->write(response->io, "HTTP/", sizeof("HTTP/") - 1);

    char* majorString;
    size_t majorStringSize;
    majorStringSize = genc_Uint_toNstr(major, 10, &majorString);
    response->io->methods->write(response->io, majorString, majorStringSize);
    free(majorString);

    response->io->methods->write(response->io, ".", sizeof(".") - 1);

    char* minorString;
    size_t minorStringSize;
    minorStringSize = genc_Uint_toNstr(minor, 10, &minorString);
    response->io->methods->write(response->io, minorString, minorStringSize);
    free(minorString);

    response->io->methods->write(response->io, " ", sizeof(" ") - 1);

    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeStatusCode(struct vinbero_interface_HTTP_Response* response, int statusCode) {
    char* statusCodeString;
    size_t statusCodeStringSize;
    statusCodeStringSize = genc_Uint_toNstr(statusCode, 10, &statusCodeString);
    response->io->methods->write(response->io, statusCodeString, statusCodeStringSize);
    free(statusCodeString);

    response->io->methods->write(response->io, " \r\n", sizeof(" \r\n") - 1);

    return VINBERO_COMMON_STATUS_SUCCESS;
}
static int vinbero_mt_epoll_http_writeIntHeader(struct vinbero_interface_HTTP_Response* response, const char* headerField, size_t headerFieldSize, int headerValue) {
    response->io->methods->write(response->io, (void*)headerField, headerFieldSize);
    response->io->methods->write(response->io, ": ", sizeof(": ") - 1);
    char* headerValueString;
    size_t headerValueStringSize;
    headerValueStringSize = genc_Uint_toNstr(headerValue, 10, &headerValueString);
    response->io->methods->write(response->io, headerValueString, headerValueStringSize);
    free(headerValueString);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);

    return VINBERO_COMMON_STATUS_SUCCESS;
}
static int vinbero_mt_epoll_http_writeDoubleHeader(struct vinbero_interface_HTTP_Response* response, const char* headerField, size_t headerFieldSize, double headerValue) {
    // not implemented yet
    return VINBERO_COMMON_STATUS_SUCCESS;
}
static int vinbero_mt_epoll_http_writeStringHeader(struct vinbero_interface_HTTP_Response* response, const char* headerField, size_t headerFieldSize, const char* headerValue, size_t headerValueSize) {
    response->io->methods->write(response->io, (void*)headerField, headerFieldSize);
    response->io->methods->write(response->io, ": ", sizeof(": ") - 1);
    response->io->methods->write(response->io, (void*)headerValue, headerValueSize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeStringBody(struct vinbero_interface_HTTP_Response* response, const char* stringBody, size_t stringBodySize) {
    // TODO: add keep-alive here
    vinbero_mt_epoll_http_writeIntHeader(response, "Content-Length", sizeof("Content-Length") - 1, stringBodySize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    response->io->methods->write(response->io, (void*)stringBody, stringBodySize);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeIoBody(struct vinbero_interface_HTTP_Response* response, struct gaio_Io* ioBody, size_t ioBodySize) {
    vinbero_mt_epoll_http_writeIntHeader(response, "Content-Length", sizeof("Content-Length") - 1, ioBodySize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    response->io->methods->sendfile(response->io, ioBody, NULL, ioBodySize);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeChunkedBodyStart(struct vinbero_interface_HTTP_Response* response) {
    vinbero_mt_epoll_http_writeStringHeader(response, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1, "chunked", sizeof("chunked") - 1);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeChunkedBody(struct vinbero_interface_HTTP_Response* response, const char* stringBody, size_t stringBodySize) {
    char* stringBodySizeString;
    size_t stringBodySizeStringSize;
    stringBodySizeStringSize = genc_Uint_toNstr(stringBodySize, 16, &stringBodySizeString);
    response->io->methods->write(response->io, stringBodySizeString, stringBodySizeStringSize);
    free(stringBodySizeString);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    response->io->methods->write(response->io, (void*)stringBody, stringBodySize);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static int vinbero_mt_epoll_http_writeChunkedBodyEnd(struct vinbero_interface_HTTP_Response* response) {
    response->io->methods->write(response->io, "0\r\n", sizeof("0\r\n") - 1);
    response->io->methods->write(response->io, "\r\n", sizeof("\r\n") - 1);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

int vinbero_interface_MODULE_init(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;

    vinbero_common_Module_init(module, "vinbero_mt_epoll_http", VINBERO_MT_EPOLL_HTTP_VERSION, true);
    module->localModule.pointer = calloc(1, sizeof(struct vinbero_mt_epoll_http_Module));

    struct vinbero_mt_epoll_http_Module* localModule = module->localModule.pointer;
    struct vinbero_common_Module* childModule = &GENC_TREE_NODE_GET_CHILD(module, 0);

    VINBERO_INTERFACE_HTTP_DLSYM(&localModule->childInterface, &childModule->dlHandle, &ret);
    if(ret < VINBERO_COMMON_STATUS_SUCCESS)
        return ret;

    int parserBufferCapacity;
    vinbero_common_Config_getInt(module->config, module, "vinbero_mt_epoll_http.parserBufferCapacity", &parserBufferCapacity, 4 * 1024);

    localModule->parserBufferCapacity = parserBufferCapacity;


    localModule->responseMethods.writeBytes = vinbero_mt_epoll_http_writeBytes;
    localModule->responseMethods.writeIo = vinbero_mt_epoll_http_writeIo;
    localModule->responseMethods.writeCrLf = vinbero_mt_epoll_http_writeCrLf;
    localModule->responseMethods.writeVersion = vinbero_mt_epoll_http_writeVersion;
    localModule->responseMethods.writeStatusCode = vinbero_mt_epoll_http_writeStatusCode;
    localModule->responseMethods.writeIntHeader = vinbero_mt_epoll_http_writeIntHeader;
    localModule->responseMethods.writeDoubleHeader = vinbero_mt_epoll_http_writeDoubleHeader;
    localModule->responseMethods.writeStringHeader = vinbero_mt_epoll_http_writeStringHeader;
    localModule->responseMethods.writeStringBody = vinbero_mt_epoll_http_writeStringBody;
    localModule->responseMethods.writeIoBody = vinbero_mt_epoll_http_writeIoBody;
    localModule->responseMethods.writeChunkedBodyStart = vinbero_mt_epoll_http_writeChunkedBodyStart;
    localModule->responseMethods.writeChunkedBody = vinbero_mt_epoll_http_writeChunkedBody;
    localModule->responseMethods.writeChunkedBodyEnd = vinbero_mt_epoll_http_writeChunkedBodyEnd;

    localModule->parserCallbacks.on_message_begin = vinbero_mt_epoll_http_on_message_begin;
    localModule->parserCallbacks.on_url = vinbero_mt_epoll_http_on_url;
    localModule->parserCallbacks.on_status = NULL;
    localModule->parserCallbacks.on_header_field = vinbero_mt_epoll_http_on_header_field;
    localModule->parserCallbacks.on_header_value = vinbero_mt_epoll_http_on_header_value;
    localModule->parserCallbacks.on_headers_complete = vinbero_mt_epoll_http_on_headers_complete;
    localModule->parserCallbacks.on_body = vinbero_mt_epoll_http_on_body;
    localModule->parserCallbacks.on_message_complete = vinbero_mt_epoll_http_on_message_complete;
    localModule->parserCallbacks.on_chunk_header = NULL;
    localModule->parserCallbacks.on_chunk_complete = NULL;

    return VINBERO_COMMON_STATUS_SUCCESS;
}

int vinbero_interface_MODULE_rInit(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    return VINBERO_COMMON_STATUS_SUCCESS;
}

int vinbero_interface_TLOCAL_init(struct vinbero_common_TlModule* tlModule) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
/*
    GENC_TREE_NODE_FOR_EACH_CHILD(tlModule, index) {
        struct vinbero_common_TlModule* childTlModule = &GENC_TREE_NODE_GET_CHILD(tlModule, index);
        VINBERO_COMMON_CALL(TLOCAL, init, childTlModule->module, &ret, childTlModule);
        if(ret < VINBERO_COMMON_STATUS_SUCCESS)
            return ret;
    }
*/
    return VINBERO_COMMON_STATUS_SUCCESS;
}

int vinbero_interface_TLOCAL_rInit(struct vinbero_common_TlModule* tlModule) {
    VINBERO_COMMON_LOG_TRACE2();
    return VINBERO_COMMON_STATUS_SUCCESS;
}

int vinbero_interface_CLOCAL_init(struct vinbero_common_ClModule* clModule) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;

    struct vinbero_mt_epoll_http_Module* localModule = clModule->tlModule->module->localModule.pointer;
    clModule->localClModule.pointer = malloc(1 * sizeof(struct vinbero_mt_epoll_http_ClModule));
    struct vinbero_mt_epoll_http_ClModule* localClModule = clModule->localClModule.pointer;

    localClModule->clientIo = clModule->arg;
    localClModule->isKeepAlive = false;

    http_parser_init(&localClModule->parser, HTTP_REQUEST);
    localClModule->parser.data = malloc(sizeof(struct vinbero_mt_epoll_http_ParserData));
    struct vinbero_mt_epoll_http_ParserData* parserData = localClModule->parser.data;
    parserData->clModule = clModule;
    parserData->bufferCapacity = localModule->parserBufferCapacity * sizeof(char);
    parserData->buffer = malloc(localModule->parserBufferCapacity * sizeof(char));
    parserData->bufferSize = 0;

    localClModule->clientResponse.clModule = clModule;
    localClModule->clientResponse.methods = &localModule->responseMethods;
    localClModule->clientResponse.io = localClModule->clientIo;

    clModule->arg = &localClModule->clientResponse;
/*
    GENC_TREE_NODE_FOR_EACH_CHILD(clModule, index) {
        struct vinbero_common_ClModule* childClModule = &GENC_TREE_NODE_GET_CHILD(clModule, index);
        childClModule->arg = &localClModule->clientResponse;
        VINBERO_COMMON_CALL(CLOCAL, init, childClModule->tlModule->module, &ret, childClModule);
        if(ret < VINBERO_COMMON_STATUS_SUCCESS)
            return ret;

    }
*/
    return VINBERO_COMMON_STATUS_SUCCESS;
}

static inline int
vinbero_mt_epoll_http_readRequest(struct vinbero_common_ClModule* clModule) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    struct vinbero_mt_epoll_http_Module* localModule = clModule->tlModule->module->localModule.pointer;
    struct vinbero_mt_epoll_http_ClModule* localClModule = clModule->localClModule.pointer;
    struct http_parser* parser = &localClModule->parser;
    struct vinbero_mt_epoll_http_ParserData* parserData = parser->data;
    struct gaio_Io* clientIo = localClModule->clientIo;
    struct vinbero_common_ClModule* childClModule = &GENC_TREE_NODE_GET_CHILD(clModule, 0);

    ssize_t readSize;
    size_t bufferFreeCapacity = parserData->bufferCapacity - parserData->bufferSize;
    if(bufferFreeCapacity == 0) {
        VINBERO_COMMON_LOG_ERROR("Parser buffer is full");
        return VINBERO_COMMON_ERROR_NO_SPACE;
    }
    while((readSize = clientIo->methods->read(clientIo, parserData->buffer + parserData->bufferSize, bufferFreeCapacity)) > 0) {
        VINBERO_COMMON_LOG_DEBUG("Read client socket %d bytes", readSize); 
        if((ret = http_parser_execute(parser, &localModule->parserCallbacks, parserData->buffer, readSize)) < readSize || HTTP_PARSER_ERRNO(parser) != HPE_OK) {
            VINBERO_COMMON_LOG_ERROR("Parser error: %s %s", http_errno_name(parser->http_errno), http_errno_description(parser->http_errno));
            return VINBERO_COMMON_ERROR_INVALID_DATA;
        }
    }
    if(readSize == -1) {
        if(errno == EAGAIN) {
            VINBERO_COMMON_LOG_DEBUG("Client socket EAGAIN");
            return VINBERO_COMMON_STATUS_AGAIN;
        } else if(errno == EWOULDBLOCK) {
            VINBERO_COMMON_LOG_DEBUG("Client socket EWOULDBLOCK");
            return VINBERO_COMMON_STATUS_AGAIN;
        } else
            VINBERO_COMMON_LOG_ERROR("Client socket read() error");
        return VINBERO_COMMON_ERROR_READ;
    }
    else if(readSize == 0) {
        VINBERO_COMMON_LOG_DEBUG("Read client socket %d bytes", readSize); 
        VINBERO_COMMON_LOG_DEBUG("Client socket has been closed");
        return VINBERO_COMMON_STATUS_SUCCESS;
    }
    return VINBERO_COMMON_STATUS_SUCCESS; // request finsihed
}

int vinbero_interface_CLSERVICE_call(struct vinbero_common_ClModule* clModule) {
    VINBERO_COMMON_LOG_TRACE2();
    int ret;
    struct vinbero_mt_epoll_http_ClModule* localClModule = clModule->localClModule.pointer;
    struct gaio_Io* clientIo = localClModule->clientIo;
    struct vinbero_common_ClModule* childClModule = &GENC_TREE_NODE_GET_CHILD(clModule, 0);
    ret = vinbero_mt_epoll_http_readRequest(clModule);
    if(ret < VINBERO_COMMON_STATUS_SUCCESS)
        return ret;

    if(ret == VINBERO_COMMON_STATUS_AGAIN || ret == VINBERO_COMMON_STATUS_CONTINUE)
        return VINBERO_COMMON_STATUS_AGAIN;

    return VINBERO_COMMON_STATUS_SUCCESS;
}

int vinbero_interface_CLOCAL_destroy(struct vinbero_common_ClModule* clModule) {
    VINBERO_COMMON_LOG_TRACE2();
    struct vinbero_mt_epoll_http_ClModule* localClModule = clModule->localClModule.pointer;
    struct vinbero_mt_epoll_http_ParserData* parserData = localClModule->parser.data;
    free(parserData->buffer);
    free(localClModule);
    return VINBERO_COMMON_STATUS_SUCCESS;
}

int vinbero_interface_TLOCAL_destroy(struct vinbero_common_TlModule* tlModule) {
    int ret;
    VINBERO_COMMON_LOG_TRACE2();
/*
    GENC_TREE_NODE_FOR_EACH_CHILD(tlModule, index) {
        struct vinbero_common_TlModule* childTlModule = &GENC_TREE_NODE_GET_CHILD(tlModule, index);
        VINBERO_COMMON_CALL(TLOCAL, destroy, childTlModule->module, &ret, childTlModule);
        if(ret < VINBERO_COMMON_STATUS_SUCCESS)
            return ret;
    }
    return VINBERO_COMMON_STATUS_SUCCESS;
*/
}

int vinbero_interface_TLOCAL_rDestroy(struct vinbero_common_TlModule* tlModule) {
    VINBERO_COMMON_LOG_TRACE2();
    return VINBERO_COMMON_STATUS_SUCCESS;
}

int vinbero_interface_MODULE_destroy(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    return VINBERO_COMMON_STATUS_SUCCESS;
}

int vinbero_interface_MODULE_rDestroy(struct vinbero_common_Module* module) {
    VINBERO_COMMON_LOG_TRACE2();
    free(module->localModule.pointer);
    return VINBERO_COMMON_STATUS_SUCCESS;
}
