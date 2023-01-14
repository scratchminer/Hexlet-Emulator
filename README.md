# Hexlet

An emulator for the [Hexheld](https://github.com/Hexheld), a quirky fantasy console with a hexagonal LCD

## Advantages
- Hexlet is designed to be portable across a wide variety of systems. 
- Its statically linked driver system has a standard interface and file structure, so anyone can implement a driver in C.
- A sample driver using SDL2 is provided so it will build out of the box.

## Dependencies
- *The emulator itself* has no external dependencies, but the drivers are allowed to have as many as needed.
- *The sample driver* uses SDL2 as its only dependency, but feel free to replace it in `CMakeLists.txt`.

## Building
I used to provide a build script for each OS, but that was harder to maintain and less in the spirit of the project.
CMake is used because it works on all major operating systems and supports C the best.

From the command line:

- Install [CMake](https://cmake.org/)
- Create a build directory and `cd` to it
- `cmake ..`
- `make`

Then run `build/bin/hexlet`.