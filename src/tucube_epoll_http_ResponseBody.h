#ifndef _TUCUBE_EPOLL_HTTP_RESPONSE_BODY_H
#define _TUCUBE_EPOLL_HTTP_RESPONSE_BODY_H

enum tucube_epoll_http_ResponseBodyType {
    TUCUBE_EPOLL_HTTP_RESPONSE_BODY_STRING,
    TUCUBE_EPOLL_HTTP_RESPONSE_BODY_FILE,
};

struct tucube_epoll_http_ResponseBody {
    enum tucube_epoll_http_ResponseBodyType type;
    union {
        const char* chars;
        int fd;
    };
    size_t size;
};

#endif
