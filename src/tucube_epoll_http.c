#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tucube/tucube_module.h>
#include <libgonc/gonc_cast.h>
#include <libgonc/gonc_list.h>
#include <libgonc/gonc_ltostr.h>
#include "tucube_epoll_http.h"
#include "tucube_epoll_http_parser.h"
#include "../../tucube_tcp_epoll/src/tucube_tcp_epoll_cldata.h"

static const char* status_codes[600];

int tucube_tcp_epoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list)
{
status_codes[100] = "Continue";
status_codes[101] = "Switching Protocols";
status_codes[102] = "Processing";

status_codes[200] = "OK";
status_codes[201] = "Created";
status_codes[202] = "Accepted";
status_codes[203] = "Non-authoritative Information";
status_codes[204] = "No Content";
status_codes[205] = "Reset Content";
status_codes[206] = "Partial Content";
status_codes[207] = "Multi-Status";
status_codes[208] = "Already Reported";
status_codes[226] = "IM Used";

status_codes[300] = "Multiple Choices";
status_codes[301] = "Moved Permanently";
status_codes[302] = "Found";
status_codes[303] = "See Other";
status_codes[304] = "Not Modified";
status_codes[305] = "Use Proxy";
status_codes[307] = "Temporary Redirect";
status_codes[308] = "Permanent Redirect";

status_codes[400] = "Bad Request";
status_codes[401] = "Unauthorized";
status_codes[402] = "Payment Required";
status_codes[403] = "Forbidden";
status_codes[404] = "Not Found";
status_codes[405] = "Method Not Allowed";
status_codes[406] = "Not Acceptable";
status_codes[407] = "Proxy Authentication Required";
status_codes[408] = "Request Timeout";
status_codes[409] = "Conflict";
status_codes[410] = "Gone";
status_codes[411] = "Length Required";
status_codes[412] = "Precondition Failed";
status_codes[413] = "Payload Too Large";
status_codes[414] = "Request-URI Too Long";
status_codes[415] = "Unsupported Media Type";
status_codes[416] = "Requested Range Not Satisfiable";
status_codes[417] = "Expectation Failed";
status_codes[418] = "I'm a teapot";
status_codes[421] = "Misdirected Request";
status_codes[422] = "Unprocessable Entity";
status_codes[423] = "Locked";
status_codes[424] = "Failed Dependency";
status_codes[426] = "Upgrade Required";
status_codes[428] = "Precondition Required";
status_codes[429] = "Too Many Requests";
status_codes[431] = "Request Header Fields Too Large";
status_codes[444] = "Connection Closed Without Response";
status_codes[451] = "Unavailable For Legal Reasons";
status_codes[499] = "Client Closed Request";

status_codes[500] = "Internal Server Error";
status_codes[501] = "Not Implemented";
status_codes[502] = "Bad Gateway";
status_codes[503] = "Service Unavailable";
status_codes[504] = "Gateway Timeout";
status_codes[505] = "HTTP Version Not Supported";
status_codes[506] = "Variant Also Negotiates";
status_codes[507] = "Insufficient Storage";
status_codes[508] = "Loop Detected";
status_codes[510] = "Not Extended";
status_codes[511] = "Network Authentication Required";
status_codes[599] = "Network Connect Timeout Error";

    if(GONC_LIST_ELEMENT_NEXT(module_args) == NULL)
        errx(EXIT_FAILURE, "tucube_epoll_http requires another module");
    struct tucube_module* module = malloc(1 * sizeof(struct tucube_module));
    GONC_LIST_ELEMENT_INIT(module);
    module->pointer = malloc(1 * sizeof(struct tucube_epoll_http_module));
    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->dl_handle
              = dlopen(GONC_LIST_ELEMENT_NEXT(module_args)->module_path, RTLD_LAZY)) == NULL)
        err(EXIT_FAILURE, "%s: %u", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_init =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_init")) == NULL)
        errx(EXIT_FAILURE, "%s: %u, Unable to find tucube_epoll_http_module_init()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tlinit =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_tlinit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_tlinit()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_clinit =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_clinit")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_clinit()", __FILE__, __LINE__);


    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_method =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_method")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_method()", __FILE__, __LINE__);
         
    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_uri =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_uri")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_uri", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_version =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_version")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_version()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_field =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_header_field")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_header_field()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_on_header_value =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_on_header_value")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_on_header_value()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_service =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_service")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_module_service()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_status_code =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_get_status_code")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_get_status_code()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_next_header =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_get_next_header")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_get_next_header()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_body =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_get_body")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_get_body()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_cldestroy =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_cldestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_cldestroy()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_tldestroy =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_tldestroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_tldestroy()", __FILE__, __LINE__);

    if((GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_destroy =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_destroy")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_destroy", __FILE__, __LINE__);


    GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->parser_header_buffer_capacity =
              GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->parser_body_buffer_capacity = 0;

    GONC_LIST_FOR_EACH(module_args, struct tucube_module_arg, module_arg)
    {
        if(strncmp("parser-header-buffer-capacity", module_arg->name, sizeof("parser-header-buffer-capacity") - 1) == 0)
        {
            GONC_CAST(module->pointer, struct tucube_epoll_http_module*)->parser_header_buffer_capacity = strtol(module_arg->value, NULL, 10);
        }
        else if(strncmp("parser-body-buffer-capacity", module_arg->name, sizeof("parser-body-buffer-capacity") - 1) == 0)
        {
            GONC_CAST(module->pointer, struct tucube_epoll_http_module*)->parser_body_buffer_capacity = strtol(module_arg->value, NULL, 10);
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

int tucube_tcp_epoll_module_clinit(struct tucube_module* module, struct tucube_tcp_epoll_cldata_list* cldata_list, int* client_socket)
{
    struct tucube_tcp_epoll_cldata* cldata = malloc(1 * sizeof(struct tucube_tcp_epoll_cldata));
    GONC_LIST_ELEMENT_INIT(cldata);
    cldata->pointer = malloc(1 * sizeof(struct tucube_epoll_http_cldata));

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->client_socket = client_socket;
    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->parser = calloc(1, sizeof(struct tucube_epoll_http_parser));
    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_cldata*)->parser->buffer =
              malloc(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->parser_header_buffer_capacity * sizeof(char));

    GONC_LIST_APPEND(cldata_list, cldata);

    GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_clinit(GONC_LIST_ELEMENT_NEXT(module),
              cldata_list, client_socket);
    return 0;
}

int tucube_tcp_epoll_module_service(struct tucube_module* module, struct tucube_tcp_epoll_cldata* cldata)
{
    ssize_t read_size;
    while((read_size = read(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket,
         GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->buffer +
         GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token_size,
         GONC_CAST(module->pointer, struct tucube_epoll_http_module*)->parser_header_buffer_capacity -
         GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token_size)) > 0)
    {
        if(tucube_epoll_http_parser_parse_message_header(module, cldata, GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser,
             GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token_size + read_size) <= 0)
        {
            if(GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->state == TUCUBE_EPOLL_HTTP_PARSER_ERROR)
                warnx("%s: %u: Parser error", __FILE__, __LINE__);
            break;
        }
        else
        {
            memmove(GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->buffer,
                 GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token,
                 GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->parser->token_size * sizeof(char));
        }
        if(GONC_CAST(module->pointer,
             struct tucube_epoll_http_module*)->parser_header_buffer_capacity -
                  GONC_CAST(cldata->pointer,
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
    
    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_service(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata)) == 0)
    {
        int result;
        int status_code;
        if((result = GONC_CAST(module->pointer,
                 struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_status_code(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &status_code)) == 0)
        {
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, "HTTP/1.1", sizeof("HTTP/1.1") - 1);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, " ", sizeof(" ") - 1);
            char* status_code_string;
            size_t status_code_string_length = gonc_ltostr(status_code, 10, &status_code_string);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, status_code_string, status_code_string_length);
            free(status_code_string);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, " ", sizeof(" ") - 1);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, status_codes[status_code], strlen(status_codes[status_code]));
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, "\r\n", sizeof("\r\n") - 1);
        }
        else if(result == -1)
            return -1;

        const char* header_field;
        size_t header_field_size;
        const char* header_value;
        size_t header_value_size;
        const char* body;
        size_t body_size;
        do
        {
            if((result = GONC_CAST(module->pointer,
                 struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_next_header(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &header_field, &header_field_size, &header_value, &header_value_size)) == -1)
                return -1;
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, header_field, header_field_size);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, ": ", sizeof(": ") - 1);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, header_value, header_value_size);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, "\r\n", sizeof("\r\n") - 1);
        }
        while(result == 1);

        write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, "\r\n", sizeof("\r\n") - 1);

        do
        {
            if((result = GONC_CAST(module->pointer,
                 struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_body(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &body, &body_size)) == -1)
                return -1;
                write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, body, body_size);
                break;
        }
        while(result == 1);
    }

    return 0;
}

int tucube_tcp_epoll_module_cldestroy(struct tucube_module* module, struct tucube_tcp_epoll_cldata* cldata)
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

    dlclose(GONC_CAST(module->pointer, struct tucube_epoll_http_module*)->dl_handle);
    free(module->pointer);
    free(module);
    return 0;
}
