find_package (Boost REQUIRED)
find_package (Threads REQUIRED)
find_package (Folly REQUIRED)
find_package (Wangle REQUIRED)

find_path(
    TURKEY_INCLUDE_DIR Client.h Pool.h
    HINTS
        $ENV{TURKEY_HOME}/src
)

find_library(
    TURKEY_LIBRARY client pool
    HINTS
        $ENV{TURKEY_HOME}/build
)

set (TURKEY_LIBRARIES ${TURKEY_LIBRARY} ${WANGLE_LIBRARY} ${FOLLY_LIBRARIES})
set (TURKEY_INCLUDE_DIRS ${TURKEY_INCLUDE_DIR} ${WANGLE_INCLUDE_DIR} ${FOLLY_INCLUDEDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
TURKEY DEFAULT_MSG TURKEY_INCLUDE_DIR TURKEY_LIBRARIES)

mark_as_advanced(TURKEY_INCLUDE_DIR TURKEY_LIBRARIES TURKEY_FOUND)

if(TURKEY_FOUND AND NOT TURKEY_FIND_QUIETLY)
    message(STATUS "TURKEY: ${TURKEY_INCLUDE_DIR}")
endif()
