file(GLOB sources *.cpp)

add_executable(benchmark ${sources})

target_include_directories(benchmark PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../library
)

target_link_libraries(benchmark LINK_PUBLIC 
    genomus-core
)