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

// Simple Mapper Simulator (2) - UNROM.

#include "pch.h"

using namespace BaseLogic;

namespace Mappers
{
	UNROM::UNROM(ConnectorType p1, uint8_t* nesImage, size_t nesImageSize) : AbstractCartridge(p1, nesImage, nesImageSize)
	{
		printf("UNROM::UNROM()\n");

		NESHeader* head = (NESHeader*)nesImage;

		bool trainer = (head->Flags_6 & 0b100) != 0;
		V_Mirroring = (head->Flags_6 & 1) != 0;

		// Load CHR ROM

		if (head->CHRSize != 0)
		{
			CHRSize = head->CHRSize * 0x2000;
			CHR = new uint8_t[CHRSize];

			uint8_t* chrPtr = nesImage + sizeof(NESHeader) + (trainer ? NES_TRAINER_SIZE : 0) + head->PRGSize * 0x4000;
			memcpy(CHR, chrPtr, CHRSize);
		}
		else
		{
			CHRSize = 0x2000;
			CHR = new uint8_t[CHRSize];
			memset(CHR, 0, CHRSize);
		}

		// Load PRG ROM

		PRGSize = head->PRGSize * 0x4000;
		PRG = new uint8_t[PRGSize];

		uint8_t* prgPtr = nesImage + sizeof(NESHeader) + (trainer ? NES_TRAINER_SIZE : 0);
		memcpy(PRG, prgPtr, PRGSize);

		valid = true;
	}

	UNROM::~UNROM()
	{
		printf("UNROM::~UNROM()\n");

		if (PRG != nullptr)
			delete PRG;

		if (CHR != nullptr)
			delete CHR;
	}

	bool UNROM::Valid()
	{
		return valid;
	}

	void UNROM::sim(
		TriState cart_in[(size_t)CartInput::Max],
		TriState cart_out[(size_t)CartOutput::Max],
		uint16_t cpu_addr,
		uint8_t* cpu_data, bool& cpu_data_dirty,
		uint16_t ppu_addr,
		uint8_t* ppu_data, bool& ppu_data_dirty,
		// Famicom only
		CartAudioOutSignal* snd_out,
		// NES only
		uint16_t* exp, bool& exp_dirty)
	{
		if (!valid)
			return;

		// PPU Part

		TriState nRD = cart_in[(size_t)CartInput::nRD];
		TriState nWR = cart_in[(size_t)CartInput::nWR];

		// H/V Mirroring
		cart_out[(size_t)CartOutput::VRAM_A10] = V_Mirroring ? FromByte((ppu_addr >> 10) & 1) : FromByte((ppu_addr >> 11) & 1);

		// Contains a jumper between `/PA13` and `/VRAM_CS`
		cart_out[(size_t)CartOutput::VRAM_nCS] = cart_in[(size_t)CartInput::nPA13];

		// CHR_A13 is actually `/CS` for CHR
		TriState nCHR_CS = FromByte((ppu_addr >> 13) & 1);

		if (nCHR_CS == TriState::Zero)
		{
			assert(ppu_addr < CHRSize);

			if (nRD == TriState::Zero)
			{
				uint8_t val = CHR[ppu_addr];

				if (!ppu_data_dirty)
				{
					*ppu_data = val;
					ppu_data_dirty = true;
				}
				else
				{
					*ppu_data = *ppu_data & val;
				}
			}

			if (nWR == TriState::Zero)
			{
				CHR[ppu_addr] = *ppu_data;
			}
		}

		// CPU Part

		TriState nROMSEL = cart_in[(size_t)CartInput::nROMSEL];
		TriState CPU_RnW = cart_in[(size_t)CartInput::RnW];

		// Counter to select PRG Bank

		TriState P[4]{};

		P[0] = FromByte(((*cpu_data) >> 0) & 1);
		P[1] = FromByte(((*cpu_data) >> 1) & 1);
		P[2] = FromByte(((*cpu_data) >> 2) & 1);
		P[3] = gnd;

		TriState RCO;		// unused
		TriState Q[4]{};

		counter.sim(nROMSEL, vdd, CPU_RnW, gnd, gnd, P, RCO, Q);

		// OR as MUX

		TriState A[4]{};
		TriState B[4]{};
		TriState Y[4]{};

		TriState A14 = FromByte((cpu_addr >> 14) & 1);

		A[0] = Q[1];
		A[1] = Q[0];
		A[2] = gnd;
		A[3] = A14;

		B[0] = A14;
		B[1] = A14;
		B[2] = gnd;
		B[3] = Q[2];

		quad_or.sim(A, B, Y);

		size_t prg_address = (cpu_addr & 0x3fff) |			// Clear original A14 (A15 is not routed actually)
			((size_t)ToByte(Y[1]) << 14) |		// A14 
			((size_t)ToByte(Y[0]) << 15) |		// A15
			((size_t)ToByte(Y[3]) << 16);		// A16

		if (nROMSEL == TriState::Zero)
		{
			uint8_t val = PRG[prg_address];

			if (!cpu_data_dirty)
			{
				*cpu_data = val;
				cpu_data_dirty = true;
			}
			else
			{
				*cpu_data = *cpu_data & val;
			}
		}

		TriState nIRQ = cart_out[(size_t)CartOutput::nIRQ];
		if (!(nIRQ == TriState::Zero || nIRQ == TriState::One))
		{
			cart_out[(size_t)CartOutput::nIRQ] = TriState::Z;
		}

		if (p1_type == ConnectorType::FamicomStyle && snd_out)
		{
			snd_out->normalized = 0.0f;
		}
	}
}