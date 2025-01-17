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

#pragma once

namespace BaseBoard
{
	class LS373
	{
		uint8_t val = 0;

	public:

		/// <summary>
		/// Latch simulation.
		/// </summary>
		/// <param name="LE">1: Enable input</param>
		/// <param name="n_OE">0: Enable output</param>
		/// <param name="d">Input value for storage on the latch</param>
		/// <param name="q">Output value. Set only if n_OE = `0`.</param>
		/// <param name="qz">true: Output valid; false; Output has value `z` (disconnected)</param>
		void sim(BaseLogic::TriState LE, BaseLogic::TriState n_OE, uint8_t d, uint8_t* q, bool& qz);
	};
}