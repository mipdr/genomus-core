cmake_minimum_required(VERSION 3.0.0)
project(genomus-core VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 20)

set (CMAKE_BUILD_TYPE Release)

# Following three lines will allow profiling and debugging

# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
# SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

add_subdirectory(library)
add_subdirectory(tests)
add_subdirectory(interpreter)
add_subdirectory(benchmark)