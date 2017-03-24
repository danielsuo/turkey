cmake_minimum_required (VERSION 3.5)
project (turkey)

# Set some flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -rdynamic")
set (CMAKE_BUILD_TYPE Debug)

# Add external libraries
add_subdirectory (lib/json)
include_directories (${JSON_INCLUDE_DIRS})

add_subdirectory (lib/zeromq)
include_directories (${ZEROMQ_INCLUDE_DIRS})
link_directories (${ZEROMQ_LINK_DIRS})

add_subdirectory (lib/czmq)
include_directories (${CZMQ_INCLUDE_DIRS})
link_directories (${CZMQ_LINK_DIRS})

add_subdirectory (lib/cppzmq)
include_directories (${CPPZMQ_INCLUDE_DIRS})

add_subdirectory (lib/flatbuffers)
include_directories (${FLATBUFFERS_INCLUDE_DIR})

# Macro for adding and including subdirectories
macro (add_sublib LIB)
  add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/src/${LIB})
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/${LIB})
endmacro (add_sublib)

# Add libraries
add_sublib (utils)
add_sublib (common)
add_sublib (client)
add_sublib (docker)
add_sublib (server)

# Get applications to build
set (APP_DIR apps)
set (MAKE "all" CACHE STRING "Application to build (default is all)")

# List applications
set (
  APPS
  dummy
  server
  straw
)

if (${MAKE} STREQUAL "all")
  foreach (APP ${APPS})
    add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP})
  endforeach ()
endif ()

add_executable (${PROJECT_NAME} src/main.cpp)
target_link_libraries(
  ${PROJECT_NAME}
  docker
  curl
)
