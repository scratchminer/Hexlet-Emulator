# CMake listfile for Hexlet's sample SDL2 driver

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
cmake_policy(SET CMP0079 NEW)

# We need SDL2 (of course)
find_package(SDL2 CONFIG REQUIRED)
include_directories(hexlet ${SDL2_INCLUDE_DIRS})

# Add the driver's code
target_sources(hexlet PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/main.c
	${CMAKE_CURRENT_LIST_DIR}/graphics_sdl2.c
	${CMAKE_CURRENT_LIST_DIR}/logger.c
)

# ...and link it with SDL2
target_link_libraries(hexlet ${SDL2_LIBRARIES})