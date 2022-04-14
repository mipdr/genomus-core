cmake_minimum_required(VERSION 3.0.0)
project(genomus-core VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB_RECURSE ParameterMappingSources *.cpp)
message(STATUS "Found following source files: ${ParameterMappingSources}")
add_library(parameter_mapping ${ParameterMappingSources})
set_target_properties(parameter_mapping PROPERTIES LINKER_LANGUAGE CXX)