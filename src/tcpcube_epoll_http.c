#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <tcpcube/tcpcube_module.h>
#include <libgonc/gonc_list.h>
#include "tcpcube_epoll_http.h"
#include "tcpcube_epoll_http_parser.h"

int tcpcube_epoll_module_init(struct tcpcube_module_args* module_args, struct tcpcube_module_list* module_list)
{
    struct tcpcube_module* module = malloc(sizeof(struct tcpcube_module));
    GONC_LIST_ELEMENT_INIT(module);
    GONC_LIST_APPEND(module_list, module);
    return 0;
}

int tcpcube_epoll_module_service(struct tcpcube_module* module, struct tcpcube_epoll_data* client_data)
{
    if(client_data->ptr == 0)
    {
        warnx("client_data->ptr not initialized");
        client_data->ptr = malloc(sizeof(struct tcpcube_epoll_http_client_data));
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser = malloc(sizeof(struct tcpcube_epoll_http_parser));
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser->buffer = malloc(1024 * sizeof(char));
        ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser->buffer_size = 1024;
    }
    ssize_t read_size;
    while((read_size = read(client_data->fd, ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser->buffer, ((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser->buffer_size)) > 0)
    {
        warnx("read chars: %d", read_size);
        tcpcube_epoll_http_parser_parse(((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser, read_size);
    }
    if(read_size == -1)
    {
        if(errno == EAGAIN)
        {
            warnx("%s: %u: client socket EAGAIN", __FILE__, __LINE__);
            return 1;
        }
        else
        {
            warn("%s: %u", __FILE__, __LINE__);
            return -1;
        }
    }
    else if(read_size == 0)
    {
        warnx("%s: %u: client socket EOF", __FILE__, __LINE__);
        free(((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser->buffer);
        free(((struct tcpcube_epoll_http_client_data*)client_data->ptr)->http_parser);
        free(client_data->ptr);
        client_data->ptr = NULL;
        return 0;
    }
}

int tcpcube_epoll_module_destroy(struct tcpcube_module* module)
{
    free(module);
    return 0;
}
