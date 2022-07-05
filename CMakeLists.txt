cmake_minimum_required(VERSION 3.0.0)
project(genomus-core VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 20)

set (CMAKE_BUILD_TYPE Release)

add_subdirectory(library)
add_subdirectory(tests)
add_subdirectory(interpreter)