cmake_minimum_required (VERSION 3.5)
project (blackscholes)

set(CMAKE_C_STANDARD 99)

#Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

# Set target name and source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

add_executable (${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
    ${PROJECT_NAME}
    m
    # turkey-client
    # turkey-common
    # turkey-utils
    # flatcc
    # flatccrt
    # czmq
    # zmq
)
