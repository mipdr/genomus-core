

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../genomus-core)
add_subdirectory(testing_utils)

set(EXEC_NAME parameter_mapping_test)

add_executable(${EXEC_NAME} parameter_mapping_test.cpp)

target_link_libraries(${EXEC_NAME} LINK_PUBLIC genomus-core testing_utils)
set_target_properties(${EXEC_NAME} PROPERTIES LINKER_LANGUAGE CXX)
