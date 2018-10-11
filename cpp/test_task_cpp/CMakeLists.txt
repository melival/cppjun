cmake_minimum_required(VERSION 3.10)

project(server)
set(TARGET server)

add_executable(${TARGET} sel_server.cpp)

target_sources(${TARGET} PRIVATE TZinfo.cpp)

target_link_libraries(${TARGET} PRIVATE pthread)

set_target_properties(${TARGET} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
	COMPILE_FLAGS -lpthread
)
