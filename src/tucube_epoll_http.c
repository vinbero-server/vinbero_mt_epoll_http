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
#include "tucube_epoll_http_Parser.h"

int tucube_tcp_epoll_module_init(struct tucube_module_args* module_args, struct tucube_module_list* module_list) {
    if(GONC_LIST_ELEMENT_NEXT(module_args) == NULL)
        errx(EXIT_FAILURE, "tucube_epoll_http requires another module");

    struct tucube_module* module = malloc(1 * sizeof(struct tucube_module));
    GONC_LIST_ELEMENT_INIT(module);
    module->pointer = malloc(1 * sizeof(struct tucube_epoll_http_Module));

    TUCUBE_MODULE_DLOPEN(module, module_args);

    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_init);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_tlinit);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_clinit);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestStart);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestMethod);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestUri);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestProtocol);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestScriptPath);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestContentType);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestContentLength);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onGetRequestContentLength);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestHeaderField);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestHeaderValue);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestHeadersFinish);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestBodyStart);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestBody);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestBodyFinish);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onRequestFinish);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseStatusCode);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseHeaderStart);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseHeader);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseBodyStart);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_onResponseBody);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_cldestroy);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_tldestroy);
    TUCUBE_MODULE_DLSYM(module, struct tucube_epoll_http_Module, tucube_epoll_http_Module_destroy);

    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->parserHeaderBufferCapacity =
              GONC_CAST(module->pointer,
                   struct tucube_epoll_http_Module*)->parserBodyBufferCapacity = 0;

    GONC_LIST_FOR_EACH(module_args, struct tucube_module_arg, module_arg) {
        if(strncmp("parser-header-buffer-capacity", module_arg->name, sizeof("parser-header-buffer-capacity")) == 0) {
            GONC_CAST(module->pointer,
                 struct tucube_epoll_http_Module*)->parserHeaderBufferCapacity = strtol(module_arg->value, NULL, 10);
        }
        else if(strncmp("parser-body-buffer-capacity", module_arg->name, sizeof("parser-body-buffer-capacity")) == 0) {
            GONC_CAST(module->pointer,
                 struct tucube_epoll_http_Module*)->parserBodyBufferCapacity = strtol(module_arg->value, NULL, 10);
        }
    }

    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->parserHeaderBufferCapacity == 0) {
        GONC_CAST(module->pointer,
             struct tucube_epoll_http_Module*)->parserHeaderBufferCapacity = 256;
    }

    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->parserBodyBufferCapacity == 0) {
        GONC_CAST(module->pointer,
             struct tucube_epoll_http_Module*)->parserBodyBufferCapacity = 1024;
    }

    GONC_LIST_APPEND(module_list, module);

    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_init(GONC_LIST_ELEMENT_NEXT(module_args), module_list) == -1)
        errx(EXIT_FAILURE, "%s: %u: tucube_epoll_http_Module_init() failed", __FILE__, __LINE__);

    return 0;
}

int tucube_tcp_epoll_module_tlinit(struct tucube_module* module, struct tucube_module_args* module_args) {
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_tlinit(GONC_LIST_ELEMENT_NEXT(module),
              GONC_LIST_ELEMENT_NEXT(module_args));
    return 0;
}

