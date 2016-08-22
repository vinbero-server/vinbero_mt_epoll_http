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
    return parser->buffer + parser->token_offset;
}

size_t tucube_epoll_http_parser_get_buffer_left(struct tucube_epoll_http_parser* parser)
{
    return parser->header_buffer_capacity - parser->token_offset;
}

int tucube_epoll_http_parser_parse_message_header(struct tucube_module* module, struct tucube_cldata* cldata, struct tucube_epoll_http_parser* parser, ssize_t read_size)
{
    size_t buffer_size = parser->token_offset + read_size;
    parser->buffer_offset = parser->token_offset;
    parser->token = parser->buffer;

    while(parser->buffer_offset < buffer_size)
    {
        switch(parser->state)
        {
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
            if(GONC_CAST(module->pointer,
                 struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_method(GONC_LIST_ELEMENT_NEXT(module),
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
            if(GONC_CAST(module->pointer,
                 struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_uri(GONC_LIST_ELEMENT_NEXT(module),
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
                if(GONC_CAST(module->pointer,
                     struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_version(GONC_LIST_ELEMENT_NEXT(module),
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
            {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN:
            GONC_DEBUG("HEADER_FIELD_BEGIN (%c)", parser->buffer[parser->buffer_offset]);
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
            GONC_DEBUG("HEADER_FIELD_END (%c)", parser->buffer[parser->buffer_offset]);
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
            }
            else
            {
                if(GONC_CAST(module->pointer,
                     struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_field(GONC_LIST_ELEMENT_NEXT(module),
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
            GONC_DEBUG("HEADER_VALUE_BEGIN (%c)", parser->buffer[parser->buffer_offset]);
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
            GONC_DEBUG("HEADER_VALUE_END (%c)", parser->buffer[parser->buffer_offset]);
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                if(GONC_CAST(module->pointer,
                     struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_value(GONC_LIST_ELEMENT_NEXT(module),
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
                return 0;
            }
            else
            {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_ERROR:
            return -1;
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

int tucube_epoll_http_parser_parse_message_body(struct tucube_module* module, struct tucube_cldata* cldata, struct tucube_epoll_http_parser* parser, ssize_t read_size)
{
    return 0;
}

