cmake_minimum_required (VERSION 3.5)
project (x264 C)

# Set global project finders
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)
set (ASM_OPTIONS "-x assembler-with-cpp")
set (CMAKE_ASM_FLAGS "${CFLAGS} ${ASM_OPTIONS}" )

# Set (CMAKE_C_STANDARD 99)
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BUILD_TYPE Debug)

# Find packages
find_package (Threads)
find_package (SDL)

# Set definitions
add_definitions (-DHAVE_PTHREAD)
# add_definitions (-DVISUALIZE)
add_definitions (-DHAVE_MMX)

add_definitions (-DARCH_X86_64)

# Include directories
include_directories (${TURKEY_INCLUDE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86)

# Set target name and source files
# file (GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/**/* src/*)

set (X264_ASM_FILES
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/cabac-a.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/dct-a.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/deblock-a.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/mc-a.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/mc-a2.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/pixel-a.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/predict-a.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/quant-a.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/sad-a.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/cpu-a.asm
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/dct-64.asm
)

set (X264_LIB_FILES
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/mc.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/mc-c.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/predict.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86/predict-c.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pixel.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/macroblock.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/frame.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/dct.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/cpu.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/cabac.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/mdate.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/set.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/quant.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/common/vlc.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder/analyse.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder/me.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder/ratecontrol.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder/set.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder/macroblock.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder/cabac.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder/cavlc.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/encoder/encoder.c
)

set (X264_CLI_FILES
     ${CMAKE_CURRENT_SOURCE_DIR}/src/x264.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/matroska.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/muxers.c
)

set (OBJ_FILES)
foreach (ASM_FILE ${X264_ASM_FILES})
  message (STATUS "Processing file: ${ASM_FILE}")

  # Generate output file name
  get_filename_component (OBJ_FILE ${ASM_FILE} NAME_WE)
  set (OBJ_FILE "${CMAKE_CURRENT_BINARY_DIR}/${OBJ_FILE}.o")
  message (STATUS "Output file: ${OBJ_FILE}")

  # Run each input file through yasm
  add_custom_command (
    OUTPUT ${OBJ_FILE}
    COMMAND yasm -f elf -m amd64 -DPIC -DARCH_X86_64 -I${CMAKE_CURRENT_SOURCE_DIR}/src/common/x86 -o ${OBJ_FILE} ${ASM_FILE}
    DEPENDS ${ASM_FILE}
    COMMENT "YASMing ${ASM_FILE}"
  )

  set (OBJ_FILES ${OBJ_FILES} ${OBJ_FILE})
endforeach (ASM_FILE)

add_custom_target (
  ${PROJECT_NAME}_configure_lib
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/src/configure --enable-pthread
  --enable-shared --prefix=${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target (
  ${PROJECT_NAME}_assemble ALL DEPENDS ${OBJ_FILES}
)

add_library (${PROJECT_NAME} ${X264_LIB_FILES} ${OBJ_FILES})
add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}_configure_lib ${PROJECT_NAME}_assemble)

add_executable (${PROJECT_NAME}_pthread ${X264_CLI_FILES})
target_link_libraries (
  ${PROJECT_NAME}_pthread
  ${PROJECT_NAME}
  m
  ${CMAKE_THREAD_LIBS_INIT}
)
