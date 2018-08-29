include_directories(
	${SOLUTION_INCLUDE_DIRS}
	/usr/local/include
)
link_directories(
	/usr/local/lib
)

add_executable(test_http
    test_http.cpp
)

target_link_libraries(test_http scy_base scy_net scy_http libuv)
