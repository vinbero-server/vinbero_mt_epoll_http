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

int tcpcube_epoll_module_tlinit(struct tcpcube_module* module, struct tcpcube_module_args* module_args)
{
    return 0;
}

int tcpcube_epoll_module_clinit(struct tcpcube_epoll_cldata_list* cldata_list, int client_socket)
{
    struct tcpcube_epoll_cldata* cldata = malloc(sizeof(struct tcpcube_epoll_cldata));
    GONC_LIST_ELEMENT_INIT(cldata);
    cldata->data = malloc(sizeof(struct tcpcube_epoll_http_cldata));
    ((struct tcpcube_epoll_http_cldata*)cldata->data)->client_socket = client_socket;
    ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser = calloc(1, sizeof(struct tcpcube_epoll_http_parser));
    ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->buffer = malloc(32 * sizeof(char));
    ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->buffer_capacity = 32;
    GONC_LIST_APPEND(cldata_list, cldata);
    return 0;
}

int tcpcube_epoll_module_service(struct tcpcube_module* module, struct tcpcube_epoll_cldata* cldata)
{
    ssize_t read_size;
    while((read_size = read(((struct tcpcube_epoll_http_cldata*)cldata->data)->client_socket,
         ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->buffer +
         ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->token_offset,
         ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->buffer_capacity -
         ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->token_offset)) > 0)
    {
        if(tcpcube_epoll_http_parser_parse_message_header(((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser,
             ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->token_offset + read_size) <= 0)
        {
            if(((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->state == TCPCUBE_EPOLL_HTTP_PARSER_ERROR)
                warnx("%s: %u: parser error", __FILE__, __LINE__);
            break;
        }
        else
        {
            memmove(((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->buffer,
                 ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->token,
                 ((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->token_offset);
        }
    }
    if(read_size == -1)
    {
        if(errno == EAGAIN)
        {
            warnx("%s: %u: client socket EAGAIN", __FILE__, __LINE__);
            return 1;
        }
        else if(errno == EFAULT)
        {
            warnx("%s: %u: a token is bigger than http_buffer", __FILE__, __LINE__);
            return -1;
        }
        else
        {
            warn("%s: %u", __FILE__, __LINE__);
            return -1;
        }
    }

    warnx("%s: %u: end of http request", __FILE__, __LINE__);
    free(((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->buffer);
    free(((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser);
    free(cldata->data);
    cldata->data = NULL;

    return 0;
}

int tcpcube_epoll_module_cldestroy(struct tcpcube_epoll_cldata* cldata)
{
    warnx("cldestroy()");

    free(((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser->buffer);
    free(((struct tcpcube_epoll_http_cldata*)cldata->data)->http_parser);
//    close(((struct tcpcube_epoll_http_cldata*)cldata->data)->client_socket);
    free(cldata->data);
    free(cldata);

    return 0;
}

int tcpcube_epoll_module_tldestroy(struct tcpcube_module* module)
{
    warnx("tcpcube_epoll_module_tldestroy()");
    return 0;
}

int tcpcube_epoll_module_destroy(struct tcpcube_module* module)
{
    free(module);
    return 0;
}
