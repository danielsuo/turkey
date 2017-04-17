cmake_minimum_required (VERSION 3.5)
cmake_minimum_required (VERSION 3.5)

include (ExternalProject)

ExternalProject_Add (libpng
    GIT_REPOSITORY "https://github.com/glennrp/libpng"
    GIT_TAG "v1.2.57"
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND "${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng/configure"
    BUILD_COMMAND "make"
    INSTALL_COMMAND ""
)

set (LIBPNG_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/libpng-prefix/src/libpng PARENT_SCOPE)

file (GLOB LIBRARY_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} libpng-prefix/src/libpng/.libs/*)
set (LIBPNG_LIBRARY ${LIBRARY_FILES} PARENT_SCOPE)
