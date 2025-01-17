/*
 * breakscore - Famicom functional simulator.
 *
 * Copyright (C) 2024 org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
#include <string>
#include <list>

#pragma warning(disable: 26812)		// warning C26812: The enum type 'BaseLogic::TriState' is unscoped. Prefer 'enum class' over 'enum' (Enum.3).

#define SDL_MAIN_HANDLED
#ifdef _WIN32
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#endif

#include "baselogic.h"
#include "ls32.h"
#include "ls139.h"
#include "ls161.h"
#include "ls368.h"
#include "ls373.h"
#include "sram.h"
#include "6502.h"
#include "apu.h"
#include "ppu.h"
#include "cd4021.h"
#include "io.h"
#include "iodevice.h"
#include "famicontr.h"
#include "nes_header.h"
#include "cart.h"
#include "nrom.h"
#include "aorom.h"
#include "unrom.h"
#include "board.h"
#include "famicom.h"
#include "core.h"
#include "sound.h"
#include "video.h"