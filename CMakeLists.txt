project(tucube_epoll_http)
set(CMAKE_BUILD_TYPE Debug)
add_library(tucube_epoll_http SHARED src/tucube_epoll_http.c)
set_target_properties(tucube_epoll_http PROPERTIES PREFIX "")
set_target_properties(tucube_epoll_http PROPERTIES PUBLIC_HEADER "src/tucube_IHttp.h")
target_link_libraries(tucube_epoll_http gon_http_parser)
install(TARGETS tucube_epoll_http DESTINATION /usr/local/lib/tucube/
	PUBLIC_HEADER DESTINATION /usr/local/include/tucube/)
