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

static const char* reason_phrases[600];

int tucube_tcp_epoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list)
{
reason_phrases[100] = "Continue";
reason_phrases[101] = "Switching Protocols";
reason_phrases[102] = "Processing";

reason_phrases[200] = "OK";
reason_phrases[201] = "Created";
reason_phrases[202] = "Accepted";
reason_phrases[203] = "Non-authoritative Information";
reason_phrases[204] = "No Content";
reason_phrases[205] = "Reset Content";
reason_phrases[206] = "Partial Content";
reason_phrases[207] = "Multi-Status";
reason_phrases[208] = "Already Reported";
reason_phrases[226] = "IM Used";

reason_phrases[300] = "Multiple Choices";
reason_phrases[301] = "Moved Permanently";
reason_phrases[302] = "Found";
reason_phrases[303] = "See Other";
reason_phrases[304] = "Not Modified";
reason_phrases[305] = "Use Proxy";
reason_phrases[307] = "Temporary Redirect";
reason_phrases[308] = "Permanent Redirect";

reason_phrases[400] = "Bad Request";
reason_phrases[401] = "Unauthorized";
reason_phrases[402] = "Payment Required";
reason_phrases[403] = "Forbidden";
reason_phrases[404] = "Not Found";
reason_phrases[405] = "Method Not Allowed";
reason_phrases[406] = "Not Acceptable";
reason_phrases[407] = "Proxy Authentication Required";
reason_phrases[408] = "Request Timeout";
reason_phrases[409] = "Conflict";
reason_phrases[410] = "Gone";
reason_phrases[411] = "Length Required";
reason_phrases[412] = "Precondition Failed";
reason_phrases[413] = "Payload Too Large";
reason_phrases[414] = "Request-URI Too Long";
reason_phrases[415] = "Unsupported Media Type";
reason_phrases[416] = "Requested Range Not Satisfiable";
reason_phrases[417] = "Expectation Failed";
reason_phrases[418] = "I'm a teapot";
reason_phrases[421] = "Misdirected Request";
reason_phrases[422] = "Unprocessable Entity";
reason_phrases[423] = "Locked";
reason_phrases[424] = "Failed Dependency";
reason_phrases[426] = "Upgrade Required";
reason_phrases[428] = "Precondition Required";
reason_phrases[429] = "Too Many Requests";
reason_phrases[431] = "Request Header Fields Too Large";
reason_phrases[444] = "Connection Closed Without Response";
reason_phrases[451] = "Unavailable For Legal Reasons";
reason_phrases[499] = "Client Closed Request";

reason_phrases[500] = "Internal Server Error";
reason_phrases[501] = "Not Implemented";
reason_phrases[502] = "Bad Gateway";
reason_phrases[503] = "Service Unavailable";
reason_phrases[504] = "Gateway Timeout";
reason_phrases[505] = "HTTP Version Not Supported";
reason_phrases[506] = "Variant Also Negotiates";
reason_phrases[507] = "Insufficient Storage";
reason_phrases[508] = "Loop Detected";
reason_phrases[510] = "Not Extended";
reason_phrases[511] = "Network Authentication Required";
reason_phrases[599] = "Network Connect Timeout Error";

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
         struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_header =
              dlsym(GONC_CAST(module->pointer,
                   struct tucube_epoll_http_module*)->dl_handle,
                        "tucube_epoll_http_module_get_header")) == NULL)
        errx(EXIT_FAILURE, "%s: %u: Unable to find tucube_epoll_http_module_get_header()", __FILE__, __LINE__);

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
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_cldata*)->client_socket, reason_phrases[status_code], strlen(reason_phrases[status_code]));
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
                 struct tucube_epoll_http_module*)->tucube_epoll_http_module_get_header(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &header_field, &header_field_size, &header_value, &header_value_size)) == -1)
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
