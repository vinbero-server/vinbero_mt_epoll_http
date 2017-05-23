#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tucube/tucube_Module.h>
#include <tucube/tucube_ClData.h>
#include <libgonc/gonc_cast.h>
#include <libgonc/gonc_nstrncasecmp.h>
#include "tucube_epoll_http.h"
#include "tucube_epoll_http_Parser.h"

int tucube_epoll_http_Parser_init(struct tucube_epoll_http_Parser* parser, size_t headerBufferCapacity, size_t bodyBufferCapacity) {
    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADERS_BEGIN;
    parser->headerBufferCapacity = headerBufferCapacity;
    parser->bodyBufferCapacity = bodyBufferCapacity;
    parser->buffer = malloc(1 * headerBufferCapacity * sizeof(char)); 
    parser->bufferOffset = 0;
    parser->bufferSize = 0;
    parser->token = NULL;
    parser->tokenOffset = 0; 
    parser->bodyRemainder = 0;
    parser->isKeepAlive = false; 
    return 0;
}

int tucube_epoll_http_Parser_reset(struct tucube_epoll_http_Parser* parser) {
    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADERS_BEGIN;
    parser->buffer = realloc(parser->buffer, parser->headerBufferCapacity * sizeof(char)); 
    parser->bufferOffset = 0;
    parser->bufferSize = 0;
    parser->token = NULL;
    parser->tokenOffset = 0; 
    parser->bodyRemainder = 0;
    parser->isKeepAlive = false;
    return 0;
}

static inline char* tucube_epoll_http_Parser_getBufferPosition(struct tucube_epoll_http_Parser* parser) {
    if(parser->state < TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN)
        return parser->buffer + parser->tokenOffset;
    return parser->buffer;
}

static inline size_t tucube_epoll_http_Parser_getAvailableBufferSize(struct tucube_epoll_http_Parser* parser) {
    if(parser->state < TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN)
        return parser->headerBufferCapacity - parser->tokenOffset;
    return parser->bodyRemainder;
}

int tucube_epoll_http_Parser_read(struct tucube_Module* module, struct tucube_ClData* clData, struct tucube_epoll_http_Parser* parser) {
    return read(
        *GONC_CAST(clData->pointer, struct tucube_epoll_http_ClData*)->clientSocket,
        tucube_epoll_http_Parser_getBufferPosition(parser),
        tucube_epoll_http_Parser_getAvailableBufferSize(parser)
    );
}

