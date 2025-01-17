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

// https://github.com/emu-russia/breaks/blob/master/Docs/Famicom/LR74HC139.pdf

#include "pch.h"

using namespace BaseLogic;

namespace BaseBoard
{
	void LS139::sim(
		TriState& n_EN1,
		TriState& n_EN2,
		TriState A1_0,
		TriState A1_1,
		TriState A2_0,
		TriState A2_1,
		TriState n_Y1[4],
		TriState n_Y2[4])
	{
		TriState EN1 = NOT(n_EN1);
		n_Y1[0] = NAND3(NOT(A1_0), NOT(A1_1), EN1);
		n_Y1[1] = NAND3(A1_0, NOT(A1_1), EN1);
		n_Y1[2] = NAND3(NOT(A1_0), A1_1, EN1);
		n_Y1[3] = NAND3(A1_0, A1_1, EN1);

		TriState EN2 = NOT(n_EN2);
		n_Y2[0] = NAND3(NOT(A2_0), NOT(A2_1), EN2);
		n_Y2[1] = NAND3(A2_0, NOT(A2_1), EN2);
		n_Y2[2] = NAND3(NOT(A2_0), A2_1, EN2);
		n_Y2[3] = NAND3(A2_0, A2_1, EN2);
	}
}