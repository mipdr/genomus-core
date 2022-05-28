add_subdirectory(testing_utils)

add_executable(run_tests run_tests.cpp)

target_include_directories(run_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../library
    testing_utils
)

target_link_libraries(run_tests LINK_PUBLIC 
    genomus-core 
    testing_utils 
)