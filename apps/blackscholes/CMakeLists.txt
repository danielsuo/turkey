cmake_minimum_required (VERSION 3.5)
project (blackscholes)

set(CMAKE_C_STANDARD 99)

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DENABLE_THREADS)

# TODO: refactor into something more general if we use m4 a lot
file (MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/out/main.c
  COMMAND /usr/bin/m4
    ${CMAKE_CURRENT_SOURCE_DIR}/pthreads.m4
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    > ${CMAKE_CURRENT_SOURCE_DIR}/out/main.c
  COMMENT "Applying pthreads.m4 to main.c"
)
add_custom_target (
  blackscholes-m4
  ALL DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/out/main.c
)

# Set target name and source files
add_executable (${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/out/main.c)

find_package (Threads)
target_link_libraries(
    ${PROJECT_NAME}
    m
    ${CMAKE_THREAD_LIBS_INIT}
)
