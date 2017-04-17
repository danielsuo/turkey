cmake_minimum_required (VERSION 3.5)
project (ferret-image CXX)

# Set global project finders
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

# Set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BUILD_TYPE Debug)

# Find packages
find_package (Threads)

# Set definitions
add_definitions (-DENABLE_THREADS)

# Include directories
include_directories (${TURKEY_INCLUDE_DIR})

file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

# Set target name and source files
add_library (${PROJECT_NAME} SHARED ${SOURCE_FILES})
