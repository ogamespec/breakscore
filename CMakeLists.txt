cmake_minimum_required (VERSION 3.0)
project (breakscore CXX)

add_definitions (-D_LINUX)

set(CMAKE_BUILD_TYPE Release)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

add_executable (breakscore 
	6502.cpp
	aorom.cpp
	apu.cpp
	baselogic.cpp
	board.cpp
	cart.cpp
	cd4021.cpp
	core.cpp
	famicom.cpp
	famicontr.cpp
	io.cpp
	ls32.cpp
	ls139.cpp
	ls161.cpp
	ls368.cpp
	ls373.cpp
	main.cpp
	nrom.cpp
	ppu.cpp
	sound.cpp
	sram.cpp
	unrom.cpp
	video.cpp
)

target_link_libraries (breakscore LINK_PUBLIC SDL2)
