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

// Famicom Controllers (1/2) simulator
#pragma once

namespace IO
{
	enum class FamiController1State
	{
		Up = 0,
		Down,
		Left,
		Right,
		Select,			// P1 only
		Start,			// P1 only
		B,
		A,
		Max,
	};

	enum class FamiController2State
	{
		Up = 0,
		Down,
		Left,
		Right,
		B,
		A,
		Volume,			// P2 only
		MicLevel,		// P2 only
		Max,
	};

	class FamiController1 : public IODevice
	{
		CD4021 sr{};

	public:
		FamiController1();
		virtual ~FamiController1();

		virtual uint32_t GetID() override;
		virtual std::string GetName() override;
		virtual int GetIOStates() override;
		virtual std::string GetIOStateName(size_t io_state) override;

		virtual void SetState(size_t io_state, uint32_t value) override;
		virtual uint32_t GetState(size_t io_state) override;

		virtual void sim(BaseLogic::TriState inputs[], BaseLogic::TriState outputs[], float analog[]) override;
	};

	class FamiController2 : public IODevice
	{
		CD4021 sr{};

	public:
		FamiController2();
		virtual ~FamiController2();

		virtual uint32_t GetID() override;
		virtual std::string GetName() override;
		virtual int GetIOStates() override;
		virtual std::string GetIOStateName(size_t io_state) override;

		virtual void SetState(size_t io_state, uint32_t value) override;
		virtual uint32_t GetState(size_t io_state) override;

		virtual void sim(BaseLogic::TriState inputs[], BaseLogic::TriState outputs[], float analog[]) override;
	};
}