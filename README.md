# Hexlet
A [Hexheld](https://github.com/Hexheld) emulator with a focus on portability and performance.

## Advantages
- Hexlet is designed to be portable across a wide variety of systems. 
- Its static driver system has a standard interface and file structure, so anyone can implement a driver in C.
- A sample driver using SDL3 is provided so it will work out of the box.

## Dependencies
- *The emulator itself* has no external dependencies, but the drivers are allowed to have as many as needed.
- *The sample driver* uses SDL3 as its only dependency, but feel free to add your own, then replace the `DRIVER` variable in `CMakeLists.txt`.

## Building
I used to provide a build script for each OS, but that was harder to maintain and less in the spirit of the project.
CMake is used because it works on all major operating systems and supports C the best.


Before building, install [SDL3](https://libsdl.org/) and [CMake](https://cmake.org/).

From the command line:
- Create a build directory and `cd` to it
- `cmake ..`
- `make`

Then run `./bin/hexlet`.
