#ifndef _TUCUBE_EHTTP_PARSER_H
#define _TUCUBE_EHTTP_PARSER_H

enum tucube_ehttp_parser_state
{
    TUCUBE_EHTTP_PARSER_METHOD_BEGIN,
    TUCUBE_EHTTP_PARSER_METHOD,
    TUCUBE_EHTTP_PARSER_METHOD_END,
    TUCUBE_EHTTP_PARSER_URL_BEGIN,
    TUCUBE_EHTTP_PARSER_URL,
    TUCUBE_EHTTP_PARSER_URL_END,
    TUCUBE_EHTTP_PARSER_VERSION_BEGIN,
    TUCUBE_EHTTP_PARSER_VERSION,
    TUCUBE_EHTTP_PARSER_VERSION_END,
    TUCUBE_EHTTP_PARSER_HEADER_FIELD_BEGIN,
    TUCUBE_EHTTP_PARSER_HEADER_FIELD,
    TUCUBE_EHTTP_PARSER_HEADER_FIELD_END,
    TUCUBE_EHTTP_PARSER_HEADER_VALUE_BEGIN,
    TUCUBE_EHTTP_PARSER_HEADER_VALUE,
    TUCUBE_EHTTP_PARSER_HEADER_VALUE_END,
    TUCUBE_EHTTP_PARSER_HEADER_END,
    TUCUBE_EHTTP_PARSER_ERROR
};

struct tucube_ehttp_parser
{
    enum tucube_ehttp_parser_state state;
    char* buffer;
    ssize_t buffer_offset;
    ssize_t buffer_capacity;
    char* token;
    ssize_t token_offset;
};

int tucube_ehttp_parser_parse_message_header(struct tucube_ehttp_parser* parser, ssize_t buffer_size);
int tucube_ehttp_parser_parse_message_body(struct tucube_ehttp_parser* parser, ssize_t buffer_size);

#endif
