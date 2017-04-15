cmake_minimum_required (VERSION 3.5)
project (dedup)

set(CMAKE_C_STANDARD 99)

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DENABLE_PTHREADS)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/**)

# Set target name and source files
add_executable (${PROJECT_NAME} ${SOURCE_FILES})

find_package (Threads)
target_link_libraries(
    ${PROJECT_NAME}
    m
    ${CMAKE_THREAD_LIBS_INIT}
)