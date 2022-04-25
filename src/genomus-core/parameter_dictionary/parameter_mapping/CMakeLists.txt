set(LIB_NAME parameter_mapping)

add_library(${LIB_NAME} parameter_mapping.cpp)
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/error_handling
)