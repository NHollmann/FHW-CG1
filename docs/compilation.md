# Compilation instructions
All solutions are written in C99 and use OpenGL with GLUT for graphics.

## Linux
You can either use the cmake file or the makefile in each exercise directory. The makefile was only used on computers at the FH Wedel.

The cmake file is the recommended method of compilation. It was tested in Xubuntu. You may need to install required libraries by yourself.

This is the compilation process with cmake for exercise 1. Other exercises work accordingly.
```
cd ueb01
mkdir build
cd build
cmake ..
make
./ueb01
```

## macOS
**Warning: OpenGL is deprecated on macOS.**
On macOS you should only use cmake and never the makefile. For the shader exercise you need to install GLEW (for example with homebrew) to compile it.

This is the compilation process with cmake for exercise 1. Other exercises work accordingly.
```
cd ueb01
mkdir build
cd build
cmake ..
make
./ueb01
```

## Windows
Compilation in Windows was never tried. The cmake configuration should be able to compile it, but you must put the needed libraries in a lib directory by yourself.
