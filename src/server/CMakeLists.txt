cmake_minimum_required (VERSION 3.5)
project (turkey-server)

#Set C++ 14
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

# Set target name and source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

add_library (${PROJECT_NAME} SHARED ${SOURCE_FILES})

message (${PROJECT_NAME})

# target_link_libraries (
#   ${PROJET_NAME}
#   docker
#   turkey-common
#   turkey-utils
# )
