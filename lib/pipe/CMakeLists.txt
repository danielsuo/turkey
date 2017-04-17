cmake_minimum_required (VERSION 2.8)

include (ExternalProject)

ExternalProject_Add (turkey-pipe
    GIT_REPOSITORY "https://github.com/cgaebel/pipe.git"
    GIT_TAG "master"
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

set (JSON_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/turkey-pipe-prefix/src/turkey-pipe/src PARENT_SCOPE)
