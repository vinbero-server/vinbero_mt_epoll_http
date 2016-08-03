#include "tcpcube_epoll_http_lexer.h"

int tcpcube_epoll_http_lexer_lex(struct tcpcube_epoll_http_lexer* lexer)
{
    while(lexer->buffer_offset < buffer_size)
    {
        switch(lexer->state)
        {
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_METHOD_BEGIN:
            lexer->token = lexer->buffer[lexer->buffer_offset];
            lexer->token_offset = lexer->buffer_offset;
            lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_METHOD;
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_METHOD:
	    if(lexer->buffer[lexer->buffer_offset] == ' ')
            {
                ++lexer->buffer_offset;
                lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_METHOD_END;
                return 0;
            }
            else if(lexer->buffer[lexer->buffer_offset] < 'A' || lexer->buffer[lexer->buffer_offset] > 'z')
                return -1;
            else
            {
                ++lexer->token_offset;
                ++lexer->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_METHOD_END:
            lexer->state = TCPCUBE_HTTP_LEXER_HTTP_URL_BEGIN;
            return 0;
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_URL_BEGIN:
            lexer->token = lexer->buffer[lexer->buffer_offset];
            lexer->token_offset = lexer->buffer_offset;
            lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_URL;
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_URL:
            if(lexer->buffer[lexer->buffer_offset] == ' ')
            {
                ++lexer->buffer_offset;
                lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_URL_END;
            }
            else
            {
                ++lexer->token_offset;
                ++lexer->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_URL_END:
            lexer->state = TCPCUBE_HTTP_LEXER_HTTP_VERSION_BEGIN;
            return 0;
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_VERSION_BEGIN:
            lexer->token = lexer->buffer[lexer->buffer_offset];
            lexer->token_offset = lexer->buffer_offset;
            lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_VERSION;
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_VERSION:
            if(lexer->buffer[lexer->buffer_offset] == '\r')
            {
                ++lexer->buffer_offset;
                lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_VERSION_END;
            }
            else
            {
                ++lexer->token_offset;
                ++lexer->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_VERSION_END:
            if(lexer->buffer[lexer->buffer_offset] == '\n')
            {
                ++lexer->buffer_offset;
                lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_FIELD_BEGIN;
                return 0;
            }
            else
            {
                return -1;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_FIELD_BEGIN:
            lexer->token = lexer->buffer[lexer->buffer_offset];
            lexer->token_offset = lexer->buffer_offset;
            lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_FIELD;
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_FIELD:
            if(lexer->buffer[lexer->buffer_offset] == ':')
            {
                ++lexer->buffer_offset;
                lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_FIELD_END;
            }
            else
            {
                ++lexer->token_offset;
                ++lexer->buffer_offset;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_FIELD_END:
            if(lexer->buffer[lexer->buffer_offset] == ' ')
            {
                ++lexer->buffer_offset;
            }
            else
            {
                lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_VALUE_BEGIN;
                return 0;
            }
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_VALUE_BEGIN:
            lexer->token = lexer->buffer[lexer->buffer_offset];
            lexer->token_offset = lexer->buffer_offset;
            lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_VALUE;
            
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_VALUE:
            lexer->state = TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_VALUE_END;
            break;
        case TCPCUBE_EPOLL_HTTP_LEXER_HTTP_HEADER_VALUE_END:
            return 0;
            break;
        }
    }
    return 1;
}

