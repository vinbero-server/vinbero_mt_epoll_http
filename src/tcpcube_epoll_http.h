#ifndef _TCPCUBE_EPOLL_HTTP_H
#define _TCPCUBE_EPOLL_HTTP_H

#include "tcpcube_epoll_http_parser.h"
#include "../../tcpcube_epoll/src/tcpcube_epoll.h"

struct tcpcube_epoll_http_cldata
{
    int client_socket;
    struct tcpcube_epoll_http_parser* http_parser;
};

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list);
int tcpcube_epoll_module_tlinit(struct tcpcube_module* module, struct tcpcube_module_args* module_args);
int tcpcube_epoll_module_clinit(struct tcpcube_epoll_cldata_list* cldata_list, int client_socket);
int tcpcube_epoll_module_service(struct tcpcube_module* module, struct tcpcube_epoll_cldata*);
int tcpcube_epoll_module_cldestroy(struct tcpcube_epoll_cldata* cldata);
int tcpcube_epoll_module_tldestroy(struct tcpcube_module* module);
int tcpcube_epoll_module_destroy(struct tcpcube_module* module);

#endif
