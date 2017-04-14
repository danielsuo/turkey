cmake_minimum_required (VERSION 3.5)
project (streamcluster)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DENABLE_THREADS)
# add_definitions (-DTBB_VERSION)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/**/* src/**)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)

# Set target name and source files
add_executable (${PROJECT_NAME} ${SOURCE_FILES})

find_package (Threads)
find_package (TBB)
target_link_libraries(
    ${PROJECT_NAME}
    m
    tbb
    ${CMAKE_THREAD_LIBS_INIT}
)
