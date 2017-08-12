add_library(tucube_epoll_http MODULE tucube_epoll_http.c)
set_target_properties(tucube_epoll_http PROPERTIES PREFIX "")
set_target_properties(tucube_epoll_http PROPERTIES PUBLIC_HEADER "tucube_IHttp.h")
target_link_libraries(tucube_epoll_http gon_http_parser)
install(TARGETS tucube_epoll_http
        LIBRARY DESTINATION lib/tucube
	PUBLIC_HEADER DESTINATION include/tucube)
