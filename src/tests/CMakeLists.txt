add_subdirectory(testing_utils)
set(EXEC_NAME parameter_mapping_test)

add_executable(${EXEC_NAME} parameter_mapping_test.cpp)

target_include_directories(${EXEC_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../genomus-core
    # ${CMAKE_CURRENT_SOURCE_DIR}/../common/error_handling
    testing_utils
)

target_link_libraries(${EXEC_NAME} LINK_PUBLIC 
    genomus-core 
    testing_utils 
    # error_handling
)
# set_target_properties(${EXEC_NAME} PROPERTIES LINKER_LANGUAGE CXX)
