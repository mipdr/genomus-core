set(LIBRARY_NAME genomus-core)

add_library(${LIBRARY_NAME}
    genomus-core.cpp 
    parameter_mapping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/error_handling/error_handling.cpp
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/error_handling
)