#pragma once

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <random>
#include <algorithm>
#ifdef _WIN32
#include <intrin.h>
#endif
#include <cstddef>
#include <ctime>
#include <cstdarg>
#include <iostream>

#pragma warning(disable: 26812)		// warning C26812: The enum type 'BaseLogic::TriState' is unscoped. Prefer 'enum class' over 'enum' (Enum.3).

#define SDL_MAIN_HANDLED
#ifdef _WIN32
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#endif

#include "baselogic.h"
#include "sound.h"
#include "video.h"