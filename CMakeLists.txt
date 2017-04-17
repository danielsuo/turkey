cmake_minimum_required (VERSION 3.5)
project (turkey)

set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

find_package (Boost REQUIRED)
find_package (Threads REQUIRED)
find_package (Folly REQUIRED)
find_package (Wangle REQUIRED)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}/*
  ${FOLLY_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${WANGLE_INCLUDE_DIR}
)

set (COMMON_TURKEY_LIBS folly wangle rt ${CMAKE_THREAD_LIBS_INIT})

# Client library
add_library (client SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp)
target_link_libraries (client ${COMMON_TURKEY_LIBS})

# Pool library
add_library (pool SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/Pool.cpp)
target_link_libraries (pool ${COMMON_TURKEY_LIBS} client)

add_executable (
  main

  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ProcReader.cpp
)

add_executable (
  main_client

  ${CMAKE_CURRENT_SOURCE_DIR}/src/main_client.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
)
target_link_libraries (main ${COMMON_TURKEY_LIBS})
target_link_libraries (main_client ${COMMON_TURKEY_LIBS})
