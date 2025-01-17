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

#include "pch.h"

using namespace BaseLogic;

// TBD: We will simulate high level without gates, then we can make a decap of the original SHARP chip when we are in the mood.

namespace BaseBoard
{
	void LS161::sim(
		TriState CLK,
		TriState nRST,
		TriState nLD,
		TriState EN_T,
		TriState EN_P,
		TriState P[4],
		TriState& RCO,
		TriState Q[4])
	{
		if (nLD == TriState::Zero && CLK == TriState::Zero /*nROMSEL :P */)
		{
			val = PackNibble(P);
		}

		UnpackNibble(val, Q);

		// TBD: So far, crooked, some of the signals are not used at all because they are not needed
	}
}