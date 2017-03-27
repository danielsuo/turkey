cmake_minimum_required (VERSION 3.5)
project (turkey-cgroup)

#Set C++ 14
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -rdynamic")

#Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

# Set target name and source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

add_library (${PROJECT_NAME} SHARED ${SOURCE_FILES})
