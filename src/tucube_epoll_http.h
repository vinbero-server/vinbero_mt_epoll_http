#ifndef _TUCUBE_EPOLL_HTTP_H
#define _TUCUBE_EPOLL_HTTP_H

#include "tucube_epoll_http_parser.h"
#include "../../tucube_tcp_epoll/src/tucube_tcp_epoll_cldata.h"

struct tucube_epoll_http_module
{
    void* dl_handle;
    int (*tucube_epoll_http_module_init)(struct tucube_module_args*, struct tucube_module_list*);
    int (*tucube_epoll_http_module_tlinit)(struct tucube_module*, struct tucube_module_args*);
    int (*tucube_epoll_http_module_clinit)(struct tucube_module*, struct tucube_tcp_epoll_cldata_list*, int);

    int (*tucube_epoll_http_module_on_method)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_on_uri)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_on_version)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_on_header_field)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);
    int (*tucube_epoll_http_module_on_header_value)(struct tucube_module*, struct tucube_tcp_epoll_cldata*, char*, ssize_t);

    int (*tucube_epoll_http_module_service)(struct tucube_module*, struct tucube_tcp_epoll_cldata*);

    int (*tucube_epoll_http_module_cldestroy)(struct tucube_module*, struct tucube_tcp_epoll_cldata*);
    int (*tucube_epoll_http_module_tldestroy)(struct tucube_module*);
    int (*tucube_epoll_http_module_destroy)(struct tucube_module*);

    size_t parser_header_buffer_capacity;
    size_t parser_body_buffer_capacity;
};

struct tucube_epoll_http_cldata
{
    int client_socket;
    struct tucube_epoll_http_parser* parser;
};

int tucube_tcp_epoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list);
int tucube_tcp_epoll_module_tlinit(struct tucube_module* module, struct tucube_module_args* module_args);
int tucube_tcp_epoll_module_clinit(struct tucube_module* module, struct tucube_tcp_epoll_cldata_list* cldata_list, int client_socket);
int tucube_tcp_epoll_module_service(struct tucube_module* module, struct tucube_tcp_epoll_cldata*);
int tucube_tcp_epoll_module_cldestroy(struct tucube_module* module, struct tucube_tcp_epoll_cldata* cldata);
int tucube_tcp_epoll_module_tldestroy(struct tucube_module* module);
int tucube_tcp_epoll_module_destroy(struct tucube_module* module);

#endif
