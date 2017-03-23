cmake_minimum_required (VERSION 3.5)
project (turkey)

# Set some flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BUILD_TYPE Debug)

# Add external libraries
set (JSON_DIR vendor/json)
include_directories (${JSON_DIR}/src)

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
  server
  straw
)

if (${MAKE} STREQUAL "all")
  foreach (APP ${APPS})
    add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP})
  endforeach ()
endif ()
