cmake_minimum_required (VERSION 3.5)

include (ExternalProject)

ExternalProject_Add (json
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG "master"
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

set (JSON_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/json-prefix/src/json/src PARENT_SCOPE)
