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

set(CMAKE_CXX_FLAGS "-Wall -Wpedantic -Wno-switch -std=c++20 -Wno-deprecated-copy -Wdeprecated-enum-enum-conversion -Wmisleading-indentation -Wc++20-extensions  -Wshadow -Wconversion")

add_executable(read read.cc print.cc blf_reader.cc)
target_link_libraries(read -lz )

add_executable(object_types object_types.cc print.cc blf_reader.cc)
target_link_libraries(object_types -lz )

add_executable(read_blf read_blf.cc print.cc blf_reader.cc)
target_link_libraries(read_blf -lz )

add_executable(read_can read_can.cc print.cc blf_reader.cc)
target_link_libraries(read_can -lz )

add_executable(read_filestatistics read_filestatistics.cc print.cc blf_reader.cc)
target_link_libraries(read_filestatistics -lz )
