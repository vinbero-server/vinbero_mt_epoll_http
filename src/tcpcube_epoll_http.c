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

int tcpcube_epoll_module_service(struct tcpcube_module* module, int* client_socket, char* client_buffer, size_t client_buffer_size)
{
    ssize_t read_count;
    while((read_count = read(*client_socket, client_buffer, client_buffer_size)) > 0)
    {
        write(STDOUT_FILENO, client_buffer, read_count);
    }
    if(read_count == -1)
        return 1;
    else if(read_count == 0)
        return 0;
}

int tcpcube_epoll_module_destroy(struct tcpcube_module* module)
{
    free(module);
    return 0;
}
