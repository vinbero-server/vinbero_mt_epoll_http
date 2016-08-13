#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include "tcpcube_epoll_http_parser.h"

int tcpcube_epoll_http_parser_parse_message_header(struct tcpcube_epoll_http_parser* parser, ssize_t buffer_size)
{
    parser->buffer_offset = 0;
    parser->token = parser->buffer + parser->buffer_offset;

    while(parser->buffer_offset < buffer_size)
    {
        switch(parser->state)
        {
        case TCPCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN:
            fprintf(stderr, "METHOD_BEGIN\n");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_METHOD;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_METHOD:
//            fprintf(stderr, "METHOD");
	    if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_METHOD_END;
            }
            else if(parser->buffer[parser->buffer_offset] < 'A' || parser->buffer[parser->buffer_offset] > 'z')
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_ERROR;
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_METHOD_END:
            fprintf(stderr, "\nMETHOD_END\n");
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_URL_BEGIN;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_URL_BEGIN:
            fprintf(stderr, "URL_BEGIN\n");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_URL;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_URL:
//            fprintf(stderr, "URL");
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_URL_END;
            }
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_URL_END:
            fprintf(stderr, "\nURL_END\n");
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_VERSION_BEGIN;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_VERSION_BEGIN:
            fprintf(stderr, "VERSION_BEGIN\n");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_VERSION;
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_VERSION:
//            fprintf(stderr, "VERSION");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_VERSION_END;
            }
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_VERSION_END:
            fprintf(stderr, "\nVERSION_END\n");
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
            fprintf(stderr, "HEADER_FIELD_BEGIN\n");
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
//            fprintf(stderr, "HEADER_FIELD");
            if(parser->buffer[parser->buffer_offset] == ':')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END;
            }
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END:
            fprintf(stderr, "\nHEADER_FIELD_END\n");
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
            fprintf(stderr, "HEADER_VALUE_BEGIN\n");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE;            
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE:
//            fprintf(stderr, "HEADER_VALUE");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END;
            }
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END:
            fprintf(stderr, "\nHEADER_VALUE_END\n");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                parser->state = TCPCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_PARSER_HEADER_END:
            fprintf(stderr, "\nHEADER_END\n");
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

int tcpcube_epoll_http_parser_parse_message_body(struct tcpcube_epoll_http_parser* parser, ssize_t buffer_size)
{
    return 0;
}

