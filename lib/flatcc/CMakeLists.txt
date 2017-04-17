cmake_minimum_required (VERSION 2.8)

include (ExternalProject)

ExternalProject_Add (turkey-flatcc
    GIT_REPOSITORY "https://github.com/dvidelabs/flatcc.git"
    GIT_TAG "master"
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CMAKE_GENERATOR ""
    BUILD_COMMAND "make"
    INSTALL_COMMAND ""
)
#
set (FLATCC_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/turkey-flatcc-prefix/src/turkey-flatcc/include PARENT_SCOPE)
set (FLATCC_LINK_DIR ${CMAKE_CURRENT_BINARY_DIR}/turkey-flatcc-prefix/src/turkey-flatcc/lib PARENT_SCOPE)
set (FLATCC_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/turkey-flatcc-prefix/src/turkey-flatcc/bin/flatcc PARENT_SCOPE)
