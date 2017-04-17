cmake_minimum_required (VERSION 2.8)

include (ExternalProject)

ExternalProject_Add (turkey-flatbuffers
    GIT_REPOSITORY "https://github.com/google/flatbuffers.git"
    GIT_TAG "master"
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND "make"
    INSTALL_COMMAND ""
)

set (FLATBUFFERS_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/turkey-flatbuffers-prefix/src/turkey-flatbuffers/include PARENT_SCOPE)
set (FLATBUFFERS_LINK_DIR ${CMAKE_CURRENT_BINARY_DIR}/turkey-flatbuffers-prefix/src/turkey-flatbuffers/include PARENT_SCOPE)
set (FLATBUFFERS_FLATC_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/turkey-flatbuffers-prefix/src/turkey-flatbuffers/flatc PARENT_SCOPE)
