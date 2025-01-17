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

#pragma once

namespace IO
{
#define IO_MaxStates 0x100		// Maximum number of IO device actuators

	struct IOState
	{
		std::string actuator_name;
		uint32_t value;
	};

	/// <summary>
	/// Each IO device inherits this class to implement the processing of actuator states.
	/// </summary>
	class IODevice
	{
	protected:
		IOState states[IO_MaxStates]{};

	public:
		IODevice() {}
		virtual ~IODevice() {}

		virtual uint32_t GetID() { return (uint32_t)DeviceID::Abstract; }
		virtual std::string GetName() { return "Abstract"; }
		virtual int GetIOStates() { return 0; }
		virtual std::string GetIOStateName(size_t io_state) { return ""; }

		virtual void SetState(size_t io_state, uint32_t value) {}
		virtual uint32_t GetState(size_t io_state) { return 0; }

		/// <summary>
		/// The number and types of IO port signals are determined by the Motherboard implementation. The IODevice implementation must take this into account
		/// </summary>
		virtual void sim(BaseLogic::TriState inputs[], BaseLogic::TriState outputs[], float analog[]) {}
	};
}