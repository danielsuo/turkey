cmake_minimum_required (VERSION 3.5)

include (ExternalProject)

ExternalProject_Add (flatbuffers
    GIT_REPOSITORY "https://github.com/google/flatbuffers.git"
    GIT_TAG "master"
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CMAKE_GENERATOR "Unix Makefiles"
    BUILD_COMMAND "make"
    INSTALL_COMMAND ""
)

set (FLATBUFFERS_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-prefix/src/flatbuffers/include PARENT_SCOPE)
set (FLATBUFFERS_LINK_DIR ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-prefix/src/flatbuffers/include PARENT_SCOPE)
set (FLATBUFFERS_FLATC_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-prefix/src/flatbuffers/flatc PARENT_SCOPE)
