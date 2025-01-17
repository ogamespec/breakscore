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

// It just so happens that not all methods are purely abstract.
#include "pch.h"

namespace Breaknes
{
	Board::Board(APUSim::Revision apu_rev, PPUSim::Revision ppu_rev, Mappers::ConnectorType p1)
	{
		p1_type = p1;
		pal = new RGB_Triplet[8 * 64];
	}

	Board::~Board()
	{
		delete pal;
	}

	int Board::InsertCartridge(uint8_t* nesImage, size_t nesImageSize)
	{
		Mappers::CartridgeFactory cf(p1_type, nesImage, nesImageSize);
		cart = cf.GetInstance();

		if (!cart)
			return -1;

		if (!cart->Valid())
		{
			delete cart;
			cart = nullptr;

			return -2;
		}

		return 0;
	}

	void Board::EjectCartridge()
	{
		if (cart)
		{
			delete cart;
			cart = nullptr;
		}
	}

	void Board::Reset()
	{
	}

	bool Board::InResetState()
	{
		return false;
	}

	size_t Board::GetACLKCounter()
	{
		return apu->GetACLKCounter();
	}

	size_t Board::GetPHICounter()
	{
		return apu->GetPHICounter();
	}

	void Board::SampleAudioSignal(float* sample)
	{
		// NES/Famicom motherboards have some analog circuitry that acts as a LPF/HPF. Now the sound from the simulator comes without filtering, so there may be some ear-unpleasant harmonics.
		// TBD: Add LPF/HPF

		if (sample != nullptr)
		{
			// There are 2 resistors (12k and 20k) on the motherboard that equalize the AUX A/B levels and then mix.
			// Although the internal resistance of the AUX A/B terminals inside the APU we counted with the 100 ohm pull-ups -- the above 2 resistors are part of the "Board", so they count here.

			*sample = (aux.normalized.a * 0.4f /* 20k resistor */ + aux.normalized.b /* 12k resistor */) / 2.0f;

			// This is essentially "muting" AUX A, because the level of AUX A at its peak is about 300 mV, and the level of AUX B at its peak is about 1100 mV.
			// Accordingly, if you do just (A+B)/2, the square channels will be "overshoot".
		}
	}

	void Board::GetApuSignalFeatures(APUSim::AudioSignalFeatures* features)
	{
		APUSim::AudioSignalFeatures feat{};
		apu->GetSignalFeatures(feat);
		*features = feat;
	}

	size_t Board::GetPCLKCounter()
	{
		return ppu->GetPCLKCounter();
	}

	void Board::SampleVideoSignal(PPUSim::VideoOutSignal* sample)
	{
		if (sample != nullptr)
		{
			*sample = vidSample;
		}
	}

	size_t Board::GetHCounter()
	{
		return ppu->GetHCounter();
	}

	size_t Board::GetVCounter()
	{
		return ppu->GetVCounter();
	}

	void Board::GetPpuSignalFeatures(PPUSim::VideoSignalFeatures* features)
	{
		PPUSim::VideoSignalFeatures feat{};
		ppu->GetSignalFeatures(feat);
		*features = feat;
	}

	void Board::ConvertRAWToRGB(uint16_t raw, uint8_t* r, uint8_t* g, uint8_t* b)
	{
		if (!pal_cached)
		{
			PPUSim::VideoOutSignal rawIn{}, rgbOut{};

			// 8 Emphasis bands, each with 64 colors.

			for (size_t n = 0; n < (8 * 64); n++)
			{
				rawIn.RAW.raw = (uint16_t)n;
				ppu->ConvertRAWToRGB(rawIn, rgbOut);
				pal[n].r = rgbOut.RGB.RED;
				pal[n].g = rgbOut.RGB.GREEN;
				pal[n].b = rgbOut.RGB.BLUE;
			}

			pal_cached = true;
		}

		size_t n = raw & 0b111'11'1111;

		*r = pal[n].r;
		*g = pal[n].g;
		*b = pal[n].b;
	}

	void Board::SetRAWColorMode(bool enable)
	{
		ppu->SetRAWOutput(enable);
	}

	void Board::SetOamDecayBehavior(PPUSim::OAMDecayBehavior behavior)
	{
		ppu->SetOamDecayBehavior(behavior);
	}

	void Board::SetNoiseLevel(float volts)
	{
		ppu->SetCompositeNoise(volts);
	}

