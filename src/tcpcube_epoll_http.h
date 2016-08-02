#ifndef _TCPCUBE_EPOLL_HTTP_H
#define _TCPCUBE_EPOLL_HTTP_H

#include <pthread.h>

struct tcpcube_epoll_data
{
    int fd;
    void* custom_data;
};

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list);
int tcpcube_epoll_module_service(struct tcpcube_module* module, struct tcpcube_epoll_data*);
int tcpcube_epoll_module_destroy(struct tcpcube_module* module);

#endif
