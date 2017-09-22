cmake_minimum_required (VERSION 3.5)
project (canneal)

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

find_package (Threads)

add_definitions (-DENABLE_THREADS)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/atomic/* src/**)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)

# Set target name and source files
add_executable (${PROJECT_NAME}_pthread
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pthread/main_pthread.cpp
  ${SOURCE_FILES}
)
# add_executable (${PROJECT_NAME}_turkey
  # ${CMAKE_CURRENT_SOURCE_DIR}/src/turkey/main_turkey.cpp
  # ${SOURCE_FILES}
# )

target_link_libraries(
    ${PROJECT_NAME}_pthread
    m
    ${CMAKE_THREAD_LIBS_INIT}
    # ${TURKEY_LIBRARIES}
)

# target_link_libraries(
    # ${PROJECT_NAME}_turkey
    # m
    # ${CMAKE_THREAD_LIBS_INIT}
    # ${TURKEY_LIBRARIES}
# )
# target_include_directories (${PROJECT_NAME}_turkey PUBLIC ${TURKEY_INCLUDE_DIRS})
# add_dependencies (${PROJECT_NAME}_turkey turkey)

# Utility
add_executable (${PROJECT_NAME}_ascii2binary ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/ascii2binary.cpp)