	BoardFactory::BoardFactory(std::string board, std::string apu, std::string ppu, std::string p1)
	{
		board_name = board;

		// Perform a reflection for APU

		if (apu == "RP2A03G")
		{
			apu_rev = APUSim::Revision::RP2A03G;
		}
		else if (apu == "RP2A03H")
		{
			apu_rev = APUSim::Revision::RP2A03H;
		}
		else if (apu == "RP2A07")
		{
			apu_rev = APUSim::Revision::RP2A07;
		}
		else if (apu == "UA6527P")
		{
			apu_rev = APUSim::Revision::UA6527P;
		}
		else if (apu == "TA03NP1")
		{
			apu_rev = APUSim::Revision::TA03NP1;
		}
		else
		{
			board_name = "Bogus";
		}

		// Perform a reflection for PPU

		if (ppu == "RP2C02G")
		{
			ppu_rev = PPUSim::Revision::RP2C02G;
		}
		else if (ppu == "RP2C02H")
		{
			ppu_rev = PPUSim::Revision::RP2C02H;
		}
		else if (ppu == "RP2C03B")
		{
			ppu_rev = PPUSim::Revision::RP2C03B;
		}
		else if (ppu == "RP2C03C")
		{
			ppu_rev = PPUSim::Revision::RP2C03C;
		}
		else if (ppu == "RC2C03B")
		{
			ppu_rev = PPUSim::Revision::RC2C03B;
		}
		else if (ppu == "RC2C03C")
		{
			ppu_rev = PPUSim::Revision::RC2C03C;
		}
		else if (ppu == "RP2C04-0001")
		{
			ppu_rev = PPUSim::Revision::RP2C04_0001;
		}
		else if (ppu == "RP2C04-0002")
		{
			ppu_rev = PPUSim::Revision::RP2C04_0002;
		}
		else if (ppu == "RP2C04-0003")
		{
			ppu_rev = PPUSim::Revision::RP2C04_0003;
		}
		else if (ppu == "RP2C04-0004")
		{
			ppu_rev = PPUSim::Revision::RP2C04_0004;
		}
		else if (ppu == "RC2C05-01")
		{
			ppu_rev = PPUSim::Revision::RC2C05_01;
		}
		else if (ppu == "RC2C05-02")
		{
			ppu_rev = PPUSim::Revision::RC2C05_02;
		}
		else if (ppu == "RC2C05-03")
		{
			ppu_rev = PPUSim::Revision::RC2C05_03;
		}
		else if (ppu == "RC2C05-04")
		{
			ppu_rev = PPUSim::Revision::RC2C05_04;
		}
		else if (ppu == "RC2C05-99")
		{
			ppu_rev = PPUSim::Revision::RC2C05_99;
		}
		else if (ppu == "RP2C07-0")
		{
			ppu_rev = PPUSim::Revision::RP2C07_0;
		}
		else if (ppu == "UMC UA6538")
		{
			ppu_rev = PPUSim::Revision::UMC_UA6538;
		}
		else
		{
			board_name = "Bogus";
		}

		// Perform a reflection for cartridge slot

		if (p1 == "Fami")
		{
			p1_type = Mappers::ConnectorType::FamicomStyle;
		}
		else if (p1 == "NES")
		{
			p1_type = Mappers::ConnectorType::NESStyle;
		}
		else
		{
			board_name = "Bogus";
		}
	}

	BoardFactory::~BoardFactory()
	{
	}

	Board* BoardFactory::CreateInstance()
	{
		Board* inst = nullptr;

		// At this time, we don't pay much attention to the differences between the NES/Famicom models and consider them to be `Generic'.
		// As more information about significant differences appears, we will add it.

		if (std::string(board_name).find("HVC") != std::string::npos)
		{
			inst = new FamicomBoard(apu_rev, ppu_rev, p1_type);
		}
		//else if (std::string(board_name).find("NES") != std::string::npos)
		//{
		//	inst = new NESBoard(apu_rev, ppu_rev, p1_type);
		//}
		//else if (board_name == "APUPlayer")
		//{
		//	inst = new APUPlayerBoard(apu_rev, ppu_rev, p1_type);
		//}
		//else if (board_name == "PPUPlayer")
		//{
		//	inst = new PPUPlayerBoard(apu_rev, ppu_rev, p1_type);
		//}
		//else
		//{
		//	inst = new BogusBoard(apu_rev, ppu_rev, p1_type);
		//}

		return inst;
	}
}