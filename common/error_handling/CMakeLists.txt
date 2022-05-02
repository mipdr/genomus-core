set(LIB_NAME error_handling)

add_library(${LIB_NAME} error_handling.cpp)
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)