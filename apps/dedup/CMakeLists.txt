cmake_minimum_required (VERSION 3.5)
project (dedup CXX)

# set(CMAKE_C_STANDARD 99)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -w")

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)
find_package (Threads)
find_package (ZLIB REQUIRED)

add_definitions (-DENABLE_PTHREADS)
add_definitions (-DENABLE_GZIP_COMPRESSION)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/**)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${ZLIB_INCLUDE_DIRS}
)

# Set target name and source files
add_executable (${PROJECT_NAME}_pthread
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pthread/dedup.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pthread/encoder.c
  ${SOURCE_FILES}
)
add_executable (${PROJECT_NAME}_turkey
  ${CMAKE_CURRENT_SOURCE_DIR}/src/turkey/dedup.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/turkey/encoder.cpp
  ${SOURCE_FILES}
)

target_link_libraries(
    ${PROJECT_NAME}_pthread
    m
    ${ZLIB_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
target_link_libraries(
    ${PROJECT_NAME}_turkey
    ${TURKEY_LIBRARIES}
    m
    ${ZLIB_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
target_include_directories (
  ${PROJECT_NAME}_turkey
  PUBLIC
  ${TURKEY_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIRS}
)
add_dependencies (${PROJECT_NAME}_turkey turkey)
