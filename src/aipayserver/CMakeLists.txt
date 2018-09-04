include_directories(
	${SOLUTION_INCLUDE_DIRS}
	/usr/local/include
)
link_directories(
	/usr/local/lib
)

add_executable(aipayserver
	aipayserver.h
	aipayserver.cpp
    utils.h
    utils.cpp
)

target_link_libraries(aipayserver pthread tcp_server jsoncpp)

