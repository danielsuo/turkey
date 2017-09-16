# Resources:
#  - http://zeromq.org/intro:get-the-software
#  - http://bit.ly/2dK0UBT
#
# Remember to have libtool, pkg-config, build-essential, autoconf, and automake
# installed.

cmake_minimum_required (VERSION 3.5)

include (ExternalProject)

# set (CXX "clang++ -stdlib=c++")
set (ZMQ_EXTERNAL_PROJECT zmq-git)
set (ZMQ_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${ZMQ_EXTERNAL_PROJECT}-prefix/src/${ZMQ_EXTERNAL_PROJECT})

ExternalProject_Add (${ZMQ_EXTERNAL_PROJECT}
    GIT_REPOSITORY "https://github.com/zeromq/libzmq.git"
    GIT_TAG "master"
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ./autogen.sh && ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
)

set (ZMQ_EXTERNAL_PROJECT ${ZMQ_EXTERNAL_PROJECT} PARENT_SCOPE)
set (ZMQ_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/src/${ZMQ_EXTERNAL_PROJECT}/include PARENT_SCOPE)
set (ZMQ_LINK_DIRS ${CMAKE_CURRENT_BINARY_DIR}/src/${ZMQ_EXTERNAL_PROJECT}/src/.libs PARENT_SCOPE)
set (ZMQ_LIBRARY_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}zmq${CMAKE_SHARED_LIBRARY_SUFFIX})
set (ZMQ_LIBRARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib PARENT_SCOPE)

add_library (zmq SHARED IMPORTED)
set_target_properties (zmq PROPERTIES IMPORTED_LOCATION ${ZMQ_LIBRARY_DIR}/${ZMQ_LIBRARY_NAME})

