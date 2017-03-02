cmake_minimum_required(VERSION 3.5)
project(parsec-server)

#Set C++ 14
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

# Set target name and source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/**)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Add json
set(JSON_DIR lib/json)
#add_subdirectory(${JSON_DIR})

# Add libdocker
set(LIBDOCKER_DIR lib/libdocker)
add_subdirectory(${LIBDOCKER_DIR})

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/${LIBDOCKER_DIR}/${DOCKER_INCLUDE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/${JSON_DIR}/src
)

target_link_libraries(
    ${PROJECT_NAME}
    docker
)
