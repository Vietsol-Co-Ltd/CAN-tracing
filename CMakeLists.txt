cmake_minimum_required(VERSION 3.31)

project("lblf")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CXX_STANDARD 20)

find_package(ZLIB)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wno-switch -std=c++20")

add_executable(read read.cc print.cc blf_reader.cc)
target_link_libraries(read -lz )

add_executable(object_types object_types.cc print.cc blf_reader.cc)
target_link_libraries(object_types -lz )

add_executable(read_blf read_blf.cc print.cc blf_reader.cc)
target_link_libraries(read_blf -lz )
