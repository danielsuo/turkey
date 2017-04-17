cmake_minimum_required (VERSION 3.5)

include (ExternalProject)

ExternalProject_Add (turkey-czmq
    GIT_REPOSITORY "https://github.com/zeromq/czmq.git"
    GIT_TAG "master"
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND libzmq_LIBS=${ZEROMQ_LINK_DIRS} ./autogen.sh && ./configure
    BUILD_COMMAND make
    INSTALL_COMMAND ""
)

set (CZMQ_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/turkey-czmq-prefix/src/turkey-czmq/include PARENT_SCOPE)
set (CZMQ_LINK_DIRS ${CMAKE_CURRENT_BINARY_DIR}/turkey-czmq-prefix/src/turkey-czmq/src/.libs PARENT_SCOPE)
