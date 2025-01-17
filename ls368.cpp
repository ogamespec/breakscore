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

// 40H368 aka `notif0` x6
// Hex bus buffer inverted 3-State output; inverted tri-state enable

// https://github.com/emu-russia/breaks/blob/master/Docs/Famicom/40H368.pdf

#include "pch.h"

using namespace BaseLogic;

namespace BaseBoard
{
	void LS368::sim(TriState inputs[], TriState outputs[])
	{
		TriState G1 = NOT(inputs[(size_t)LS368_Input::n_G1]);
		TriState G2 = NOT(inputs[(size_t)LS368_Input::n_G2]);

		if (G1 == TriState::One)
		{
			outputs[(size_t)LS368_Output::n_Y1] = NOT(inputs[(size_t)LS368_Input::A1]);
			outputs[(size_t)LS368_Output::n_Y2] = NOT(inputs[(size_t)LS368_Input::A2]);
			outputs[(size_t)LS368_Output::n_Y3] = NOT(inputs[(size_t)LS368_Input::A3]);
			outputs[(size_t)LS368_Output::n_Y4] = NOT(inputs[(size_t)LS368_Input::A4]);
		}
		else
		{
			outputs[(size_t)LS368_Output::n_Y1] = TriState::Z;
			outputs[(size_t)LS368_Output::n_Y2] = TriState::Z;
			outputs[(size_t)LS368_Output::n_Y3] = TriState::Z;
			outputs[(size_t)LS368_Output::n_Y4] = TriState::Z;
		}

		if (G2 == TriState::One)
		{
			outputs[(size_t)LS368_Output::n_Y5] = NOT(inputs[(size_t)LS368_Input::A5]);
			outputs[(size_t)LS368_Output::n_Y6] = NOT(inputs[(size_t)LS368_Input::A6]);
		}
		else
		{
			outputs[(size_t)LS368_Output::n_Y5] = TriState::Z;
			outputs[(size_t)LS368_Output::n_Y6] = TriState::Z;
		}
	}
}