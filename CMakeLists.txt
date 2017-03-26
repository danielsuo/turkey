cmake_minimum_required (VERSION 3.5)
project (turkey)

# Set some flags
# TODO: Turn back on warnings and take out -fpermissive
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -rdynamic -fpermissive -w")
set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Add external libraries
add_subdirectory (lib/json)
include_directories (${JSON_INCLUDE_DIRS})

add_subdirectory (lib/zmq)
include_directories (${ZEROMQ_INCLUDE_DIRS})
link_directories (${ZEROMQ_LINK_DIRS})

add_subdirectory (lib/czmq)
include_directories (${CZMQ_INCLUDE_DIRS})
link_directories (${CZMQ_LINK_DIRS})

add_subdirectory (lib/cppzmq)
include_directories (${CPPZMQ_INCLUDE_DIRS})

add_subdirectory (lib/flatbuffers)
include_directories (${FLATBUFFERS_INCLUDE_DIR})
link_directories (${FLATBUFFERS_LINK_DIR})

add_subdirectory (lib/flatcc)
include_directories (${FLATCC_INCLUDE_DIR})
link_directories (${FLATCC_LINK_DIR})

# Generate flatbuffer stubs
file (GLOB FBS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/fbs/*.fbs)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stubs)

# See cmake/BuildFlatBuffers.cmake
include (${CMAKE_MODULE_PATH}/BuildFlatBuffers.cmake)
build_flatbuffers (
  "${FBS_FILES}"
  ""
  "GENERATED_FLATBUFFERS"
  "turkey-flatbuffers"
  "${CMAKE_CURRENT_BINARY_DIR}/stubs"
  ""
  ""
)

include (${CMAKE_MODULE_PATH}/BuildFlatCC.cmake)
build_flatcc (
  "${FBS_FILES}"
  "GENERATED_FLATCC"
  "turkey-flatcc"
  "${CMAKE_CURRENT_BINARY_DIR}/stubs"
)

# Add libraries
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/src/utils)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/src/common)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/src/fbs)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/src/client)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/src/docker)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/src/server)

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
  cgroup
  docker
  curl
)
