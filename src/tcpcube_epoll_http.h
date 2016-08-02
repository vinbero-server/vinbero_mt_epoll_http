#ifndef _TCPCUBE_EPOLL_HTTP_H
#define _TCPCUBE_EPOLL_HTTP_H

#include <pthread.h>

struct tcpcube_epoll_http_module
{
    pthread_key_t http_parser_key;
    pthread_key_t http_parser_settings_key;
};

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list);
int tcpcube_epoll_module_service(struct tcpcube_module* module, int* client_socket);
int tcpcube_epoll_module_destroy(struct tcpcube_module* module);

#endif
