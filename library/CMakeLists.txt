set(LIBRARY_NAME genomus-core)

file(GLOB cpp_sources *.cpp)

add_library(${LIBRARY_NAME}
    ${cpp_sources}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/error_handling/error_handling.cpp
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/error_handling
)