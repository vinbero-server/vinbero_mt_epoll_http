#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <tcpcube/tcpcube_module.h>
#include <libgonc/gonc_list.h>
#include "tcpcube_epoll_http.h"

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list)
{
    struct tcpcube_module* module = malloc(sizeof(struct tcpcube_module));
    GONC_LIST_ELEMENT_INIT(module);
    GONC_LIST_APPEND(module_list, module);
    return 0;
}
#include <stdio.h>
int tcpcube_epoll_module_service(struct tcpcube_module* module, int* client_socket)
{
    ssize_t buffer_size = 10;
    char* buffer = malloc(buffer_size);
    ssize_t read_size;
    while((read_size = read(*client_socket, buffer, buffer_size - 1)) > 0)
    {
        buffer[read_size] = '\0';
        printf("READ(%d): (%s)\n", read_size, buffer);
    }
    if(read_size == -1)
    {
        if(errno == EAGAIN)
            return 1;
        else
            return -1;
    }
    else if(read_size == 0)
        return 0;
}

int tcpcube_epoll_module_destroy(struct tcpcube_module* module)
{
    free(module);
    return 0;
}
