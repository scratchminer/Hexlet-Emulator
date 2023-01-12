# Hexlet

An emulator for the [Hexheld](https://github.com/Hexheld), a quirky fantasy console with a hexagonal LCD

## Features
(To be done later)

## Dependencies
The emulator itself has no external dependencies, but the drivers are allowed to have as many as needed.

The sample driver uses SDL2 for video and audio as its only depedency, but feel free to replace it in the `Makefile`!

## Building

### Windows
You're mostly on your own here, but I have provided a batch script (`makeHexlet.bat`) to run for the default driver:
- Edit `makeHexlet.bat` to provide the `include` and `lib` directories for the the MSVC version of SDL2.
- Use your favorite method to navigate to the Visual Studio Developer Command Prompt, and run `makeHexlet.bat`. (I hope it works!)

### macOS
Due to the fact that Apple's Clang version (seemingly aliased to `gcc`) only builds for x86_64, 
for an arm64 Mac (my case) you need to install GCC from Homebrew. I found that `gcc-12` works but `gcc` doesn't, so
I've set up a condition to check the machine's architecture in the Makefile.

- `brew install gcc@12`
- `make`

### Linux
No extra setup is required. Simply type `make` to build.

(Guess why I like Linux so much...)
