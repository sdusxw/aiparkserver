include_directories(
	${SOLUTION_INCLUDE_DIRS}
	/usr/local/include
)
link_directories(
	/usr/local/lib
)

add_library(tcp_server SHARED
	tcp_server.h
	tcp_server.cpp
)

add_executable(test_tcp
    test_tcp.cpp
)

target_link_libraries(tcp_server scy_base scy_net libuv jsoncpp)

target_link_libraries(test_tcp tcp_server)
