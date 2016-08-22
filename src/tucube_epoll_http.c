#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tucube/tucube_module.h>
#include <tucube/tucube_cldata.h>
#include <libgonc/gonc_cast.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_ltostr.h>
#include "tucube_epoll_http.h"
#include "tucube_epoll_http_parser.h"

int tucube_tcp_epoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list)
{
    if(GONC_LIST_ELEMENT_NEXT(module_args) == NULL)
        errx(EXIT_FAILURE, "tucube_epoll_http requires another module");

    struct tucube_module* module = malloc(1 * sizeof(struct tucube_module));
    GONC_LIST_ELEMENT_INIT(module);
    module->pointer = malloc(1 * sizeof(struct tucube_epoll_http_module));

    TUCUBE_MODULE_DLOPEN(module, module_args);

    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_init);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_tlinit);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_clinit);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_on_method);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_on_uri);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_on_version);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_on_header_field);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_on_header_value);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_service);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_get_status_code);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_prepare_get_header);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_get_header);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_prepare_get_body);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_get_body);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_cldestroy);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_tldestroy);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_module, tucube_epoll_http_module_destroy);

    GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->parser_header_buffer_capacity =
              GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->parser_body_buffer_capacity = 0;

    GONC_LIST_FOR_EACH(module_args, struct tucube_module_arg, module_arg)
    {
        if(strncmp("parser-header-buffer-capacity", module_arg->name, sizeof("parser-header-buffer-capacity")) == 0)
        {
            GONC_CAST(module->pointer,
                 struct tucube_epoll_http_module*)->parser_header_buffer_capacity = strtol(module_arg->value, NULL, 10);
        }
        else if(strncmp("parser-body-buffer-capacity", module_arg->name, sizeof("parser-body-buffer-capacity")) == 0)
        {
            GONC_CAST(module->pointer,
                 struct tucube_epoll_http_module*)->parser_body_buffer_capacity = strtol(module_arg->value, NULL, 10);
        }
    }

    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->parser_header_buffer_capacity == 0)
    {
        GONC_CAST(module->pointer,
             struct tucube_epoll_http_module*)->parser_header_buffer_capacity = 256;
    }

    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->parser_body_buffer_capacity == 0)
    {
        GONC_CAST(module->pointer,
             struct tucube_epoll_http_module*)->parser_body_buffer_capacity = 1024;
    }

    GONC_LIST_APPEND(module_list, module);

    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_init(GONC_LIST_ELEMENT_NEXT(module_args), module_list) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_epoll_http_module_init() failed", __FILE__, __LINE__);

    return 0;
}

int tucube_tcp_epoll_module_tlinit(struct tucube_module* module, struct tucube_module_args* module_args)
{
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tlinit(GONC_LIST_ELEMENT_NEXT(module),
              GONC_LIST_ELEMENT_NEXT(module_args));
    return 0;
}

int tucube_tcp_epoll_module_clinit(struct tucube_module* module, struct tucube_cldata_list* cldata_list, int* client_socket)
{
    struct tucube_cldata* cldata = malloc(1 * sizeof(struct tucube_cldata));
    GONC_LIST_ELEMENT_INIT(cldata);
    cldata->pointer = malloc(1 * sizeof(struct tucube_epoll_http_cldata));

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->client_socket = client_socket;
    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->parser = calloc(1, sizeof(struct tucube_epoll_http_parser));

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->parser->header_buffer_capacity =
              GONC_CAST(module->pointer, struct tucube_epoll_http_module*)->parser_header_buffer_capacity;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->parser->buffer =
              malloc(GONC_CAST(cldata->pointer,
                   struct tucube_epoll_http_cldata*)->parser->header_buffer_capacity * sizeof(char));

    GONC_LIST_APPEND(cldata_list, cldata);

    GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_clinit(GONC_LIST_ELEMENT_NEXT(module),
              cldata_list, client_socket);
    return 0;
}

static int tucube_epoll_http_read_request(struct tucube_module* module, struct tucube_cldata* cldata)
{
    ssize_t read_size;

    while((read_size = read(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket,
         tucube_epoll_http_parser_get_buffer_position(GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser),
         tucube_epoll_http_parser_get_buffer_left(GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser))) > 0)
    {
        if(tucube_epoll_http_parser_parse_message_header(module, cldata, GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser, read_size) <= 0)
        {
            if(GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->state == TUCUBE_EPOLL_HTTP_PARSER_ERROR)
            {
                warnx("%s: %u: Parser error", __FILE__, __LINE__);
                return -1;
            }
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
        else if (errno == EWOULDBLOCK)
        {
            warnx("%s: %u: Client socket EWOULDBLOCK", __FILE__, __LINE__);
            return 1;
        }
        else
            warn("%s: %u", __FILE__, __LINE__);
        return -1;
    }
    else if(read_size == 0)
    {
        warnx("%s: %u: Client socket has been closed", __FILE__, __LINE__);
        return -1;
    }

    return 0;
}

static int tucube_epoll_http_write_crlf(int client_socket)
{
    return write(client_socket, "\r\n", sizeof("\r\n") - 1);
}

static int tucube_epoll_http_write_status_code(struct tucube_module* module, struct tucube_cldata* cldata)
{
    int status_code;
    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_status_code(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &status_code) == -1)
    {
        return -1;
    }

    write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, "HTTP/1.1", sizeof("HTTP/1.1") - 1);
    write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, " ", sizeof(" ") - 1);

    char* status_code_string;
    size_t status_code_string_length = gonc_ltostr(status_code, 10, &status_code_string);
    write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, status_code_string, status_code_string_length);
    free(status_code_string);

    write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, " ", sizeof(" ") - 1); // reason phrase is optional but blank space after status code is necessary

    tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);
    return 0;
}

