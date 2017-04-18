cmake_minimum_required (VERSION 3.5)

# Set some CMake variables
set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

# Some fugly hackery for CentOS
include (${CMAKE_MODULE_PATH}/SysInfo.cmake)
if (${LINUX_NAME} STREQUAL "CentOS")
  set (CMAKE_C_COMPILER "/opt/rh/devtoolset-6/root/usr/bin/gcc")
  set (CMAKE_CXX_COMPILER "/opt/rh/devtoolset-6/root/usr/bin/g++")
endif()

project (turkey)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Set some directories
set (APP_DIR apps)

# Set some configurable variables
set (MAKE "all" CACHE STRING "Application to build (default is all)")

# Find dem packages
find_package (Boost REQUIRED)
find_package (Threads REQUIRED)
find_package (Folly REQUIRED)
find_package (Wangle REQUIRED)

# Build vendored libraries
# add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/libpng)

# Set some convenience variables
set (TURKEY_LIB_DEPS folly wangle rt glog gflags ${CMAKE_THREAD_LIBS_INIT})
set (TURKEY_LIBRARIES ${TURKEY_LIB_DEPS} ${PROJECT_NAME})
set (TURKEY_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${FOLLY_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${WANGLE_INCLUDE_DIR}
)
set (APPS
  blackscholes
  bodytrack
  canneal
  dedup
  dummy
  facesim
  ferret
  fluidanimate
  freqmine
  raytrace
  streamcluster
  swaptions
  vips
  x264
)

# Include some directories
include_directories (${TURKEY_INCLUDE_DIR})

# Add external libraries
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/folly)

# Pool library
add_library (${PROJECT_NAME} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Pool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
)
target_link_libraries (${PROJECT_NAME} ${TURKEY_LIB_DEPS})

# Compiling client applications
if (${MAKE} STREQUAL "all")
  foreach (APP ${APPS})
    add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP})
  endforeach ()
else ()
  add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${MAKE})
endif ()

add_executable (
  ${PROJECT_NAME}_server

  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ProcReader.cpp
)

add_executable (
  ${PROJECT_NAME}_client

  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_client.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
)
target_link_libraries (${PROJECT_NAME}_server ${TURKEY_LIB_DEPS})
target_link_libraries (${PROJECT_NAME}_client ${TURKEY_LIB_DEPS})
