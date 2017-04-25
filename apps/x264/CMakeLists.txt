# cmake_minimum_required (VERSION 3.5)
# project (x264 CXX)
#
# # Set global project finders
# set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)
#
# # Set (CMAKE_C_STANDARD 99)
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
# set (CMAKE_BUILD_TYPE Debug)
#
# # Find packages
# find_package (Threads)
# find_package (SDL)
# # find_package
#
# # Set definitions
# add_definitions (-DHAVE_PTHREAD)
# add_definitions (-DVISUALIZE)
#
# # Include directories
# include_directories (${TURKEY_INCLUDE_DIR})
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)
#
# # Set target name and source files
# file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/**/* src/*)
#
# add_executable (${PROJECT_NAME} ${SOURCE_FILES})
#
# target_link_libraries(
#     ${PROJECT_NAME}
#     ${TURKEY_LIBRARIES}
#     ${SDL_LIBRARIES}
#     X11
#     m
#     ${CMAKE_THREAD_LIBS_INIT}
# )
#
# add_dependencies (${PROJECT_NAME} turkey)
