#include <err.h>
#include <stdlib.h>
#include "tcpcube_epoll_http_parser.h"

int tcpcube_epoll_http_parser_parse(struct tcpcube_epoll_http_parser* parser, ssize_t read_size)
{
    parser->buffer_offset = 0;
    while(parser->buffer_offset < read_size)
    {
        switch(parser->state)
        {
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_METHOD_BEGIN:
            warnx("HTTP_METHOD_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_METHOD;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_METHOD:
            warnx("HTTP_METHOD");
	    if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_METHOD_END;
            }
            else if(parser->buffer[parser->buffer_offset] < 'A' || parser->buffer[parser->buffer_offset] > 'z')
                return -1;
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_METHOD_END:
            warnx("HTTP_METHOD_END");
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_URL_BEGIN;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_URL_BEGIN:
            warnx("HTTP_URL_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_URL;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_URL:
            warnx("HTTP_URL");
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_URL_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_URL_END:
            warnx("HTTP_URL_END");
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_VERSION_BEGIN;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_VERSION_BEGIN:
            warnx("HTTP_VERSION_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_VERSION;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_VERSION:
            warnx("HTTP_VERSION");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_VERSION_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_VERSION_END:
            warnx("HTTP_VERSION_END");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_FIELD_BEGIN;
            }
            else
            {
                return -1;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_FIELD_BEGIN:
            warnx("HTTP_HEADER_FIELD_BEGIN");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_END;
            }
            else
            {
                parser->token = parser->buffer + parser->buffer_offset;
                parser->token_offset = 0;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_FIELD;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_FIELD:
            warnx("HTTP_HEADER_FIELD");
            if(parser->buffer[parser->buffer_offset] == ':')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_FIELD_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_FIELD_END:
            warnx("HTTP_HEADER_FIELD_END");
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
            }
            else
            {
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_VALUE_BEGIN;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_VALUE_BEGIN:
            warnx("HTTP_HEADER_VALUE_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_VALUE;            
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_VALUE:
            warnx("HTTP_HEADER_VALUE");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_VALUE_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_VALUE_END:
            warnx("HTTP_HEADER_VALUE_END");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_FIELD_BEGIN;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HTTP_HEADER_END:
            warnx("HTTP_HEADER_END");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                return 0;
            }
            else
            {
                return -1;
            }
            break;
        }
    }
    return 1;
}
