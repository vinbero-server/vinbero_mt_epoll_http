add_library(vinbero_mt_epoll_http MODULE vinbero_mt_epoll_http.c)
set_target_properties(vinbero_mt_epoll_http PROPERTIES PREFIX "")
set_target_properties(vinbero_mt_epoll_http PROPERTIES
                                            VERSION ${version}
                                            SOVERSION ${soversion})

find_library(fastdl libfastdl.so REQUIRED)
find_library(gaio libgaio.so.0.3 REQUIRED)
find_library(vinbero_com libvinbero_com.so REQUIRED)
target_link_libraries(vinbero_mt_epoll_http http_parser ${vinbero_com} ${fastdl} ${gaio})
install(TARGETS vinbero_mt_epoll_http
        LIBRARY DESTINATION lib/vinbero
        PUBLIC_HEADER DESTINATION include/vinbero)
