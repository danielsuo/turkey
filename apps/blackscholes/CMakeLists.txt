cmake_minimum_required (VERSION 3.5)
project (blackscholes CXX)

# Set global project finders
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

# Set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BUILD_TYPE Debug)

# Find packages
find_package (Threads)
find_package (TBB)
find_package (OpenMP)

# PTHREAD
add_executable (${PROJECT_NAME}_pthread ${CMAKE_CURRENT_SOURCE_DIR}/main_pthread.cpp)
set_target_properties (${PROJECT_NAME}_pthread PROPERTIES COMPILE_DEFINITIONS "ENABLE_THREADS")
target_link_libraries(
  ${PROJECT_NAME}_pthread
  m
  ${CMAKE_THREAD_LIBS_INIT}
  )

# EQUI
add_executable (${PROJECT_NAME}_equi ${CMAKE_CURRENT_SOURCE_DIR}/main_equi.cpp)
set_target_properties (${PROJECT_NAME}_equi PROPERTIES COMPILE_DEFINITIONS "ENABLE_THREADS")
target_link_libraries(
  ${PROJECT_NAME}_equi
  m
  ${CMAKE_THREAD_LIBS_INIT}
  )

# TBB
add_executable (${PROJECT_NAME}_tbb ${CMAKE_CURRENT_SOURCE_DIR}/main_pthread.cpp)
set_target_properties (${PROJECT_NAME}_tbb PROPERTIES COMPILE_DEFINITIONS "ENABLE_TBB")
target_link_libraries(
  ${PROJECT_NAME}_tbb
  tbb
  m
  ${CMAKE_THREAD_LIBS_INIT}
  )

# OpenMP
add_executable (${PROJECT_NAME}_omp ${CMAKE_CURRENT_SOURCE_DIR}/main_pthread.cpp)
set_target_properties (${PROJECT_NAME}_omp PROPERTIES COMPILE_DEFINITIONS "ENABLE_OPENMP")
target_link_libraries(
  ${PROJECT_NAME}_omp
  gomp
  m
  ${CMAKE_THREAD_LIBS_INIT}
  )

# TURKEY
# add_executable (${PROJECT_NAME}_turkey ${CMAKE_CURRENT_SOURCE_DIR}/main_turkey.cpp)
# target_include_directories (${PROJECT_NAME}_turkey PUBLIC ${TURKEY_INCLUDE_DIRS})
# target_link_libraries(
# ${PROJECT_NAME}_turkey
# ${TURKEY_LIBRARIES}
# m
# ${CMAKE_THREAD_LIBS_INIT}
# )

# add_dependencies (${PROJECT_NAME}_turkey turkey)

# Utility
add_executable (${PROJECT_NAME}_ascii2binary ${CMAKE_CURRENT_SOURCE_DIR}/ascii2binary.cpp)
