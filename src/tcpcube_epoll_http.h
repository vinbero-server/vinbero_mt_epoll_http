#ifndef _TCPCUBE_EPOLL_HTTP_H
#define _TCPCUBE_EPOLL_HTTP_H

#include <tcpcube/tcpcube_module.h>
#include <libgonc/gonc_list.h>

struct tcpcube_epoll_http_buffer
{
    char* data;
    ssize_t data_size;
    GONC_LIST_ELEMENT(struct tcpcube_epoll_http_buffer);
};

struct tcpcube_epoll_http_buffer_list
{
    GONC_LIST(struct tcpcube_epoll_http_buffer);
};

struct tcpcube_epoll_http_module
{
    struct tcpcube_epoll_http_buffer_list* http_buffer_list;
};

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list);
int tcpcube_epoll_module_service(struct tcpcube_module* module, int* client_socket);
int tcpcube_epoll_module_destroy(struct tcpcube_module* module);

#endif
