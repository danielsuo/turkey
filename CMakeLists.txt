cmake_minimum_required (VERSION 3.5)

# TODO: We should have smarter file generation. Generated files are recreated even if nothing changes, which triggers a lot of recompilation.

# Set some CMake variables
set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

project (turkey)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Set some directories
set (APP_DIR apps)

# Set some configurable variables
set (MAKE "all" CACHE STRING "Application to build (default is all)")

# Find dem packages
find_package (Boost REQUIRED)
find_package (Threads REQUIRED)

# Build vendored libraries
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/folly)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/wangle)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/yasm)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/zmq)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/czmq)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/cppzmq)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/lib/flatbuffers)

# Set some convenience variables
set (TURKEY_DEPS
	${FOLLY_EXTERNAL_PROJECT}
	${WANGLE_EXTERNAL_PROJECT}
	${YASM_EXTERNAL_PROJECT}
	${ZMQ_EXTERNAL_PROJECT}
	${CZMQ_EXTERNAL_PROJECT}
	${CPPZMQ_EXTERNAL_PROJECT}
	${FLATBUFFERS_EXTERNAL_PROJECT}
	)

set (TURKEY_LIB_DEPS folly wangle rt glog gflags yasm zmq ${CMAKE_THREAD_LIBS_INIT})
set (TURKEY_LIBRARIES ${TURKEY_LIB_DEPS} ${PROJECT_NAME})
set (TURKEY_HOME ${CMAKE_CURRENT_SOURCE_DIR})

link_directories (${FOLLY_LIBRARY_DIR} ${WANGLE_LIBRARY_DIR} ${YASM_LIBRARY_DIR} ${ZMQ_LIBRARY_DIR})

set (TURKEY_INCLUDE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${FOLLY_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${WANGLE_INCLUDE_DIR}
	${YASM_INCLUDE_DIR}
	${ZMQ_INCLUDE_DIRS}
	${CZMQ_INCLUDE_DIRS}
	${CPPZMQ_INCLUDE_DIRS}
	${FLATBUFFERS_INCLUDE_DIR}
	)
set (APPS
	barnes
	blackscholes
	bodytrack
	canneal
	dedup
	dummy
	facesim
	ferret
	fluidanimate
	freqmine
	raytrace
	streamcluster
	swaptions
	vips
	x264
	)

# Include some directories
include_directories (${TURKEY_INCLUDE_DIR})

function (make_turkey)

	# Compile flatbuffers
	file (GLOB FBS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/fbs/*.fbs)

	include (${CMAKE_MODULE_PATH}/BuildFlatBuffers.cmake)

  # Build C++ bindings
	build_flatbuffers (
		"${FBS_FILES}"
		""
    "GENERATED_CXX_FLATBUFFERS"
		"${FLATBUFFERS_EXTERNAL_PROJECT}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/fbs"
		""
		""
    "--cpp"
		)

  # Build Python bindings
  # TODO: Actually generating the cpp headers twice
	build_flatbuffers (
		"${FBS_FILES}"
		""
    "GENERATED_PYTHON_FLATBUFFERS"
		"${FLATBUFFERS_EXTERNAL_PROJECT}"
    "${CMAKE_CURRENT_SOURCE_DIR}/turkey/fbs"
		""
		""
    "--python"
		)

	# Build client library
	add_library (${PROJECT_NAME} SHARED
		${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Pool.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
		)

	# Build dummy server
	add_executable (
		${PROJECT_NAME}_server

		${CMAKE_CURRENT_SOURCE_DIR}/src/main_server.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/ProcReader.cpp
		)

	# Build dummy client
	add_executable (
		${PROJECT_NAME}_client

		${CMAKE_CURRENT_SOURCE_DIR}/src/main_client.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
		)

  add_dependencies (${PROJECT_NAME} ${TURKEY_DEPS} "GENERATED_CXX_FLATBUFFERS" "GENERATED_PYTHON_FLATBUFFERS")
	add_dependencies (${PROJECT_NAME}_server ${TURKEY_DEPS})
	add_dependencies (${PROJECT_NAME}_client ${TURKEY_DEPS})

	target_link_libraries (${PROJECT_NAME} ${TURKEY_LIB_DEPS})
	target_link_libraries (${PROJECT_NAME}_server ${TURKEY_LIB_DEPS})
	target_link_libraries (${PROJECT_NAME}_client ${TURKEY_LIB_DEPS} ${PROJECT_NAME})
endfunction(make_turkey)

make_turkey ()

# Compiling client applications
if (${MAKE} STREQUAL "all")
	foreach (APP ${APPS})
		add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP})
	endforeach ()
elseif (NOT ${MAKE} STREQUAL "turkey")
  message (${MAKE})
	if (${MAKE} IN_LIST APPS)
		add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${MAKE})
	endif()
endif ()
