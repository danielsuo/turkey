cmake_minimum_required (VERSION 2.8)
project (physbam)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -rdynamic -fpermissive -fexceptions -w")

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DTASKQ_DIST_FIXED)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */*)

# Set target name and source files
add_library (${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
    ${PROJECT_NAME}
    m
)
