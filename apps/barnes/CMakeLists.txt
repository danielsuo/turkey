cmake_minimum_required (VERSION 3.5)
project (barnes CXX)

# Set global project finders
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

Set (CMAKE_C_STANDARD 99)
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BUILD_TYPE Debug)

# Find packages
find_package (Threads)

# Set definitions
add_definitions (-DENABLE_THREADS)

# Include directories
include_directories (${TURKEY_INCLUDE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)

file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*)

# Set target name and source files
add_executable (${PROJECT_NAME}_pthread ${SOURCE_FILES})

target_link_libraries(
    ${PROJECT_NAME}_pthread
    ${TURKEY_LIBRARIES}
    m
    ${CMAKE_THREAD_LIBS_INIT}
)


