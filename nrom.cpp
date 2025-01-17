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

// Simple Mapper Simulator (0) - NROM.
// I'm not very knowledgeable about the zoo of their implementations, so this module will gradually be improved as new knowledge becomes available.

#include "pch.h"

using namespace BaseLogic;

namespace Mappers
{
	NROM::NROM(ConnectorType p1, uint8_t* nesImage, size_t nesImageSize) : AbstractCartridge(p1, nesImage, nesImageSize)
	{
		printf("NROM::NROM()\n");

		NESHeader* head = (NESHeader*)nesImage;

		bool trainer = (head->Flags_6 & 0b100) != 0;
		V_Mirroring = (head->Flags_6 & 1) != 0;
		IgnoreMirroring = (head->Flags_6 & 0b1000) != 0;

		// Load CHR

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
			chr_ram = true;
		}

		// Load PRG ROM

		PRGSize = head->PRGSize * 0x4000;
		PRG = new uint8_t[PRGSize];

		uint8_t* prgPtr = nesImage + sizeof(NESHeader) + (trainer ? NES_TRAINER_SIZE : 0);
		memcpy(PRG, prgPtr, PRGSize);

		valid = true;
	}

	NROM::~NROM()
	{
		printf("NROM::~NROM()\n");

		if (PRG != nullptr)
			delete PRG;

		if (CHR != nullptr)
			delete CHR;
	}

	bool NROM::Valid()
	{
		return valid;
	}

	void NROM::sim(
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

		// PPU_A13 is actually `/CS` for CHR-ROM
		TriState nCHR_CS = FromByte((ppu_addr >> 13) & 1);

		if (NOR(nRD, nCHR_CS) == TriState::One)
		{
			assert(ppu_addr < CHRSize);

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

		if (NOR(nWR, nCHR_CS) == TriState::One && chr_ram)
		{
			assert(ppu_addr < CHRSize);
			CHR[ppu_addr] = *ppu_data;
		}

		// CPU Part

		TriState nROMSEL = cart_in[(size_t)CartInput::nROMSEL];

		if (nROMSEL == TriState::Zero)
		{
			uint8_t val = PRG[cpu_addr & (PRGSize - 1)];

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