file(GLOB sources *.cpp)

add_executable(run_tests ${sources})

target_include_directories(run_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../library
)

target_link_libraries(run_tests LINK_PUBLIC 
    genomus-core
)