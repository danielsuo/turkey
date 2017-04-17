cmake_minimum_required (VERSION 2.8)
project (threads)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -rdynamic -fpermissive -w")

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DHAVE_LIBPTHREAD)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} * atomic/*)

# Set target name and source files
add_library (${PROJECT_NAME} ${SOURCE_FILES})

find_package (Threads)
target_link_libraries(
    ${PROJECT_NAME}
    m
    ${CMAKE_THREAD_LIBS_INIT}
)
