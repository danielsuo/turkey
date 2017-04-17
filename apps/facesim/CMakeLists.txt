# cmake_minimum_required (VERSION 3.5)
# project (facesim)
#
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -rdynamic -fpermissive -fexceptions -w")
#
# # Set compiler flags
# set (CMAKE_BUILD_TYPE Debug)
#
# add_definitions (-DENABLE_PTHREADS)
#
# add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/src/TaskQ)
#
# # Get all source files
# file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/Benchmarks/facesim/*)
# include_directories (${TASKQ_INCLUDE_DIR})
#
# # Set target name and source files
# add_executable (${PROJECT_NAME} ${SOURCE_FILES})
#
# find_package (Threads)
# target_link_libraries(
#     ${PROJECT_NAME}
#     taskq
#     physbam
#     m
#     ${CMAKE_THREAD_LIBS_INIT}
# )
