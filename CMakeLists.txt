# CMake listfile for Hexlet
# Based on the one for TIC-80 (https://github.com/nesbox/TIC-80/blob/main/CMakeLists.txt)

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(DRIVER_DIR ${CMAKE_SOURCE_DIR}/drivers)
set(BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set(HEXLET_VERSION_MAJOR "0")
set(HEXLET_VERSION_MINOR "0")
set(HEXLET_VERSION_BUILD "1")

set(DRIVER "sdl2")

# DO NOT EDIT BELOW THIS LINE

project(Hexlet VERSION "${HEXLET_VERSION_MAJOR}.${HEXLET_VERSION_MINOR}.${HEXLET_VERSION_BUILD}" LANGUAGES C)

message("Hexlet build target: ${CMAKE_SYSTEM_NAME}")
message("Hexlet version: ${PROJECT_VERSION}")

function(use_driver name)
	message("Attempting to find driver '${name}'...")
	if(EXISTS ${DRIVER_DIR}/${name}/CMakeLists.txt)
		# drivers with CMake required
		add_subdirectory(${DRIVER_DIR}/${name})
	else()
		# drivers with no external dependencies
		if(EXISTS ${DRIVER_DIR}/${name}/driver_${name}.c)
			set(DRIVER_SRCS ${DRIVER_DIR}/${name}/driver_${name}.c)
		elseif(EXISTS ${DRIVER_DIR}/driver_${name}.c)
			set(DRIVER_SRCS ${DRIVER_DIR}/driver_${name}.c)
		else()
			message(SEND_ERROR "Driver source not found: '${name}'")
			return()
		endif()
	endif()
	message("Using driver ${name}.")
endfunction()

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR})

# For the MSVC (Visual C) compiler
if(MSVC)
	# Skip Microsoft's dumb ol' deprecation warnings
	add_definitions("/D\"_CRT_SECURE_NO_WARNINGS\"")
	add_definitions("/D\"_CRT_NONSTDC_NO_DEPRECATE\"")
	
	# All the configurations should be in the binary folder
	foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${BINARY_DIR})
	endforeach()
endif()

set(INTERNAL_SRCS
	${SOURCE_DIR}/main.c
	${SOURCE_DIR}/assembler.c
#	${SOURCE_DIR}/disassembler.c
#	${SOURCE_DIR}/graphics.c
#	${SOURCE_DIR}/loader.c
#	${SOURCE_DIR}/pilot.c
	${SOURCE_DIR}/version.c
)

add_executable(hexlet ${INTERNAL_SRCS} ${DRIVER_SRCS})

# Include the Hexheld headers (external and internal)
target_include_directories(hexlet PUBLIC ${INCLUDE_DIR})
target_include_directories(hexlet PRIVATE ${SOURCE_DIR})

use_driver(${DRIVER})