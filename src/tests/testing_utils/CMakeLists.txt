include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../utils/error_handling)

set(LIB_NAME testing_utils)
add_library(${LIB_NAME} testing_utils.cpp)
target_link_libraries(${LIB_NAME} error_handling)