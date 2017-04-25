cmake_minimum_required (VERSION 3.5)
project (streamcluster CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

find_package (Threads)
find_package (TBB)

# add_definitions (-DENABLE_THREADS)
# add_definitions (-DTBB_VERSION)

# Get all source files
file (GLOB PTHREAD_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/pthread/*)
file (GLOB TURKEY_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/turkey/*)
file (GLOB TBB_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/TBB/*)

file (GLOB COMMON_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/common/*
  src/PStream/*
  src/main.cpp
)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)

# Set target name and source files
add_executable (${PROJECT_NAME}_pthread ${PTHREAD_SOURCE_FILES} ${COMMON_SOURCE_FILES})
set_target_properties (${PROJECT_NAME}_pthread PROPERTIES COMPILE_DEFINITIONS "ENABLE_THREADS")
target_link_libraries(
    ${PROJECT_NAME}_pthread
    m
    ${CMAKE_THREAD_LIBS_INIT}
)

add_executable (${PROJECT_NAME}_turkey ${TURKEY_SOURCE_FILES} ${COMMON_SOURCE_FILES})
set_target_properties (${PROJECT_NAME}_turkey PROPERTIES COMPILE_DEFINITIONS "ENABLE_THREADS")
target_include_directories (${PROJECT_NAME}_turkey PUBLIC ${TURKEY_INCLUDE_DIRS})
target_link_libraries(
    ${PROJECT_NAME}_turkey
    ${TURKEY_LIBRARIES}
    m
    ${CMAKE_THREAD_LIBS_INIT}
)

add_executable (${PROJECT_NAME}_tbb ${TBB_SOURCE_FILES} ${COMMON_SOURCE_FILES})
set_target_properties (${PROJECT_NAME}_tbb PROPERTIES COMPILE_DEFINITIONS "TBB_VERSION")
target_link_libraries(
    ${PROJECT_NAME}_tbb
    tbb
    m
    ${CMAKE_THREAD_LIBS_INIT}
)

add_dependencies (${PROJECT_NAME}_turkey turkey)
