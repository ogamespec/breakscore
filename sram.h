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

// Generic SRAM

#pragma once

namespace BaseBoard
{
	class SRAM
	{
		size_t memSize = 0;
		uint8_t* mem = nullptr;
		bool do_trace = false;
		char sram_name[0x100]{};

	public:
		SRAM(const char* entity, size_t bits, bool trace = false);
		~SRAM();

		/// <summary>
		/// Simulate a typical SRAM chip from the 80s / 90s.
		/// </summary>
		/// <param name="n_CS">input: 0: Chip Select</param>
		/// <param name="n_WE">input: 0: Write Enable (Write Mode). Has a higher priority than `/OE` (see datasheet).</param>
		/// <param name="n_OE">input: 0: Output Enable (Read Mode)</param>
		/// <param name="addr">input: Address</param>
		/// <param name="data">inOut: Data Bus</param>
		/// <param name="dz">inOut: dz ("Data Z") determines whether the data bus to which the SRAM is attached is dirty or not. This is analogous to the z state of the bus (in Verilog terms). dz = true means that the bus is "floating".</param>
		void sim(BaseLogic::TriState n_CS, BaseLogic::TriState n_WE, BaseLogic::TriState n_OE, uint32_t* addr, uint8_t* data, bool& dz);
	};
}