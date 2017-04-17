cmake_minimum_required (VERSION 3.5)
project (blackscholes)

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

# Set target name and source files
add_executable (${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_executable (main_pool ${CMAKE_CURRENT_SOURCE_DIR}/main_pool.cpp)

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
