cmake_minimum_required (VERSION 3.5)
project (swaptions CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

# Set compiler flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BUILD_TYPE Debug)

find_package (Threads)
# find_package (TBB)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/**)

# Set target name and source files
add_executable (
  ${PROJECT_NAME}_pthread
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pthread/HJM_Securities.cpp

  ${SOURCE_FILES}
  )
set_target_properties (${PROJECT_NAME}_pthread PROPERTIES COMPILE_DEFINITIONS "ENABLE_THREADS")
target_link_libraries(
  ${PROJECT_NAME}_pthread
  m
  ${CMAKE_THREAD_LIBS_INIT}
  )

# add_executable (${PROJECT_NAME}_turkey
  # ${CMAKE_CURRENT_SOURCE_DIR}/src/turkey/HJM_Securities.cpp
  # ${SOURCE_FILES}
  # )
# target_include_directories (${PROJECT_NAME}_turkey PUBLIC ${TURKEY_INCLUDE_DIRS})
# set_target_properties (${PROJECT_NAME}_turkey PROPERTIES COMPILE_DEFINITIONS "ENABLE_THREADS")
# target_link_libraries(
  # ${PROJECT_NAME}_turkey
  # ${TURKEY_LIBRARIES}
  # m
  # ${CMAKE_THREAD_LIBS_INIT}
  # )

# add_dependencies (${PROJECT_NAME}_turkey turkey)
