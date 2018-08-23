include_directories(
	${SOLUTION_INCLUDE_DIRS}
	/usr/local/include
)
link_directories(
	/usr/local/lib
)

add_executable(test_tcpsvr
	tcpsvr.h
	test_tcpsvr.cpp
)


target_link_libraries(test_tcpsvr scy_base scy_net)

