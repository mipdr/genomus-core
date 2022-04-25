cmake_minimum_required(VERSION 3.0.0)
project(genomus-core VERSION 0.1.0)

include(CTest)
enable_testing()

set(EXEC_NAME "main")

add_subdirectory(common)
add_subdirectory(genomus-core)
add_subdirectory(tests)
# add_executable(${EXEC_NAME} src/main.cpp)
# target_link_libraries(${EXEC_NAME} LINK_PUBLIC genomus-core)
# set_target_properties(${EXEC_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)