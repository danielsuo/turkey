cmake_minimum_required (VERSION 3.5)
project (bodytrack)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -rdynamic -fpermissive -w")

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DHAVE_LIBPTHREAD)
add_definitions (-DUSE_THREADS)
add_definitions (-DHAVE_STDINT_H)

add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/src/TrackingBenchmark/threads)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/**/* src/*)

# Remove TBB files
list (REMOVE_ITEM SOURCE_FILES src/TrackingBenchmark/TrackingModelTBB.cpp)
list (REMOVE_ITEM SOURCE_FILES src/TrackingBenchmark/TrackingModelTBB.h)
list (REMOVE_ITEM SOURCE_FILES src/TrackingBenchmark/ParticleFilterTBB.h)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/FlexImageLib)

# Set target name and source files
add_executable (${PROJECT_NAME}_pthread ${SOURCE_FILES})

find_package (Threads)
# find_package (TBB)
target_link_libraries(
  ${PROJECT_NAME}_pthread
  m
  # tbb
  threads
  ${CMAKE_THREAD_LIBS_INIT}
  )

# add_dependencies (${PROJECT_NAME}_turkey turkey)
