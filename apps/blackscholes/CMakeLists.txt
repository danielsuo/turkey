cmake_minimum_required (VERSION 3.5)
project (blackscholes)

set(CMAKE_C_STANDARD 99)

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DENABLE_THREADS)

# Set target name and source files
add_executable (${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

find_package (Threads)
target_link_libraries(
    ${PROJECT_NAME}
    m
    ${CMAKE_THREAD_LIBS_INIT}
)
