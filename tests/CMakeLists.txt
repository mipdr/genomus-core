add_subdirectory(testing_utils)

add_executable(parameter_mapping_test parameter_mapping_test.cpp)

target_include_directories(parameter_mapping_test PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../library
    testing_utils
)

target_link_libraries(parameter_mapping_test LINK_PUBLIC 
    genomus-core 
    testing_utils 
)


add_executable(dec_gen_lvl_functions dec_gen_lvl_functions.cpp)

target_include_directories(dec_gen_lvl_functions PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../library
    testing_utils
)

target_link_libraries(dec_gen_lvl_functions LINK_PUBLIC 
    genomus-core 
    testing_utils 
)