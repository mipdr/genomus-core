cmake_minimum_required(VERSION 3.0.0)
project(genomus-core VERSION 0.1.0)

include(CTest)
enable_testing()

set(LIB_NAME genomus-core)

add_subdirectory(parameter_dictionary)

add_library(${LIB_NAME} genomus-core.cpp)
target_link_libraries(${LIB_NAME} parameter_dictionary)
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)
