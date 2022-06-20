set(LIBRARY_NAME genomus-core)

file(GLOB cpp_sources *.cpp)

add_library(${LIBRARY_NAME}
    ${cpp_sources}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
)