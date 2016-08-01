#ifndef _TCPCUBE_EPOLL_HTTP_H
#define _TCPCUBE_EPOLL_HTTP_H

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list);
int tcpcube_epoll_module_service(struct tcpcube_module* module, int* client_socket, char* client_buffer, size_t client_buffer_size);
int tcpcube_epoll_module_destroy(struct tcpcube_module* module);

#endif
