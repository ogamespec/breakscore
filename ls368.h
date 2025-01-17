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

// 40H368
// Hex bus buffer inverted 3-State output

#pragma once

namespace BaseBoard
{
	enum class LS368_Input
	{
		n_G1 = 0,
		n_G2,
		A1,
		A2,
		A3,
		A4,
		A5,
		A6,
		Max,
	};

	enum class LS368_Output
	{
		n_Y1 = 0,
		n_Y2,
		n_Y3,
		n_Y4,
		n_Y5,
		n_Y6,
		Max
	};

	class LS368
	{
	public:
		static void sim(BaseLogic::TriState inputs[], BaseLogic::TriState outputs[]);
	};
}