static inline int tucube_epoll_http_Parser_parseHeaders(struct tucube_Module* module, struct tucube_ClData* clData, struct tucube_epoll_http_Parser* parser) {
    parser->bufferOffset = parser->tokenOffset;
    parser->token = parser->buffer;

    while(parser->bufferOffset < parser->bufferSize) {
        switch(parser->state) {
        case TUCUBE_EPOLL_HTTP_PARSER_HEADERS_BEGIN:

            GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestStart(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData));
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN:
            parser->tokenOffset = 0;
            parser->token = parser->buffer + parser->bufferOffset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_METHOD;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_METHOD:
            if(parser->buffer[parser->bufferOffset] == ' ') {
                ++parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_METHOD_END;
            }
            else if(parser->buffer[parser->bufferOffset] < 'A' || parser->buffer[parser->bufferOffset] > 'z')
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            else {
                ++parser->tokenOffset;
                ++parser->bufferOffset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_METHOD_END:
            if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestMethod(GONC_LIST_ELEMENT_NEXT(module),
                 GONC_LIST_ELEMENT_NEXT(clData),
                 parser->token,
                 parser->tokenOffset) == -1) {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            }
            else {
                parser->tokenOffset = 0;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI_BEGIN;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI_BEGIN:
            parser->token = parser->buffer + parser->bufferOffset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI:
            if(parser->buffer[parser->bufferOffset] == ' ') {
                ++parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI_END;
            }
            else {
                ++parser->tokenOffset;
                ++parser->bufferOffset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_REQUEST_URI_END:
            if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestUri(GONC_LIST_ELEMENT_NEXT(module),
                 GONC_LIST_ELEMENT_NEXT(clData),
                 parser->token,
                 parser->tokenOffset) == -1) {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            }
            else {
                parser->tokenOffset = 0;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_BEGIN;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_BEGIN:
            parser->token = parser->buffer + parser->bufferOffset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL:
            if(parser->buffer[parser->bufferOffset] == '\r') {
                ++parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_END;
            }
            else {
                ++parser->tokenOffset;
                ++parser->bufferOffset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_END:
            if(parser->buffer[parser->bufferOffset] == '\n') {
                ++parser->bufferOffset;
                if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestProtocol(GONC_LIST_ELEMENT_NEXT(module),
                      GONC_LIST_ELEMENT_NEXT(clData),
                      parser->token,
                      parser->tokenOffset) == -1) {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                }
                else {
                    parser->tokenOffset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
                }
            }
            else
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN:
            if(parser->buffer[parser->bufferOffset] == '\r') {
                ++parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADERS_END;
            }
            else {
                parser->token = parser->buffer + parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD:
            if(parser->buffer[parser->bufferOffset] == ':') {
                ++parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END;
            }
            else {
                ++parser->tokenOffset;
                ++parser->bufferOffset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END:
            if(parser->buffer[parser->bufferOffset] == ' ') {
                ++parser->bufferOffset;
            }
            else {
                if(gonc_nstrncasecmp(parser->token, parser->tokenOffset, "X-Script-Name", sizeof("X-Script-Name") - 1) == 0) {
                    parser->tokenOffset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH_BEGIN;
                }
                else if(gonc_nstrncasecmp(parser->token, parser->tokenOffset, "Content-Type", sizeof("Content-Type") - 1) == 0) {
                    parser->tokenOffset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE_BEGIN;
                }
                else if(gonc_nstrncasecmp(parser->token, parser->tokenOffset, "Content-Length", sizeof("Content-Length") - 1) == 0) {
                    parser->tokenOffset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_CONTENT_LENGTH_BEGIN;
                }
                else if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestHeaderField(GONC_LIST_ELEMENT_NEXT(module),
                     GONC_LIST_ELEMENT_NEXT(clData),
                     parser->token,
                     parser->tokenOffset) == -1) {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                }
                else {
                    parser->tokenOffset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN;
                }
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH_BEGIN:
            parser->token = parser->buffer + parser->bufferOffset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH;            
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH:
            if(parser->buffer[parser->bufferOffset] == '\r') {
                ++parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH_END;
            }
            else {
                ++parser->tokenOffset;
                ++parser->bufferOffset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_SCRIPT_PATH_END:
            if(parser->buffer[parser->bufferOffset] == '\n') {
                ++parser->bufferOffset;
                if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestScriptPath(GONC_LIST_ELEMENT_NEXT(module),
                     GONC_LIST_ELEMENT_NEXT(clData),
                     parser->token,
                     parser->tokenOffset) == -1)
                {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                }
                else
                {
                    parser->tokenOffset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
                }
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE_BEGIN:
            parser->token = parser->buffer + parser->bufferOffset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE;            
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE:
            if(parser->buffer[parser->bufferOffset] == '\r') {
                ++parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE_END;
            }
            else {
                ++parser->tokenOffset;
                ++parser->bufferOffset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_CONTENT_TYPE_END:
            if(parser->buffer[parser->bufferOffset] == '\n') {
                ++parser->bufferOffset;
                if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestContentType(GONC_LIST_ELEMENT_NEXT(module),
                     GONC_LIST_ELEMENT_NEXT(clData),
                     parser->token,
                     parser->tokenOffset) == -1) {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                }
                else {
                    parser->tokenOffset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
                }
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_CONTENT_LENGTH_BEGIN:
            parser->token = parser->buffer + parser->bufferOffset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_CONTENT_LENGTH;            
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_CONTENT_LENGTH:
            if(parser->buffer[parser->bufferOffset] == '\r') {
                ++parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_CONTENT_LENGTH_END;
            }
            else {
                ++parser->tokenOffset;
                ++parser->bufferOffset;
            }
 
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_CONTENT_LENGTH_END:
            if(parser->buffer[parser->bufferOffset] == '\n') {
                ++parser->bufferOffset;
                if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestContentLength(GONC_LIST_ELEMENT_NEXT(module),
                     GONC_LIST_ELEMENT_NEXT(clData),
                     parser->token,
                     parser->tokenOffset) == -1) {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                }
                else {
                    parser->tokenOffset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
                }
            }
 
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN:
            parser->token = parser->buffer + parser->bufferOffset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE;            
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE:
            if(parser->buffer[parser->bufferOffset] == '\r') {
                ++parser->bufferOffset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END;
            }
            else {
                ++parser->tokenOffset;
                ++parser->bufferOffset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END:
            if(parser->buffer[parser->bufferOffset] == '\n') {
                ++parser->bufferOffset;
                if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestHeaderValue(GONC_LIST_ELEMENT_NEXT(module),
                     GONC_LIST_ELEMENT_NEXT(clData),
                     parser->token,
                     parser->tokenOffset) == -1) {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                }
                else {
                    parser->tokenOffset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
                }
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADERS_END:
            if(parser->buffer[parser->bufferOffset] == '\n') {
                ++parser->bufferOffset;
                if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestHeadersFinish(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData)) == -1)
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                else {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN;
                    return 0;
                }
            }
            else
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_ERROR:
            return -1;
            break;
        default:
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            break;
        }
    }

    if(parser->headerBufferCapacity - parser->tokenOffset == 0) {
        warnx("%s: %u: A token is bigger than http_buffer", __FILE__, __LINE__);
        parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
        return -1;
    }

    memmove(parser->buffer, parser->token, parser->tokenOffset * sizeof(char));
    return 1;
}

static inline int tucube_epoll_http_Parser_parseBody(struct tucube_Module* module, struct tucube_ClData* clData, struct tucube_epoll_http_Parser* parser) {
    int result;
    if(parser->state == TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN) {
        if((result = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onGetRequestContentLength(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData), &parser->bodyRemainder)) == -1)
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
        else if(parser->bodyRemainder == 0) {
            if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestFinish(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData)) == -1) {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                return -1;
            }
            return 0;
        }
        else {
            if(parser->bodyBufferCapacity < parser->bodyRemainder) {
                warnx("%s: %u: Request body is bigger than parser body buffer", __FILE__, __LINE__);
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                return -1;
            }
            else if(parser->bufferSize - parser->bufferOffset > parser->bodyBufferCapacity) {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                return -1;
            }
            else {
                memmove(parser->buffer, parser->buffer + parser->bufferOffset, (parser->bufferSize - parser->bufferOffset) * sizeof(char));
                parser->buffer = realloc(parser->buffer, parser->bodyBufferCapacity * sizeof(char));
                parser->bufferSize -= parser->bufferOffset;
                parser->bufferOffset = 0;
                if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBodyStart(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData)) == -1) {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                    return -1;
                }
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_BODY;
            }
        }
    }

    while(parser->bodyRemainder > 0 && parser->bufferSize > 0) {
        switch(parser->state) {
        case TUCUBE_EPOLL_HTTP_PARSER_BODY:
            if(parser->bufferSize < parser->bodyRemainder) {
                if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBody(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData), parser->buffer, parser->bufferSize) == -1)
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                else {
                    parser->bodyRemainder -= parser->bufferSize;
                    parser->bufferSize = 0;
                }
            }
            else if(parser->bufferSize >= parser->bodyRemainder)
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_BODY_END;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_BODY_END:
            if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBody(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData), parser->buffer, parser->bufferSize) == -1)
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            else {
                parser->bufferSize = 0;
                if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBodyFinish(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData)) == -1)
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                else {
                    if(GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestFinish(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(clData)) == -1) {
                        parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                        return -1;
                    }
                    return 0;
                }
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_ERROR:
            return -1;
        default:
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            break;
        }
    }
    return 1;
}

int tucube_epoll_http_Parser_parse(struct tucube_Module* module, struct tucube_ClData* clData, struct tucube_epoll_http_Parser* parser, ssize_t readSize) {
    parser->bufferSize = parser->tokenOffset + readSize;
    int result;
    if(parser->state < TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN) {
        if((result = tucube_epoll_http_Parser_parseHeaders(module, clData, parser)) != 0)
            return result;
    }
    return tucube_epoll_http_Parser_parseBody(module, clData, parser);
}
