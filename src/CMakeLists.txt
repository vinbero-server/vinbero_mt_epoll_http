find_package(fastdl REQUIRED)
find_package(gaio REQUIRED)
find_package(vinbero_com REQUIRED)

add_library(vinbero_mt_epoll_http MODULE)

target_sources(
    vinbero_mt_epoll_http
    PRIVATE
    vinbero_mt_epoll_http.c
)
target_compile_options(vinbero_mt_epoll_http PRIVATE -Wall)

set_target_properties(
    vinbero_mt_epoll_http PROPERTIES
    C_STANDARD 11
    PREFIX ""
    VERSION ${version}
    SOVERSION ${soversion}
)

target_include_directories(
    vinbero_mt_epoll_http
    PRIVATE
    ${PROJECT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/iface
)

target_link_libraries(
    vinbero_mt_epoll_http
    PRIVATE
    http_parser
    vinbero_com::vinbero_com
    fastdl::fastdl
    gaio::gaio
)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
    @ONLY
)

install(
    TARGETS vinbero_mt_epoll_http
    LIBRARY DESTINATION lib/vinbero
    PUBLIC_HEADER DESTINATION include/vinbero
)
