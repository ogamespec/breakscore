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

// CD4021 8-Stage Static Shift Register
#include "pch.h"

using namespace BaseLogic;

namespace IO
{
	void CD4021::sim(
		BaseLogic::TriState CLK,
		BaseLogic::TriState PAR_SER,
		BaseLogic::TriState SER_IN, uint8_t PAR_IN,
		BaseLogic::TriState& Q5, BaseLogic::TriState& Q6, BaseLogic::TriState& Q7)
	{
		// When the parallel input is active (P/S=1) - CLK does not matter. The input value is simply loaded into the shift register.
		// When serial input is active (P/S=0), the register value is shifted by posedge CLK and Serial in is written to the lsb.

		if (PAR_SER == TriState::One) {

			dff = PAR_IN;
		}
		else {

			if (IsPosedge(prev_clk, CLK)) {

				dff <<= 1;
				dff |= ToByte(SER_IN);
			}
		}

		Q5 = FromByte((dff >> 5) & 1);
		Q6 = FromByte((dff >> 6) & 1);
		Q7 = FromByte((dff >> 7) & 1);

		prev_clk = CLK;
	}
}