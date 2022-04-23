cmake_minimum_required(VERSION 3.0.0)
project(genomus-core VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory(parameter_mapping)
add_library(parameter_dictionary parameter_dictionary.cpp)
set_target_properties(parameter_dictionary PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(parameter_dictionary parameter_mapping)