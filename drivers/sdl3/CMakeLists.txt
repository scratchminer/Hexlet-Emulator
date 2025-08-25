# CMake listfile for Hexlet's sample SDL3 driver

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
cmake_policy(SET CMP0079 NEW)

# We need SDL3 (of course)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

# Add the driver's code
target_sources(hexlet PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/main.c
	${CMAKE_CURRENT_LIST_DIR}/graphics_sdl3.c
	${CMAKE_CURRENT_LIST_DIR}/logger.c
)

# ...and link it with SDL3
target_link_libraries(hexlet PRIVATE SDL3::SDL3)