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

// LS139
// Dual 2-to-4 line decoder/demultiplexer

#pragma once

namespace BaseBoard
{
	class LS139
	{
	public:

		// You need to simulate the signals by the references, because some perverts feed the outputs from this chip back to the input

		static void sim(
			BaseLogic::TriState& n_EN1,
			BaseLogic::TriState& n_EN2,
			BaseLogic::TriState A1_0,
			BaseLogic::TriState A1_1,
			BaseLogic::TriState A2_0,
			BaseLogic::TriState A2_1,
			BaseLogic::TriState n_Y1[4],
			BaseLogic::TriState n_Y2[4]);
	};
}