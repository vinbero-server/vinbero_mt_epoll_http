#include <err.h>
#include <stdlib.h>
#include "tcpcube_epoll_http_parser.h"

int tcpcube_epoll_http_parser_parse(struct tcpcube_epoll_http_parser* parser, ssize_t buffer_size)
{
    parser->buffer_offset = 0;
    parser->token = parser->buffer + parser->buffer_offset;

    while(parser->buffer_offset < buffer_size)
    {
        switch(parser->state)
        {
        case TCPCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN:
            warnx("METHOD_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_METHOD;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_METHOD:
            warnx("METHOD");
	    if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_METHOD_END;
            }
            else if(parser->buffer[parser->buffer_offset] < 'A' || parser->buffer[parser->buffer_offset] > 'z')
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_ERROR;
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_METHOD_END:
            warnx("METHOD_END");
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_URL_BEGIN;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_URL_BEGIN:
            warnx("URL_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_URL;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_URL:
            warnx("URL");
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_URL_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_URL_END:
            warnx("URL_END");
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_VERSION_BEGIN;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_VERSION_BEGIN:
            warnx("VERSION_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_VERSION;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_VERSION:
            warnx("VERSION");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_VERSION_END;
            }
            else
            {
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_VERSION_END:
            warnx("VERSION_END");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
            }
            else
            {
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_ERROR;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN:
            warnx("HEADER_FIELD_BEGIN");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_END;
            }
            else
            {
                parser->token = parser->buffer + parser->buffer_offset;
                parser->token_offset = 0;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD:
            warnx("HEADER_FIELD");
            if(parser->buffer[parser->buffer_offset] == ':')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END;
            }
            else
            {
                warnx("%c", parser->buffer[parser->buffer_offset]);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END:
            warnx("HEADER_FIELD_END");
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
            }
            else
            {
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN:
            warnx("HEADER_VALUE_BEGIN");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE;            
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE:
            warnx("HEADER_VALUE");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END;
            }
            else
            {
                warnx("%c", parser->buffer[parser->buffer_offset]);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END:
            warnx("HEADER_VALUE_END");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_END:
            warnx("HEADER_END");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                return 0;
            }
            else
            {
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_ERROR;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_ERROR:
            return -1;
            break;
        }
    }
    return 1;
}
