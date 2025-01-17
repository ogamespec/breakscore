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

// The abstract cartridge connector interface must include all the variety of signals, for all motherboard options.

#pragma once

namespace Mappers
{
	enum class ConnectorType
	{
		None = 0,
		FamicomStyle,
		NESStyle,
		Max,
	};

	enum class CartInput
	{
		SYSTEM_CLK,			// NES only
		M2,
		nROMSEL,
		RnW,
		nRD,
		nWR,
		nPA13,
		CIC_CLK,			// NES only
		CIC_TO_CART,		// NES only
		Max,
	};

	enum class CartOutput
	{
		VRAM_A10,
		VRAM_nCS,
		nIRQ,
		CIC_RST,			// NES only
		CIC_TO_MB,			// NES only
		Max,
	};

	/// <summary>
	/// A software descriptor of the current audio sample from cartridge port.
	/// Famicom only
	/// </summary>
	union CartAudioOutSignal
	{
		float normalized;
	};

	class AbstractCartridge
	{
	protected:
		ConnectorType p1_type;

		BaseLogic::TriState gnd = BaseLogic::TriState::Zero;
		BaseLogic::TriState vdd = BaseLogic::TriState::One;

	public:
		AbstractCartridge(ConnectorType _p1_type, uint8_t* nesImage, size_t size);
		virtual ~AbstractCartridge();

		virtual bool Valid();

		virtual void sim(
			BaseLogic::TriState cart_in[(size_t)CartInput::Max],
			BaseLogic::TriState cart_out[(size_t)CartOutput::Max],
			uint16_t cpu_addr,
			uint8_t* cpu_data, bool& cpu_data_dirty,
			uint16_t ppu_addr,
			uint8_t* ppu_data, bool& ppu_data_dirty,
			// Famicom only
			CartAudioOutSignal* snd_out,
			// NES only
			uint16_t* exp, bool& exp_dirty) = 0;
	};

	class CartridgeFactory
	{
		ConnectorType p1_type = ConnectorType::None;
		uint8_t* data = nullptr;
		size_t data_size = 0;

	public:
		CartridgeFactory(ConnectorType p1, uint8_t* nesImage, size_t size);
		~CartridgeFactory();

		AbstractCartridge* GetInstance();
	};
}