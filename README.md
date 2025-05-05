# breakscore

![breakscore_logo](/imgstore/breakscore_logo.png)

Famicom functional simulator, using decapped chip photos.

This repository is a "crystallisation" of the main repository [breaknes](https://github.com/emu-russia/breaknes), which contains a lot of obscure stuff. All functional chipset simulators were "amalgamated" into .cpp files (6502.cpp, apu.cpp, ppu.cpp), as a result the rest of the incomprehensible stuff is now all there.

## Progress

![progress](/imgstore/progress.png)

## Build for Windows

Use Windows and VS2022. Open vs2022/breakscore.sln and click the Build button with your left heel.

## Build for Linux

In general, the build process is typical for Linux. First you get all the sources from Git. Then you call CMake/make

```
# Get source
# Choose a suitable folder to store a clone of the repository, cd there and then
git clone https://github.com/ogamespec/breakscore.git
cd breakscore

# Preliminary squats
mkdir build
cd build
cmake ..
make

# Find the executable file in the depths of the build folder
./breakscore contra.nes
```

If something doesn't work, you do it. You have red eyes for a reason. :penguin:

## Build for NetBSD

In general, the build does not differ from the Linux build, but there are some minor peculiarities:
- You need to use `gmake` instead of `make`
- In CMakeLists.txt you need to add the `link_libraries(/usr/pkg/lib)` directive
- In the pch.h header, change the path to the SDL2.h header: `#include </usr/pkg/include/SDL2/SDL2.h>`