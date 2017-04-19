cmake_minimum_required (VERSION 3.5)
project (blackscholes CXX)

# Set global project finders
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

# Set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BUILD_TYPE Debug)

# Find packages
find_package (Threads)

# Include directories
include_directories (${TURKEY_INCLUDE_DIR})

# PTHREAD
add_executable (${PROJECT_NAME}_pthread ${CMAKE_CURRENT_SOURCE_DIR}/main_pthread.cpp)
target_compile_definitions (${PROJECT_NAME}_pthread PUBLIC -DENABLE_THREADS)
target_link_libraries(
    ${PROJECT_NAME}_pthread
    m
    ${CMAKE_THREAD_LIBS_INIT}
)

# TURKEY
add_executable (${PROJECT_NAME}_turkey ${CMAKE_CURRENT_SOURCE_DIR}/main_turkey.cpp)
target_link_libraries(
    ${PROJECT_NAME}_turkey
    ${TURKEY_LIBRARIES}
    m
    ${CMAKE_THREAD_LIBS_INIT}
)

add_dependencies (${PROJECT_NAME}_turkey turkey)

# Utility
add_executable (ascii2binary ${CMAKE_CURRENT_SOURCE_DIR}/ascii2binary.cpp)
