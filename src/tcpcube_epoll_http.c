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
    write(STDOUT_FILENO, at, length);
}

int on_header_value_callback(http_parser* parser, const char* at, size_t length)
{
    write(STDOUT_FILENO, at, length);
}

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list)
{
    struct tcpcube_module* module = malloc(sizeof(struct tcpcube_module));
    GONC_LIST_ELEMENT_INIT(module);
    module->object = malloc(sizeof(struct tcpcube_epoll_http_module));
    module->object_size = sizeof(struct tcpcube_epoll_http_module);

    pthread_key_create(&TCPCUBE_MODULE_CAST(module->object, struct tcpcube_epoll_http_module*)->http_parser_key, NULL);
    http_parser* parser = malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST);
    pthread_setspecific(TCPCUBE_MODULE_CAST(module->object, struct tcpcube_epoll_http_module*)->http_parser_key, parser);

    pthread_key_create(&TCPCUBE_MODULE_CAST(module->object, struct tcpcube_epoll_http_module*)->http_parser_settings_key, NULL);
    http_parser_settings* parser_settings = malloc(sizeof(http_parser_settings));
    parser_settings->on_header_field = on_header_field_callback;
    parser_settings->on_header_value = on_header_value_callback;
    pthread_setspecific(TCPCUBE_MODULE_CAST(module->object, struct tcpcube_epoll_http_module*)->http_parser_settings_key, parser_settings);

    pthread_key_create(&TCPCUBE_MODULE_CAST(module->object, struct tcpcube_epoll_http_module*)->http_parser_settings_key, NULL);

    GONC_LIST_APPEND(module_list, module);
    return 0;
}

int tcpcube_epoll_module_service(struct tcpcube_module* module, int* client_socket)
{
    http_parser parser;
    ssize_t buffer_size = 10;
    char* buffer = malloc(buffer_size);
    ssize_t read_size;
    while((read_size = read(*client_socket, buffer, buffer_size)) > 0)
    {
        warnx("%s", buffer);
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
        return 0;
    }
}

int tcpcube_epoll_module_destroy(struct tcpcube_module* module)
{
    pthread_key_delete(TCPCUBE_MODULE_CAST(module->object, struct tcpcube_epoll_http_module*)->http_parser_settings_key);
    pthread_key_delete(TCPCUBE_MODULE_CAST(module->object, struct tcpcube_epoll_http_module*)->http_parser_key);
    free(module->object);
    free(module);
    return 0;
}
