cmake_minimum_required(VERSION 3.0)
project(ufi_number)

set(CMAKE_BUILD_TYPE  Debug)
set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_executable(
	nufi
	src/main.cpp
	src/ufinumber.cpp
	src/ufiexception.cpp
	src/biginteger/src/biginteger.cpp
)

# target_link_libraries(
#	ufi_number avformat avcodec avutil
# )
