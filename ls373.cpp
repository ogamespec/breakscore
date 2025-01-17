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

// LS373
// Octal D-type transparent latch; 3-state

// https://github.com/emu-russia/breaks/blob/master/Docs/Famicom/LR74HC373.pdf

#include "pch.h"

using namespace BaseLogic;

namespace BaseBoard
{
	void LS373::sim(TriState LE, TriState n_OE, uint8_t d, uint8_t* q, bool& qz)
	{
		if (LE == TriState::One)
		{
			val = d;
		}

		if (n_OE == TriState::Zero)
		{
			*q = val;
			qz = false;
		}
		else
		{
			qz = true;
		}
	}
}