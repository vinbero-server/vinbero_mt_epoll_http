#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <tcpcube/tcpcube_module.h>
#include <libgonc/gonc_list.h>
#include <http_parser.h>
#include "tcpcube_epoll_http.h"
/*
  http_cb      on_message_begin;
  http_data_cb on_url;
  http_data_cb on_status;
  http_data_cb on_header_field;
  http_data_cb on_header_value;
  http_cb      on_headers_complete;
  http_data_cb on_body;
  http_cb      on_message_complete;

  http_cb      on_chunk_header;
  http_cb      on_chunk_complete;
*/

int on_header_field_callback(http_parser* parser, const char* at, size_t length)
{
    warnx("on_header_field_callback");
    write(STDOUT_FILENO, at, length);
    write(STDOUT_FILENO, "\n", 1);
}

int on_header_value_callback(http_parser* parser, const char* at, size_t length)
{
    warnx("on_header_value_callback");
    write(STDOUT_FILENO, at, length);
    write(STDOUT_FILENO, "\n", 1);
}

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list)
{
    struct tcpcube_module* module = malloc(sizeof(struct tcpcube_module));
    GONC_LIST_ELEMENT_INIT(module);
    GONC_LIST_APPEND(module_list, module);
    return 0;
}

int tcpcube_epoll_module_service(struct tcpcube_module* module, struct tcpcube_epoll_data* client_data)
{
    if(client_data->ptr == 0)
    {
        warnx("client_data->ptr not initialized");
        client_data->ptr = malloc(sizeof(struct tcpcube_epoll_http_client_data));
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser = malloc(sizeof(struct http_parser));
        http_parser_init(((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser, HTTP_REQUEST);
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser->data = &client_data->fd; // why is this needed?
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser_settings = malloc(sizeof(struct http_parser_settings));
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser_settings->on_header_field = on_header_field_callback;
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser_settings->on_header_value = on_header_value_callback;
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_buffer = malloc(1024 * sizeof(char));
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_buffer_size = 1024;
    }
    ssize_t read_size;
    int parsed_size;
    while((read_size = read(client_data->fd, ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_buffer, ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_buffer_size)) > 0)
    {
        warnx("read chars: %d", read_size);
        while((parsed_size = http_parser_execute(((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser,
             ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser_settings,
             ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_buffer,
             read_size)) > 0)
        {
            warnx("http_parser_execute(), parsed_size: %d", parsed_size);
        }
    }
    if(read_size == -1)
    {
        if(errno == EAGAIN)
        {
            warnx("%s: %u: client socket EAGAIN", __FILE__, __LINE__);
            return 1;
        }
        else
        {
            warn("%s: %u", __FILE__, __LINE__);
            return -1;
        }
    }
    else if(read_size == 0)
    {
        warnx("%s: %u: client socket EOF", __FILE__, __LINE__);
        free(((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser);
        free(((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser_settings);
        free(((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_buffer);
        free(client_data->ptr);
        client_data->ptr = NULL;
        return 0;
    }
}

int tcpcube_epoll_module_destroy(struct tcpcube_module* module)
{
    free(module);
    return 0;
}
