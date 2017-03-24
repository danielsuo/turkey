# Resources:
#  - http://zeromq.org/intro:get-the-software
#  - http://bit.ly/2dK0UBT
#
# Remember to have libtool, pkg-config, build-essential, autoconf, and automake
# installed.

cmake_minimum_required (VERSION 3.5)

include (ExternalProject)

set (CXX "clang++ -stdlib=c++")

ExternalProject_Add (turkey-zmq
    GIT_REPOSITORY "https://github.com/zeromq/libzmq.git"
    GIT_TAG "master"
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ./autogen.sh && ./configure
    BUILD_COMMAND make
    INSTALL_COMMAND sudo make install
)

set (ZEROMQ_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/src/turkey-zmq/include PARENT_SCOPE)
set (ZEROMQ_LINK_DIRS ${CMAKE_CURRENT_BINARY_DIR}/src/turkey-zmq/src/.libs PARENT_SCOPE)
