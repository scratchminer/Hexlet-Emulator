# CMake listfile for Hexlet
# Based on the one for TIC-80 (https://github.com/nesbox/TIC-80/blob/main/CMakeLists.txt)

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(DRIVER_DIR ${CMAKE_SOURCE_DIR}/drivers)
set(BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# Use the default SDL2 driver
set(DRIVER "sdl2")

# DO NOT EDIT BELOW THIS LINE

project(Hexlet VERSION "0.0.1" LANGUAGES C)

message("Hexlet build target: ${CMAKE_SYSTEM_NAME}")
message("Hexlet version: ${PROJECT_VERSION}")

function(use_driver name)
	message("Attempting to find driver '${name}'...")
	if(EXISTS ${DRIVER_DIR}/${name}/CMakeLists.txt)
		# drivers with CMake required should pass the source files to target_sources() like this does
		add_subdirectory(${DRIVER_DIR}/${name})
	else()
		# drivers with no external dependencies
		if(EXISTS ${DRIVER_DIR}/${name}/main.c)
			target_sources(hexlet PRIVATE ${DRIVER_DIR}/${name}/main.c)
		elseif(EXISTS ${DRIVER_DIR}/driver_${name}.c)
			target_sources(hexlet PRIVATE ${DRIVER_DIR}/driver_${name}.c)
		else()
			message(FATAL_ERROR "Driver source not found: '${name}'")
			return()
		endif()
	endif()
	message("Using driver ${name}.")
endfunction()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR})

# For the MSVC (Visual C) compiler
if(MSVC)
	# Skip Microsoft's dumb ol' deprecation warnings
	add_definitions("/D\"_CRT_SECURE_NO_WARNINGS\"")
	add_definitions("/D\"_CRT_NONSTDC_NO_DEPRECATE\"")
	
	# For more than one build configuration, all of them should be in the binary folder
	foreach(OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${BINARY_DIR})
	endforeach()
endif()

add_executable(hexlet "")

# Add the core source files
target_sources(hexlet PRIVATE
	${SOURCE_DIR}/assembler.c
#	${SOURCE_DIR}/disassembler.c
#	${SOURCE_DIR}/graphics.c
	${SOURCE_DIR}/loader.c
#	${SOURCE_DIR}/memory.c
	${SOURCE_DIR}/pilot.c
	${SOURCE_DIR}/version.c
)

# Include the Hexlet headers
target_include_directories(hexlet PRIVATE ${INCLUDE_DIR})

# Do stuff for the driver (source, include, dependencies, etc.)
use_driver(${DRIVER})