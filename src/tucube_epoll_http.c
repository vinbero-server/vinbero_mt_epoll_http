#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tucube/tucube_module.h>
#include <tucube/tucube_cast.h>
#include <libgonc/gonc_list.h>
#include "tucube_epoll_http.h"
#include "tucube_epoll_http_parser.h"
#include "../../tucube_tcp_epoll/src/tucube_tcp_epoll_cldata.h"

int tucube_tcp_epoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list)
{
    if(GONC_LIST_ELEMENT_NEXT(module_args) == NULL)
        errx(EXIT_FAILURE, "tucube_epoll_http requires another module");
    struct tucube_module* module = malloc(1 * sizeof(struct tucube_module));
    GONC_LIST_ELEMENT_INIT(module);
    module->pointer = malloc(1 * sizeof(struct tucube_epoll_http_module));
    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->dl_handle
              = dlopen(GONC_LIST_ELEMENT_NEXT(module_args)->module_path, RTLD_LAZY)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_init =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_init")) == NULL)
        errx(EXIT_FAILURE, "%s: %u, Unable to find tucube_epoll_http_module_init()", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tlinit =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_tlinit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_tlinit()", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_clinit =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_clinit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_clinit()", __FILE__, __LINE__);


    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_method =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_method")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_method()", __FILE__, __LINE__);
         
    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_uri =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_uri")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_uri", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_version =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_version")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_version()", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_field =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_header_field")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_header_field()", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_value =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_header_value")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_header_value()", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_service =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_service")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_module_service()", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_cldestroy =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_cldestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_cldestroy()", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tldestroy =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_tldestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_tldestroy()", __FILE__, __LINE__);

    if((TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_destroy =
              dlsym(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_destroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_destroy", __FILE__, __LINE__);


    TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->parser_header_buffer_capacity =
              TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->parser_body_buffer_capacity = 0;

    GONC_LIST_FOR_EACH(module_args, struct tucube_module_arg, module_arg)
    {
        if(strncmp("parser-header-buffer-capacity", module_arg->name, sizeof("parser-header-buffer-capacity") - 1) == 0)
        {
            TUCUBE_CAST(module->pointer, struct tucube_epoll_http_module*)->parser_header_buffer_capacity = strtol(module_arg->value, NULL, 10);
        }
        else if(strncmp("parser-body-buffer-capacity", module_arg->name, sizeof("parser-body-buffer-capacity") - 1) == 0)
        {
            TUCUBE_CAST(module->pointer, struct tucube_epoll_http_module*)->parser_body_buffer_capacity = strtol(module_arg->value, NULL, 10);
        }
    }

    if(TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->parser_header_buffer_capacity == 0)
    {
        TUCUBE_CAST(module->pointer,
             struct tucube_epoll_http_module*)->parser_header_buffer_capacity = 256;
    }

    if(TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->parser_body_buffer_capacity == 0)
    {
        TUCUBE_CAST(module->pointer,
             struct tucube_epoll_http_module*)->parser_body_buffer_capacity = 1024;
    }

    GONC_LIST_APPEND(module_list, module);

    if(TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_init(GONC_LIST_ELEMENT_NEXT(module_args), module_list) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_epoll_http_module_init() failed", __FILE__, __LINE__);

    return 0;
}

int tucube_tcp_epoll_module_tlinit(struct tucube_module* module, struct tucube_module_args* module_args)
{
    TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tlinit(GONC_LIST_ELEMENT_NEXT(module),
              GONC_LIST_ELEMENT_NEXT(module_args));
    return 0;
}

int tucube_tcp_epoll_module_clinit(struct tucube_module* module, struct tucube_tcp_epoll_cldata_list* cldata_list, int* client_socket)
{
    struct tucube_tcp_epoll_cldata* cldata = malloc(1 * sizeof(struct tucube_tcp_epoll_cldata));
    GONC_LIST_ELEMENT_INIT(cldata);
    cldata->pointer = malloc(1 * sizeof(struct tucube_epoll_http_cldata));

    TUCUBE_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->client_socket = client_socket;
    TUCUBE_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->parser = calloc(1, sizeof(struct tucube_epoll_http_parser));
    TUCUBE_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->parser->buffer =
              malloc(TUCUBE_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->parser_header_buffer_capacity * sizeof(char));

    GONC_LIST_APPEND(cldata_list, cldata);

    TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_clinit(GONC_LIST_ELEMENT_NEXT(module),
              cldata_list, client_socket);
    return 0;
}

int tucube_tcp_epoll_module_service(struct tucube_module* module, struct tucube_tcp_epoll_cldata* cldata)
{
    ssize_t read_size;
    while((read_size = read(*TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket,
         TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->buffer +
         TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token_size,
         TUCUBE_CAST(module->pointer, struct tucube_epoll_http_module*)->parser_header_buffer_capacity -
         TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token_size)) > 0)
    {
        if(tucube_epoll_http_parser_parse_message_header(module, cldata, TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser,
             TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token_size + read_size) <= 0)
        {
            if(TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->state == TUCUBE_EPOLL_HTTP_PARSER_ERROR)
                warnx("%s: %u: Parser error", __FILE__, __LINE__);
            break;
        }
        else
        {
            memmove(TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->buffer,
                 TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token,
                 TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token_size * sizeof(char));
        }
        if(TUCUBE_CAST(module->pointer,
             struct tucube_epoll_http_module*)->parser_header_buffer_capacity -
                  TUCUBE_CAST(cldata->pointer,
                       struct tucube_epoll_http_cldata*)->parser->token_size == 0)
        {
            read_size = -1;
            errno = EFAULT;
            break;
        }
    }
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
    else if(read_size == 0)
    {
        warnx("%s: %u: Client socket has been closed", __FILE__, __LINE__);
        return 0;
    }
    
    TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_service(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata));

    return 0;
}

int tucube_tcp_epoll_module_cldestroy(struct tucube_module* module, struct tucube_tcp_epoll_cldata* cldata)
{
    TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_cldestroy(GONC_LIST_ELEMENT_NEXT(module),
              GONC_LIST_ELEMENT_NEXT(cldata));

    free(TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->buffer);
    free(TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser);
    close(*(int*)TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);
    *(int*)TUCUBE_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket = -1;
    free(cldata->pointer);
    free(cldata);

    return 0;
}

int tucube_tcp_epoll_module_tldestroy(struct tucube_module* module)
{
    TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tldestroy(GONC_LIST_ELEMENT_NEXT(module));

    return 0;
}

int tucube_tcp_epoll_module_destroy(struct tucube_module* module)
{
    TUCUBE_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_destroy(GONC_LIST_ELEMENT_NEXT(module));

    dlclose(TUCUBE_CAST(module->pointer, struct tucube_epoll_http_module*)->dl_handle);
    free(module->pointer);
    free(module);
    return 0;
}
