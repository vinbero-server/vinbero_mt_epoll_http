add_library(vinbero_mt_epoll_http MODULE vinbero_mt_epoll_http.c)
set_target_properties(vinbero_mt_epoll_http PROPERTIES PREFIX "")
set_target_properties(vinbero_mt_epoll_http PROPERTIES
                                            VERSION ${vinbero_mt_epoll_http_version}
                                            SOVERSION ${vinbero_mt_epoll_http_soversion}
                                            PUBLIC_HEADER "vinbero_interface_HTTP.h")

find_library(fastdl libfastdl.so REQUIRED)
find_library(gaio libgaio.so.0.3 REQUIRED)
find_library(vinbero_common libvinbero_common.so REQUIRED)
target_link_libraries(vinbero_mt_epoll_http http_parser ${vinbero_common} ${fastdl} ${gaio})
install(TARGETS vinbero_mt_epoll_http
        LIBRARY DESTINATION lib/vinbero
        PUBLIC_HEADER DESTINATION include/vinbero)
