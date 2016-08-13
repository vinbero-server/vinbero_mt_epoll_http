#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include "tucube_epoll_http_parser.h"

int tucube_epoll_http_parser_parse_message_header(struct tucube_epoll_http_parser* parser, ssize_t buffer_size)
{
    parser->buffer_offset = 0;
    parser->token = parser->buffer + parser->buffer_offset;

    while(parser->buffer_offset < buffer_size)
    {
        switch(parser->state)
        {
        case TUCUBE_EPOLL_HTTP_PARSER_METHOD_BEGIN:
            fprintf(stderr, "METHOD_BEGIN\n");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_METHOD;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_METHOD:
//            fprintf(stderr, "METHOD");
	    if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_METHOD_END;
            }
            else if(parser->buffer[parser->buffer_offset] < 'A' || parser->buffer[parser->buffer_offset] > 'z')
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_METHOD_END:
            fprintf(stderr, "\nMETHOD_END\n");
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_URL_BEGIN;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_URL_BEGIN:
            fprintf(stderr, "URL_BEGIN\n");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_URL;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_URL:
//            fprintf(stderr, "URL");
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_URL_END;
            }
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_URL_END:
            fprintf(stderr, "\nURL_END\n");
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_BEGIN;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_BEGIN:
            fprintf(stderr, "PROTOCOL_BEGIN\n");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL;
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL:
//            fprintf(stderr, "PROTOCOL");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_END;
            }
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_PROTOCOL_END:
            fprintf(stderr, "\nPROTOCOL_END\n");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
            }
            else
            {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_ERROR;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN:
            fprintf(stderr, "HEADER_FIELD_BEGIN\n");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_END;
            }
            else
            {
                parser->token = parser->buffer + parser->buffer_offset;
                parser->token_offset = 0;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD:
//            fprintf(stderr, "HEADER_FIELD");
            if(parser->buffer[parser->buffer_offset] == ':')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END;
            }
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_END:
            fprintf(stderr, "\nHEADER_FIELD_END\n");
            if(parser->buffer[parser->buffer_offset] == ' ')
            {
                ++parser->buffer_offset;
            }
            else
            {
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_BEGIN:
            fprintf(stderr, "HEADER_VALUE_BEGIN\n");
            parser->token = parser->buffer + parser->buffer_offset;
            parser->token_offset = 0;
            parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE;            
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE:
//            fprintf(stderr, "HEADER_VALUE");
            if(parser->buffer[parser->buffer_offset] == '\r')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END;
            }
            else
            {
                fputc(parser->buffer[parser->buffer_offset], stderr);
                ++parser->token_offset;
                ++parser->buffer_offset;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_VALUE_END:
            fprintf(stderr, "\nHEADER_VALUE_END\n");
            if(parser->buffer[parser->buffer_offset] == '\n')
            {
                ++parser->buffer_offset;
                parser->state = TUCUBE_EPOLL_HTTP_PARSER_HEADER_FIELD_BEGIN;
            }
            break;
        case TUCUBE_EPOLL_HTTP_PARSER_HEADER_END:
            fprintf(stderr, "\nHEADER_END\n");
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
    return 1;
}

int tucube_epoll_http_parser_parse_message_body(struct tucube_epoll_http_parser* parser, ssize_t buffer_size)
{
    return 0;
}

