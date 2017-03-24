cmake_minimum_required (VERSION 3.5)
cmake_minimum_required (VERSION 3.5)

include (ExternalProject)

ExternalProject_Add (cppzmq
    GIT_REPOSITORY "https://github.com/zeromq/cppzmq.git"
    GIT_TAG "master"
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

set (CPPZMQ_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/cppzmq-prefix/src/cppzmq/src PARENT_SCOPE)
