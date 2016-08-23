#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <tucube/tucube_module.h>
#include <tucube/tucube_cldata.h>
#include <libgonc/gonc_cast.h>
#include <libgonc/gonc_debug.h>
#include "tucube_epoll_http.h"
#include "tucube_epoll_http_parser.h"

char* tucube_epoll_http_parser_get_buffer_position(struct tucube_epoll_http_parser* parser)
{
    if(parser->state < TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN)
        return parser->buffer + parser->token_offset;
    return parser->buffer;
}

size_t tucube_epoll_http_parser_get_buffer_size(struct tucube_epoll_http_parser* parser)
{
    if(parser->state < TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN)
        return parser->header_buffer_capacity - parser->token_offset;
    return parser->body_remainder;
}

static inline int tucube_epoll_http_parser_parse_headers(struct tucube_module* module, struct tucube_cldata* cldata, struct tucube_epoll_http_parser* parser, ssize_t read_size)
{
    size_t buffer_end  = parser->token_offset + read_size;
    parser->buffer_offset = parser->token_offset;
    parser->token = parser->buffer;

    while(parser->buffer_offset < buffer_end)
    {
        switch(parser->state)
        {
        case TUCUBE_EPOLL_HTTP_PARSER_HEADERS_BEGIN:
            GONC_DEBUG("HEADERS_BEGIN");
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN:
            GONC_DEBUG("METHOD_BEGIN");
            parser->token_offset = 0;
            parser->token = parser->buffer + parser->buffer_offset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_METHOD;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_METHOD:
            GONC_DEBUG("METHOD");
	    if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_METHOD_END;
            }
            else if(parser->buffer[parser->buffer_offset] < 'A' || parser->buffer[parser->buffer_offset] > 'z')
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_METHOD_END:
            GONC_DEBUG("METHOD_END");
            if(parser->on_method(GONC_LIST_ELEMENT_NEXT(module),
                 GONC_LIST_ELEMENT_NEXT(cldata),
                 parser->token,
                 parser->token_offset) == -1)
            {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            }
            else
            {
                parser->token_offset = 0;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_URI_BEGIN;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_URI_BEGIN:
            GONC_DEBUG("URI_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_URI;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_URI:
            GONC_DEBUG("URI");
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_URI_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_URI_END:
            GONC_DEBUG("URI_END");
            if(parser->on_uri(GONC_LIST_ELEMENT_NEXT(module),
                 GONC_LIST_ELEMENT_NEXT(cldata),
                 parser->token,
                 parser->token_offset) == -1)
            {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            }
            else
            {
                parser->token_offset = 0;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_VERSION_BEGIN;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_VERSION_BEGIN:
            GONC_DEBUG("VERSION_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_VERSION;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_VERSION:
            GONC_DEBUG("VERSION");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_VERSION_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_VERSION_END:
            GONC_DEBUG("VERSION_END");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                if(parser->on_version(GONC_LIST_ELEMENT_NEXT(module),
                      GONC_LIST_ELEMENT_NEXT(cldata),
                      parser->token,
                      parser->token_offset) == -1)
                {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                }
                else
                {
                    parser->token_offset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
                }
            }
            else
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN:
            GONC_DEBUG("HEADER_FIELD_BEGIN", parser->buffer[parser->buffer_offset]);
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADERS_END;
            }
            else
            {
                parser->token = parser->buffer + parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD:
            GONC_DEBUG("HEADER_FIELD (%c)", parser->buffer[parser->buffer_offset]);
            if(parser->buffer[parser->buffer_offset] == ':')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END:
            GONC_DEBUG("HEADER_FIELD_END", parser->buffer[parser->buffer_offset]);
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
            }
            else
            {
                if(parser->on_header_field(GONC_LIST_ELEMENT_NEXT(module),
                     GONC_LIST_ELEMENT_NEXT(cldata),
                     parser->token,
                     parser->token_offset) == -1)
                {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                }
                else
                {
                    parser->token_offset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN;
                }
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN:
            GONC_DEBUG("HEADER_VALUE_BEGIN", parser->buffer[parser->buffer_offset]);
            parser->token = parser->buffer + parser->buffer_offset;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE;            
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE:
            GONC_DEBUG("HEADER_VALUE (%c)", parser->buffer[parser->buffer_offset]);
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END:
            GONC_DEBUG("HEADER_VALUE_END", parser->buffer[parser->buffer_offset]);
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                if(parser->on_header_value(GONC_LIST_ELEMENT_NEXT(module),
                     GONC_LIST_ELEMENT_NEXT(cldata),
                     parser->token,
                     parser->token_offset) == -1)
                {
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                }
                else
                {
                    parser->token_offset = 0;
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
                }
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADERS_END:
            GONC_DEBUG("HEADERS_END");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                if(parser->on_headers_finish(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata)) == -1)
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                else
                {
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

    if(parser->header_buffer_capacity - parser->token_offset == 0)
    {
        warnx("%s: %u: A token is bigger than http_buffer", __FILE__, __LINE__);
        parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
        return -1;
    }

    memmove(parser->buffer, parser->token, parser->token_offset * sizeof(char));
    return 1;
}

static inline int tucube_epoll_http_parser_parse_body(struct tucube_module* module, struct tucube_cldata* cldata, struct tucube_epoll_http_parser* parser, ssize_t read_size)
{
    GONC_DEBUG("parse_body()");

    int result;
    if(parser->state == TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN)
    {
        GONC_DEBUG("BODY_BEGIN");
        if((result = parser->get_content_length(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &parser->body_remainder)) == -1)
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
        else if(result == 0)
        {
            if(parser->on_request_finish(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata)) == -1)
            {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                return -1;
            }
            return 0;
        }
        else
        {
            if(read_size - parser->buffer_offset > parser->body_buffer_capacity)
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            else
            {
                memmove(parser->buffer, parser->buffer + parser->buffer_offset, (read_size - parser->buffer_offset) * sizeof(char));
                parser->buffer = realloc(parser->buffer, parser->body_buffer_capacity * sizeof(char));
                read_size = read_size - parser->buffer_offset;
                parser->buffer_offset = 0;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_BODY;
            }
        }
    }

    while(parser->body_remainder > 0)
    {
        switch(parser->state)
        {
        case TUCUBE_EPOLL_HTTP_PARSER_BODY:
            GONC_DEBUG("BODY");
            if(read_size == 0)
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            else if(read_size < parser->body_remainder)
            {
                if(parser->on_body_chunk(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), parser->buffer, read_size) == -1)
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                else
                    parser->body_remainder -= read_size;
            }
            else if(read_size >= parser->body_remainder)
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_BODY_END;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_BODY_END:
            GONC_DEBUG("BODY_END");
            if(parser->on_body_chunk(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), parser->buffer, read_size) == -1)
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            else
            {
                if(parser->on_body_finish(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata)) == -1)
                    parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
                else
                {
                    if(parser->on_request_finish(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata)) == -1)
                    {
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
    return 0;
}

int tucube_epoll_http_parser_parse(struct tucube_module* module, struct tucube_cldata* cldata, struct tucube_epoll_http_parser* parser, ssize_t read_size)
{
    int result;
    if(parser->state < TUCUBE_EPOLL_HTTP_PARSER_BODY_BEGIN)
    {
        if((result = tucube_epoll_http_parser_parse_headers(module, cldata, parser, read_size)) != 0)
            return result;
        return tucube_epoll_http_parser_parse_body(module, cldata, parser, read_size);
    }
    else
        return tucube_epoll_http_parser_parse_body(module, cldata, parser, read_size);
}
