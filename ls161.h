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

// Presettable synchronous 4-bit binary counter; asynchronous reset

#pragma once

namespace BaseBoard
{
	class LS161
	{
		uint8_t val = 0;

	public:
		void sim(
			BaseLogic::TriState CLK,
			BaseLogic::TriState nRST,
			BaseLogic::TriState nLD,
			BaseLogic::TriState EN_T,
			BaseLogic::TriState EN_P,
			BaseLogic::TriState P[4],
			BaseLogic::TriState& RCO,
			BaseLogic::TriState Q[4]);
	};
}