int tucube_tcp_epoll_module_clinit(struct tucube_module* module, struct tucube_cldata_list* cldata_list, int* clientSocket) {
    struct tucube_cldata* cldata = malloc(1 * sizeof(struct tucube_cldata));
    GONC_LIST_ELEMENT_INIT(cldata);
    cldata->pointer = malloc(1 * sizeof(struct tucube_epoll_http_ClientLocalData));

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->clientSocket = clientSocket;
    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser = calloc(1, sizeof(struct tucube_epoll_http_Parser));

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->headerBufferCapacity =
              GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->parserHeaderBufferCapacity;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->bodyBufferCapacity =
              GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->parserBodyBufferCapacity;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->buffer =
              malloc(GONC_CAST(cldata->pointer,
                   struct tucube_epoll_http_ClientLocalData*)->parser->headerBufferCapacity * sizeof(char));

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestStart = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestStart;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestMethod = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestMethod;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestUri = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestUri;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestProtocol = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestProtocol;

    GONC_CAST(cldata->pointer,
            struct tucube_epoll_http_ClientLocalData*)->parser->onRequestScriptPath = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestScriptPath;

    GONC_CAST(cldata->pointer,
            struct tucube_epoll_http_ClientLocalData*)->parser->onRequestContentType = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestContentType;

    GONC_CAST(cldata->pointer,
            struct tucube_epoll_http_ClientLocalData*)->parser->onRequestContentLength = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestContentLength;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onGetRequestContentLength = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onGetRequestContentLength;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestHeaderField = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestHeaderField;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestHeaderValue = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestHeaderValue;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestHeadersFinish = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestHeadersFinish;

    GONC_CAST(cldata->pointer,
            struct tucube_epoll_http_ClientLocalData*)->parser->onRequestBodyStart = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBodyStart;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestBody = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBody;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestBodyFinish = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestBodyFinish;

    GONC_CAST(cldata->pointer,
         struct tucube_epoll_http_ClientLocalData*)->parser->onRequestFinish = GONC_CAST(module->pointer, struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onRequestFinish;

    GONC_LIST_APPEND(cldata_list, cldata);

    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_clinit(GONC_LIST_ELEMENT_NEXT(module),
              cldata_list, clientSocket);
    return 0;
}

static inline int tucube_epoll_http_read_request(struct tucube_module* module, struct tucube_cldata* cldata)
{
    ssize_t read_size;

    while((read_size = read(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket,
         tucube_epoll_http_Parser_getBufferPosition(GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->parser),
         tucube_epoll_http_Parser_getAvailableBufferSize(GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->parser))) > 0)
    {
        int result;
        if((result = tucube_epoll_http_Parser_parse(module, cldata, GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->parser, read_size)) <= 0)
        {
            if(GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->parser->state == TUCUBE_EPOLL_HTTP_PARSER_ERROR)
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

static inline int tucube_epoll_http_write_crlf(int clientSocket)
{
    return write(clientSocket, "\r\n", sizeof("\r\n") - 1);
}

static inline int tucube_epoll_http_write_status_code(int clientSocket, int status_code)
{
    write(clientSocket, "HTTP/1.1", sizeof("HTTP/1.1") - 1);
    write(clientSocket, " ", sizeof(" ") - 1);

    char* status_code_string;
    size_t status_code_string_length = gonc_ltostr(status_code, 10, &status_code_string);
    write(clientSocket, status_code_string, status_code_string_length);
    free(status_code_string);

    write(clientSocket, " ", sizeof(" ") - 1); // reason phrase is optional but blank space after status code is necessary

    tucube_epoll_http_write_crlf(clientSocket);
    return 0;
}

static inline int tucube_epoll_http_write_header(int clientSocket, const char* header_field, size_t header_field_size, const char* header_value, size_t header_value_size)
{
    write(clientSocket, header_field, header_field_size);
    write(clientSocket, ": ", sizeof(": ") - 1);
    write(clientSocket, header_value, header_value_size);
    tucube_epoll_http_write_crlf(clientSocket);
    return 0;
}

static inline int tucube_epoll_http_write_headers(struct tucube_module* module, struct tucube_cldata* cldata)
{
    int result;
    if((result = GONC_CAST(module->pointer,
             struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseHeaderStart(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata)) <= 0))
    {
        return result;
    }
    do
    {
        const char* header_field;
        size_t header_field_size;
        const char* header_value;
        size_t header_value_size;
        
        if((result = GONC_CAST(module->pointer,
             struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseHeader(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &header_field, &header_field_size, &header_value, &header_value_size)) == -1) {
            return -1;
        }
        tucube_epoll_http_write_header(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket, header_field, header_field_size, header_value, header_value_size);
    }
    while(result == 1);
    return 0;
}

static inline int tucube_epoll_http_write_body(struct tucube_module* module, struct tucube_cldata* cldata)
{
    int result;
    const char* body;
    size_t body_size;
    char* body_size_string;
    size_t body_size_string_size;
    if((result = GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseBodyStart(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata))) == -1)
    {
        return -1;
    }
    else if(result == 0)
    {
        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket);
        return 0;
    }

    if((result = GONC_CAST(module->pointer,
        struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseBody(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &body, &body_size)) == -1)
    {
        return -1;
    }
    else if(result == 0)
    {
        body_size_string_size = gonc_ltostr(body_size, 10, &body_size_string);
        tucube_epoll_http_write_header(*GONC_CAST(cldata->pointer,
             struct tucube_epoll_http_ClientLocalData*)->clientSocket,
                  "Content-Length", sizeof("Content-Length") - 1, body_size_string, body_size_string_size);
        free(body_size_string);
        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket);

        write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket, body, body_size);
    }
    else if(result == 1)
    {
        tucube_epoll_http_write_header(*GONC_CAST(cldata->pointer,
             struct tucube_epoll_http_ClientLocalData*)->clientSocket, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1, "chunked", sizeof("chunked"));

        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket);

        body_size_string_size = gonc_ltostr(body_size, 16, &body_size_string);
        write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket, body_size_string, body_size_string_size);
        free(body_size_string);
        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket);

        write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket, body, body_size);
        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket);

        do
        {
            if((result = GONC_CAST(module->pointer,
                 struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseBody(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &body, &body_size)) == -1)
            {
                return -1;
            }
            body_size_string_size = gonc_ltostr(body_size, 16, &body_size_string);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket, body_size_string, body_size_string_size);
            free(body_size_string);
            tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket);
            write(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket, body, body_size);
            tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket);
        }
        while(result == 1);
        tucube_epoll_http_write_crlf(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket);
    }
    return 0;
}

static inline int tucube_epoll_http_write_response(struct tucube_module* module, struct tucube_cldata* cldata)
{
    int status_code;
    if(GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_onResponseStatusCode(GONC_LIST_ELEMENT_NEXT(module), GONC_LIST_ELEMENT_NEXT(cldata), &status_code) == -1)
    {
        return -1;
    }

    tucube_epoll_http_write_status_code(*GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket, status_code);


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
    
    if(tucube_epoll_http_write_response(module, cldata) == -1)
        return -1;

    return 0;
}

int tucube_tcp_epoll_module_cldestroy(struct tucube_module* module, struct tucube_cldata* cldata)
{
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_cldestroy(GONC_LIST_ELEMENT_NEXT(module),
              GONC_LIST_ELEMENT_NEXT(cldata));

    free(GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->parser->buffer);
    free(GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->parser);
    close(*(int*)GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket);
    *(int*)GONC_CAST(cldata->pointer, struct tucube_epoll_http_ClientLocalData*)->clientSocket = -1;
    free(cldata->pointer);
    free(cldata);

    return 0;
}

int tucube_tcp_epoll_module_tldestroy(struct tucube_module* module)
{
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_tldestroy(GONC_LIST_ELEMENT_NEXT(module));

    return 0;
}

int tucube_tcp_epoll_module_destroy(struct tucube_module* module)
{
    GONC_CAST(module->pointer,
         struct tucube_epoll_http_Module*)->tucube_epoll_http_Module_destroy(GONC_LIST_ELEMENT_NEXT(module));

//    dlclose(module->dl_handle);
    free(module->pointer);
    free(module);
    return 0;
}