static int tucube_epoll_http_write_header(int client_socket, const char* header_field, size_t header_field_size, const char* header_value, size_t header_value_size)
{
    write(client_socket, header_field, header_field_size);
    write(client_socket, ": ", sizeof(": ") - 1);
    write(client_socket, header_value, header_value_size);
    tucube_epoll_http_write_crlf(client_socket);
}

static int tucube_epoll_http_write_headers(struct tucube_module* module, struct tucube_cldata* cldata)
{
    int result;
    do
    {
        if((result = GONC_CAST(module->pointer,
             struct tucube_epoll_http_module*)->tucube_epoll_http_module_prepare_get_header(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata)) == -1))
        {
            return -1;
        }

        const char* header_field;
        size_t header_field_size;
        const char* header_value;
        size_t header_value_size;
        
        if((result = GONC_CAST(module->pointer,
             struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_header(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &header_field, &header_field_size, &header_value, &header_value_size)) == -1)
        {
            return -1;
        }
        tucube_epoll_http_write_header(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, header_field, header_field_size, header_value, header_value_size);
    }
    while(result == 1);

    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_prepare_get_body(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata)) == -1)
    {
        return -1;
    }
    return 0;
}

static int tucube_epoll_http_write_body(struct tucube_module* module, struct tucube_cldata* cldata)
{
    int result;
    const char* body;
    size_t body_size;
    char* body_size_string;
    size_t body_size_string_size;

    if((result = GONC_CAST(module->pointer,
        struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_body(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &body, &body_size)) == -1)
    {
        return -1;
    }
    else if(result == 0)
    {
        body_size_string_size = gonc_ltostr(body_size, 10, &body_size_string);
        tucube_epoll_http_write_header(*GONC_CAST(cldata->pointer,
             struct tucube_epoll_http_cldata*)->client_socket,
                  "Content-Length", sizeof("Content-Length") - 1, body_size_string, body_size_string_size);
        free(body_size_string);
        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);

        write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, body, body_size);
    }
    else if(result == 1)
    {
        tucube_epoll_http_write_header(*GONC_CAST(cldata->pointer,
             struct tucube_epoll_http_cldata*)->client_socket, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1, "chunked", sizeof("chunked"));

        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);

        body_size_string_size = gonc_ltostr(body_size, 16, &body_size_string);
        write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, body_size_string, body_size_string_size);
        free(body_size_string);
        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);

        write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, body, body_size);
        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);

        do
        {
            if((result = GONC_CAST(module->pointer,
                 struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_body(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &body, &body_size)) == -1)
            {
                return -1;
            }
            body_size_string_size = gonc_ltostr(body_size, 16, &body_size_string);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, body_size_string, body_size_string_size);
            free(body_size_string);
            tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, body, body_size);
            tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);
        }
        while(result == 1);
        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);
    }
    return 0;
}

static int tucube_epoll_http_write_response(struct tucube_module* module, struct tucube_cldata* cldata)
{
    if(tucube_epoll_http_write_status_code(module, cldata) == -1)
        return -1;

    if(tucube_epoll_http_write_headers(module, cldata) == -1)
        return -1;

    if(tucube_epoll_http_write_body(module, cldata) == -1)
        return -1;

    return 0; 
}

int tucube_tcp_epoll_module_service(struct tucube_module* module, struct tucube_cldata* cldata)
{
    int result = tucube_epoll_http_read_request(module, cldata);
    if(result != 0)
        return result;
    
    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_service(GONC_LIST_ELEMENT_NEXT(module),
              GONC_LIST_ELEMENT_NEXT(cldata)) == -1)
    {
        return -1;
    }

    if(tucube_epoll_http_write_response(module, cldata) == -1)
        return -1;

    return 0;
}

int tucube_tcp_epoll_module_cldestroy(struct tucube_module* module, struct tucube_cldata* cldata)
{
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_cldestroy(GONC_LIST_ELEMENT_NEXT(module),
              GONC_LIST_ELEMENT_NEXT(cldata));

    free(GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->buffer);
    free(GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser);
    close(*(int*)GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket);
    *(int*)GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket = -1;
    free(cldata->pointer);
    free(cldata);

    return 0;
}

int tucube_tcp_epoll_module_tldestroy(struct tucube_module* module)
{
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tldestroy(GONC_LIST_ELEMENT_NEXT(module));

    return 0;
}

int tucube_tcp_epoll_module_destroy(struct tucube_module* module)
{
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_destroy(GONC_LIST_ELEMENT_NEXT(module));

//    dlclose(module->dl_handle);
    free(module->pointer);
    free(module);
    return 0;
}
