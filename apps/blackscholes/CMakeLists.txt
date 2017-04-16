cmake_minimum_required (VERSION 3.5)
project (blackscholes)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

# set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DENABLE_THREADS)

find_package(Turkey)

include_directories (${TURKEY_INCLUDE_DIRS})

# Set target name and source files
add_executable (${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_executable (main_pool ${CMAKE_CURRENT_SOURCE_DIR}/main_pool.cpp)

find_package (Threads)
target_link_libraries(
    ${PROJECT_NAME}
    ${TURKEY_LIBRARIES}
    m
    ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(
    main_pool
    ${TURKEY_LIBRARIES}
    m
    ${CMAKE_THREAD_LIBS_INIT}
)
