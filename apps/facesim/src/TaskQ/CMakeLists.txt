cmake_minimum_required (VERSION 2.8)
project (taskq)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -rdynamic -fpermissive -fexceptions -w")

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DTASKQ_DIST_FIXED)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} lib/* include/*)
set (TASKQ_INCLUDE_DIR ${CMAKE_CURRENT_SORUCE_DIR}/include PARENT_SCOPE)

# Set target name and source files
add_library (${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
    ${PROJECT_NAME}
    m
)
