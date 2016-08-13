#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <tucube/tucube_module.h>
#include <libgonc/gonc_list.h>
#include "tucube_epoll_http.h"
#include "tucube_epoll_http_parser.h"
#include "../../tucube_tcp_epoll/src/tucube_tcp_epoll_cldata.h"

int tucube_tcp_epoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list)
{
    struct tucube_module* module = malloc(sizeof(struct tucube_module));
    GONC_LIST_ELEMENT_INIT(module);
    GONC_LIST_APPEND(module_list, module);
    return 0;
}

int tucube_tcp_epoll_module_tlinit(struct tucube_module* module, struct tucube_module_args* module_args)
{
    return 0;
}

int tucube_tcp_epoll_module_clinit(struct tucube_tcp_epoll_cldata_list* cldata_list, int client_socket)
{
    struct tucube_tcp_epoll_cldata* cldata = malloc(sizeof(struct tucube_tcp_epoll_cldata));
    GONC_LIST_ELEMENT_INIT(cldata);
    cldata->data = malloc(sizeof(struct tucube_epoll_http_cldata));
    ((struct tucube_epoll_http_cldata*)cldata->data)->client_socket = client_socket;
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser = calloc(1, sizeof(struct tucube_epoll_http_parser));
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->buffer = malloc(256 * sizeof(char));
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->buffer_capacity = 256;
    GONC_LIST_APPEND(cldata_list, cldata);
    return 0;
}

int tucube_tcp_epoll_module_service(struct tucube_module* module, struct tucube_tcp_epoll_cldata* cldata)
{
    ssize_t read_size;
    while((read_size = read(((struct tucube_epoll_http_cldata*)cldata->data)->client_socket,
         ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->buffer +
         ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->token_offset,
         ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->buffer_capacity -
         ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->token_offset)) > 0)
    {
        if(tucube_epoll_http_parser_parse_message_header(((struct tucube_epoll_http_cldata*)cldata->data)->http_parser,
             ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->token_offset + read_size) <= 0)
        {
            if(((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->state == TUCUBE_EPOLL_HTTP_PARSER_ERROR)
                warnx("%s: %u: parser error", __FILE__, __LINE__);
            break;
        }
        else
        {
            memmove(((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->buffer,
                 ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->token,
                 ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->token_offset);
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
    return 0;
}

int tucube_tcp_epoll_module_cldestroy(struct tucube_tcp_epoll_cldata* cldata)
{
    warnx("cldestroy()");

    free(((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->buffer);
    free(((struct tucube_epoll_http_cldata*)cldata->data)->http_parser);
//    close(((struct tucube_epoll_http_cldata*)cldata->data)->client_socket);
    free(cldata->data);
    free(cldata);

    return 0;
}

int tucube_tcp_epoll_module_tldestroy(struct tucube_module* module)
{
    warnx("tucube_tcp_epoll_module_tldestroy()");
    return 0;
}

int tucube_tcp_epoll_module_destroy(struct tucube_module* module)
{
    free(module);
    return 0;
}
