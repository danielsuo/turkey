cmake_minimum_required (VERSION 3.5)
project (vips CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -rdynamic -fpermissive -w")

# Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

add_definitions (-DHAVE_UNISTD_H)
add_definitions (-DHAVE_SYS_PARAM_H)
add_definitions (-DHAVE_CONFIG_H)
# # add_definitions (-DWITH_DMALLOC)
#
find_package (Threads REQUIRED)
find_package (Glib REQUIRED)
find_package (LibXml2)
find_package (Pango)
find_package (PNG)
find_package (ZLIB)
find_package (X11 REQUIRED)

# Get all source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/libvips/**/*)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/libvips/include)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/libvips/include)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/libvips)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/libvips)
include_directories (${CMAKE_CURRENT_BINARY_DIR})
include_directories (${Glib_INCLUDE_DIR})
include_directories (${GlibConfig_INCLUDE_DIR})
include_directories (${LIBXML2_INCLUDE_DIR})
include_directories (${PANGO_INCLUDE_DIRS})
include_directories (${ZLIB_INCLUDE_DIRS})
include_directories (${LIBPNG_INCLUDE_DIRS})

# Set target name and source files
add_executable (${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
    ${PROJECT_NAME}
    ${ZLIB_LIBRARIES}
    ${LIBPNG_LIBRARY}
    ${X11_LIBRARIES}
    glib-2.0
    gobject-2.0
    gio-2.0
    gmodule-2.0
    xml2
    m
    ${CMAKE_THREAD_LIBS_INIT}
)

add_custom_target (
  ${PROJECT_NAME}_configure
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/src/bootstrap.sh
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/src/configure --disable-shared --without-fftw3 --without-magick --without-liboil --without-lcms --without-OpenEXR --without-matio --without-pangoft2 --without-tiff --without-jpeg --without-zip --without-png --without-libexif --without-python --without-x --without-perl --without-v4l --without-cimg --enable-threads
)

add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}_configure)

#
# cmake_minimum_required (VERSION 3.5)
#
# include (ExternalProject)
#
# ExternalProject_Add (vip
#     SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/libvips
#     # BUILD_IN_SOURCE 1
#     UPDATE_COMMAND ""
#     CONFIGURE_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/src/configure --disable-shared --disable-cxx --without-fftw3 --without-magick --without-liboil --without-lcms --without-OpenEXR --without-matio --without-pangoft2 --without-tiff --without-jpeg --without-zip --without-png --without-libexif --without-python --without-x --without-perl --without-v4l --without-cimg --enable-threads"
#     BUILD_COMMAND make
#     INSTALL_COMMAND make install
# )
#
# set (CPPZMQ_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/turkey-cppzmq-prefix/src/turkey-cppzmq PARENT_SCOPE)
#
# add_dependencies (${PROJECT_NAME} turkey)
