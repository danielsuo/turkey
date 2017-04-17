find_package (Boost REQUIRED)
find_package (Threads REQUIRED)
find_package (Folly REQUIRED)
find_package (Wangle REQUIRED)

find_package(OpenSSL REQUIRED)
find_package(Glog REQUIRED)
find_package(Gflags REQUIRED)
find_package(Libevent REQUIRED)
find_package(DoubleConversion REQUIRED)
find_package(Libdl)
find_package(Librt)

find_path(
    TURKEY_INCLUDE_DIR Client.h Pool.h
    HINTS
        $ENV{TURKEY_HOME}/src
)

find_library(
    TURKEY_LIBRARY pool
    HINTS
        $ENV{TURKEY_HOME}/build
)

set (TURKEY_LIBRARIES
  ${TURKEY_LIBRARY}
  ${WANGLE_LIBRARY}
  ${FOLLY_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${GLOG_LIBRARIES}
  ${GFLAGS_LIBRARIES}
  ${LIBEVENT_LIBRARIES}
  ${DOUBLE_CONVERSION_LIBRARIES}
  ${LIBDL_LIBRARIES}
  ${LIBRT_LIBRARIES}
)
set (TURKEY_INCLUDE_DIRS
  ${TURKEY_INCLUDE_DIR}
  ${WANGLE_INCLUDE_DIR}
  ${FOLLY_INCLUDEDIR}
  ${OPENSSL_INCLUDE_DIR}
  ${GLOG_INCLUDE_DIRS}
  ${GFLAGS_INCLUDE_DIRS}
  ${LIBEVENT_INCLUDE_DIRS}
  ${DOUBLE_CONVERSION_INCLUDE_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
TURKEY DEFAULT_MSG TURKEY_INCLUDE_DIR TURKEY_LIBRARIES)

mark_as_advanced(TURKEY_INCLUDE_DIR TURKEY_LIBRARIES TURKEY_FOUND)

if(TURKEY_FOUND AND NOT TURKEY_FIND_QUIETLY)
    message(STATUS "TURKEY: ${TURKEY_INCLUDE_DIR}")
endif()
