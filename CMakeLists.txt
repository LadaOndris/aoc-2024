cmake_minimum_required(VERSION 3.27)
project(aoc_2023)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

add_subdirectory(libs)

add_subdirectory(src)
add_subdirectory(tests)
