cmake_minimum_required (VERSION 3.5)
project (dummy)

# Set C++ 14
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

# Set target name and source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

add_executable (${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
    ${PROJECT_NAME}
    turkey-client
    turkey-common
    turkey-utils
    flatcc
    flatccrt
    czmq
    zmq
)
