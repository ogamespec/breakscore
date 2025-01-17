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

// Generic SRAM

// https://github.com/emu-russia/breaks/blob/master/Docs/Famicom/HM6116_SRAM.pdf

#include "pch.h"

using namespace BaseLogic;

namespace BaseBoard
{
	SRAM::SRAM(const char* entity, size_t bits, bool trace)
	{
		memSize = 1LL << bits;
		mem = new uint8_t[memSize];
		memset(mem, 0, memSize);
		do_trace = trace;
		strcpy(sram_name, entity);
	}

	SRAM::~SRAM()
	{
		delete[] mem;
	}

	void SRAM::sim(TriState n_CS, TriState n_WE, TriState n_OE, uint32_t* addr, uint8_t* data, bool& dz)
	{
		if (n_CS == TriState::Zero)
		{
			if (n_WE == TriState::Zero && !dz)
			{
				if (do_trace) {
					printf("%s Write 0x%x = 0x%02x\n", sram_name, *addr, *data);
				}
				mem[*addr] = *data;
			}

			else if (n_OE == TriState::Zero && n_WE == TriState::One)
			{
				*data = mem[*addr];
				dz = false;
			}
		}
	}
}