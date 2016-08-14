#include <dlfcn.h>
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
    if(GONC_LIST_ELEMENT_NEXT(module_args) == NULL)
        errx(EXIT_FAILURE, "tucube_epoll_http requires another module");
    struct tucube_module* module = malloc(sizeof(struct tucube_module));
    GONC_LIST_ELEMENT_INIT(module);
    module->object = malloc(sizeof(struct tucube_epoll_http_module));
    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->dl_handle
              = dlopen(GONC_LIST_ELEMENT_NEXT(module_args)->module_path.chars, RTLD_LAZY)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_init =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_init")) == NULL)
        errx(EXIT_FAILURE, "%s: %u, Unable to find tucube_epoll_http_module_init()", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tlinit =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_tlinit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_tlinit()", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_clinit =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_clinit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_clinit()", __FILE__, __LINE__);


    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_method =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_method")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_method()", __FILE__, __LINE__);
         
    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_uri =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_uri")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_uri", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_version =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_version")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_version()", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_field =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_header_field")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_header_field()", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_value =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_header_value")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_header_value()", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_service =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_service")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_module_service()", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_cldestroy =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_cldestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_cldestroy()", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tldestroy =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_tldestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_tldestroy()", __FILE__, __LINE__);

    if((TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_destroy =
              dlsym(TUCUBE_MODULE_CAST(module->object,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_destroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_destroy", __FILE__, __LINE__);

    GONC_LIST_APPEND(module_list, module);

    if(TUCUBE_MODULE_CAST(module->object,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_init(GONC_LIST_ELEMENT_NEXT(module_args), module_list) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_epoll_http_module_init() failed", __FILE__, __LINE__);

    return 0;
}

int tucube_tcp_epoll_module_tlinit(struct tucube_module* module, struct tucube_module_args* module_args)
{
    TUCUBE_MODULE_CAST(module->object,
        struct tucube_epoll_http_module*)->tucube_epoll_http_module_tlinit(GONC_LIST_ELEMENT_NEXT(module),
             GONC_LIST_ELEMENT_NEXT(module_args));
    return 0;
}

int tucube_tcp_epoll_module_clinit(struct tucube_module* module, struct tucube_tcp_epoll_cldata_list* cldata_list, int client_socket)
{
    struct tucube_tcp_epoll_cldata* cldata = malloc(sizeof(struct tucube_tcp_epoll_cldata));
    GONC_LIST_ELEMENT_INIT(cldata);
    cldata->data = malloc(sizeof(struct tucube_epoll_http_cldata));
    ((struct tucube_epoll_http_cldata*)cldata->data)->client_socket = client_socket;
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser = calloc(1, sizeof(struct tucube_epoll_http_parser));
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->buffer = malloc(256 * sizeof(char));
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->buffer_capacity = 256;
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->on_method =
         TUCUBE_MODULE_CAST(module->object, struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_method;
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->on_uri =
         TUCUBE_MODULE_CAST(module->object, struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_uri;
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->on_version =
         TUCUBE_MODULE_CAST(module->object, struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_version;
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->on_header_field =
         TUCUBE_MODULE_CAST(module->object, struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_field;
    ((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->on_header_value =
         TUCUBE_MODULE_CAST(module->object, struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_value;

    GONC_LIST_APPEND(cldata_list, cldata);

    TUCUBE_MODULE_CAST(module->object,
        struct tucube_epoll_http_module*)->tucube_epoll_http_module_clinit(GONC_LIST_ELEMENT_NEXT(module),
             cldata_list);

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
    warnx("read() loop finished");
    if(read_size == -1)
    {
        if(errno == EAGAIN)
        {
            warnx("%s: %u: Client socket EAGAIN", __FILE__, __LINE__);
            return 1;
        }
        else if(errno == EFAULT)
        {
            warnx("%s: %u: A token is bigger than http_buffer", __FILE__, __LINE__);
            return -1;
        }
        else
        {
            warn("%s: %u", __FILE__, __LINE__);
            return -1;
        }
    }

    return 0;
}

int tucube_tcp_epoll_module_cldestroy(struct tucube_module* module, struct tucube_tcp_epoll_cldata* cldata)
{
    TUCUBE_MODULE_CAST(module->object,
        struct tucube_epoll_http_module*)->tucube_epoll_http_module_cldestroy(GONC_LIST_ELEMENT_NEXT(module),
            GONC_LIST_ELEMENT_NEXT(cldata));

    free(((struct tucube_epoll_http_cldata*)cldata->data)->http_parser->buffer);
    free(((struct tucube_epoll_http_cldata*)cldata->data)->http_parser);
//    close(*((struct tucube_epoll_http_cldata*)cldata->data)->client_socket);
    free(cldata->data);
    free(cldata);

    return 0;
}

int tucube_tcp_epoll_module_tldestroy(struct tucube_module* module)
{
    TUCUBE_MODULE_CAST(module->object,
        struct tucube_epoll_http_module*)->tucube_epoll_http_module_tldestroy(GONC_LIST_ELEMENT_NEXT(module));

    return 0;
}

int tucube_tcp_epoll_module_destroy(struct tucube_module* module)
{
    TUCUBE_MODULE_CAST(module->object,
        struct tucube_epoll_http_module*)->tucube_epoll_http_module_destroy(GONC_LIST_ELEMENT_NEXT(module));

    dlclose(TUCUBE_MODULE_CAST(module->object, struct tucube_epoll_http_module*)->dl_handle);
    free(module->object);
    free(module);
    return 0;
}
