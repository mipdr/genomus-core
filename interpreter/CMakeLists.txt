file(GLOB sources *.cpp)

add_executable(interpreter ${sources})

target_include_directories(run_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../library
)

target_link_libraries(interpreter LINK_PUBLIC 
    genomus-core
)