add_library(vinbero_mt_epoll_http MODULE vinbero_mt_epoll_http.c)
set_target_properties(vinbero_mt_epoll_http PROPERTIES PREFIX "")
set_target_properties(vinbero_mt_epoll_http PROPERTIES PUBLIC_HEADER "vinbero_interface_HTTP.h")
target_link_libraries(vinbero_mt_epoll_http http_parser)
install(TARGETS vinbero_mt_epoll_http
        LIBRARY DESTINATION lib/vinbero
	PUBLIC_HEADER DESTINATION include/vinbero)
