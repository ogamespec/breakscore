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
#pragma once

namespace IO
{
	class CD4021
	{
		uint8_t dff = 0;
		BaseLogic::TriState prev_clk = BaseLogic::TriState::X;

	public:
		void sim(
			BaseLogic::TriState CLK,
			BaseLogic::TriState PAR_SER,
			BaseLogic::TriState SER_IN, uint8_t PAR_IN,
			BaseLogic::TriState& Q5, BaseLogic::TriState& Q6, BaseLogic::TriState& Q7);

		uint8_t get() { return dff; }
	};
}