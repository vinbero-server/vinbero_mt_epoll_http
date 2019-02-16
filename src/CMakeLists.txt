add_library(vinbero_mt_epoll_http MODULE)
target_sources(
    vinbero_mt_epoll_http
    PRIVATE
    vinbero_mt_epoll_http.c
)
target_compile_options(vinbero_mt_epoll_http PRIVATE -Wall)
set_target_properties(
    vinbero_mt_epoll_http
    PROPERTIES
    PREFIX ""
)
set_target_properties(
    vinbero_mt_epoll_http PROPERTIES
    VERSION ${version}
    SOVERSION ${soversion}
    C_STANDARD 11
)

find_library(fastdl libfastdl.so REQUIRED)
find_library(gaio libgaio.so REQUIRED)
find_package(vinbero_com REQUIRED)
target_link_libraries(
    vinbero_mt_epoll_http
    PRIVATE
    http_parser
    vinbero_com::vinbero_com
    ${fastdl}
    ${gaio}
)
install(
    TARGETS vinbero_mt_epoll_http
    LIBRARY DESTINATION lib/vinbero
    PUBLIC_HEADER DESTINATION include/vinbero
)
