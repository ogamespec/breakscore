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

// Quad 2-Input OR Gate

#include "pch.h"

using namespace BaseLogic;

namespace BaseBoard
{
	void LS32::sim(
		TriState A[4],
		TriState B[4],
		TriState Y[4])
	{
		for (size_t n = 0; n < 4; n++)
		{
			Y[n] = OR(A[n], B[n]);
		}
	}
}