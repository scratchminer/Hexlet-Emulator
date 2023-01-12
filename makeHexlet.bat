@echo off

:: Alternative to the Hexlet Unix makefile for Windows systems
:: (requires at least the Visual Studio Build Tools)

:: Relevant directories
set BINDIR=bin
set SRCDIR=src
set INCDIR=include
set OBJDIR=obj
set DRVDIR=drivers\sdl2

::
:: NOTE: THIS WILL NOT WORK. CHANGE THE TWO LOCATIONS BELOW TO YOUR INSTALLATION OF SDL2.
::

:: Change this to the flags needed for the driver to compile/link
set CFLAGS_DRIVER=/I(SDL2 include directory)
set LDFLAGS_DRIVER=/libpath:(directory of SDL2.dll)

:: DO NOT EDIT BELOW THIS LINE

set CFLAGS=/I%INCDIR%\ /Za /W3 /utf-8 /nologo

for %%g in (%SRCDIR%\*.c) do cl /c %CFLAGS% /Fo%OBJDIR%\ %%g
for %%g in (%DRVDIR%\*.c) do cl /c %CFLAGS% %CFLAGS_DRIVER% /Fo%OBJDIR%\driver\ %%g

:: put together the list of object code files
set OBJFILES=
for /r %OBJDIR% %%g in (*.obj) do set OBJFILES=%OBJFILES% %%g
link %OBJFILES% /nologo /out:%BINDIR%\hexlet.exe