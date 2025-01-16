# breakscore

![breakscore_logo](/imgstore/breakscore_logo.png)

Famicom functional simulator.

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