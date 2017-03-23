cmake_minimum_required (VERSION 3.5)
project (turkey-utils)

macro(use_c99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endmacro(use_c99)

use_c99()

#Set compiler flags
set (CMAKE_BUILD_TYPE Debug)

# Set target name and source files
file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
add_library (${PROJECT_NAME} SHARED ${SOURCE_FILES})
