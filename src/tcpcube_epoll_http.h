#ifndef _TCPCUBE_EPOLL_HTTP_H
#define _TCPCUBE_EPOLL_HTTP_H

#include <http_parser.h>

struct tcpcube_epoll_data
{
    int fd;
    void* ptr;
};

struct tcpcube_epoll_http_client_data
{
    struct http_parser* http_parser;
    struct http_parser_settings* http_parser_settings;
    char* http_buffer;
    ssize_t http_buffer_size;
};

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list);
int tcpcube_epoll_module_service(struct tcpcube_module* module, struct tcpcube_epoll_data*);
int tcpcube_epoll_module_destroy(struct tcpcube_module* module);

#endif
