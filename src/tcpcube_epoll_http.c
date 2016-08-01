#include <stdlib.h>
#include <unistd.h>
#include <tcpcube/tcpcube_module.h>
#include <libgonc/gonc_list.h>
#include "tcpcube_epoll_http.h"

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list)
{
    struct tcpcube_module* module = malloc(sizeof(struct tcpcube_module));
    GONC_LIST_ELEMENT_INIT(module);
    module->object = malloc(sizeof(struct tcpcube_epoll_http_module));
    module->object_size = sizeof(struct tcpcube_epoll_http_module);
    TCPCUBE_MODULE_CAST(module->object, struct tcpcube_epoll_http_module*)->http_buffer_list = malloc(sizeof(struct tcpcube_epoll_http_buffer_list));
    GONC_LIST_INIT(TCPCUBE_MODULE_CAST(module->object, struct tcpcube_epoll_http_module*)->http_buffer_list);

    GONC_LIST_APPEND(module_list, module);
    return 0;
}

int tcpcube_epoll_module_service(struct tcpcube_module* module, int* client_socket)
{
    struct tcpcube_epoll_http_buffer_list* http_buffer_list = malloc(sizeof(struct tcpcube_epoll_http_buffer_list));
    GONC_LIST_INIT(http_buffer_list);
    struct tcpcube_epoll_http_buffer* http_buffer = malloc(sizeof(struct tcpcube_epoll_http_buffer));
    
    http_buffer->data_size = 1024;
    http_buffer->data = malloc(1024);
    

    size_t read_size;
    while((read_size = read(*client_socket, http_buffer->data, http_buffer->data_size)) > 0)
    {
        http_buffer->data_size = read_size;
        GONC_LIST_APPEND(http_buffer_list, http_buffer);
    }
    if(read_size == -1)
        return 1;
    else if(read_size == 0)
        return 0;
}

int tcpcube_epoll_module_destroy(struct tcpcube_module* module)
{
    free(module);
    return 0;
}
