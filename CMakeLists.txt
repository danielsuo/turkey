cmake_minimum_required (VERSION 3.5)

# Set some CMake variables
set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

project (turkey)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Set some directories
set (APP_DIR apps)

# Set some configurable variables
set (MAKE "all" CACHE STRING "Application to build (default is all)")

# Find dem packages
find_package (Boost REQUIRED)
find_package (Threads REQUIRED)

# Build vendored libraries
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/folly)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/wangle)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/yasm)

# Set some convenience variables
set (TURKEY_DEPS ${FOLLY_EXTERNAL_PROJECT} ${WANGLE_EXTERNAL_PROJECT})
set (TURKEY_LIB_DEPS folly wangle rt glog gflags yasm ${CMAKE_THREAD_LIBS_INIT})
set (TURKEY_LIBRARIES ${TURKEY_LIB_DEPS} ${PROJECT_NAME})
set (TURKEY_HOME ${CMAKE_CURRENT_SOURCE_DIR})

link_directories (${FOLLY_LIBRARY_DIR} ${WANGLE_LIBRARY_DIR} ${YASM_LIBRARY_DIR})

set (TURKEY_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${FOLLY_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${WANGLE_INCLUDE_DIR}
  ${YASM_INCLUDE_DIR}
  )
set (APPS
  barnes
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

function (make_turkey)
  add_library (${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
    )

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

  add_dependencies (${PROJECT_NAME} ${TURKEY_DEPS})
  add_dependencies (${PROJECT_NAME}_server ${TURKEY_DEPS})
  add_dependencies (${PROJECT_NAME}_client ${TURKEY_DEPS})

  target_link_libraries (${PROJECT_NAME} ${TURKEY_LIB_DEPS})
  target_link_libraries (${PROJECT_NAME}_server ${TURKEY_LIB_DEPS})
  target_link_libraries (${PROJECT_NAME}_client ${TURKEY_LIB_DEPS} ${PROJECT_NAME})
endfunction(make_turkey)

make_turkey ()

# Compiling client applications
if (${MAKE} STREQUAL "all")
  foreach (APP ${APPS})
    add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP})
  endforeach ()
else ()
  if (${MAKE} IN_LIST APPS)
    add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${MAKE})
  endif()
endif ()
