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
	class IODevice;

	/// <summary>
	/// A unique device identifier that completely defines the device model and logic.
	/// </summary>
	enum class DeviceID
	{
		Abstract = 0,
		FamiController_1 = 0x00000001,
		FamiController_2 = 0x00000002,
		NESController = 0x00000003,
		DendyController = 0x00000004,
		VirtualFamiController_1 = 0x00010001,
		VirtualFamiController_2 = 0x00010002,
		VirtualNESController = 0x00010003,
		VirtualDendyController = 0x00010004,
	};

	struct IOMapped
	{
		int port;		// -1: detached
		int handle;		// -1: no handle
		IODevice* device;
	};

	/// <summary>
	/// Each motherboard implementation inherits this class to implement port handling by their simulation.
	/// </summary>
	class IOSubsystem
	{
		IOMapped* GetMappedDeviceByHandle(int handle);

	protected:
		std::list<IOMapped*> devices;

	public:
		IOSubsystem();
		virtual ~IOSubsystem();

#pragma region "Interface for the emulator"

		int CreateInstance(DeviceID device_id);
		void DisposeInstance(int handle);

		void Attach(int port, int handle);
		void Detach(int port, int handle);

		void SetState(int handle, size_t io_state, uint32_t value);
		uint32_t GetState(int handle, size_t io_state);

		int GetNumStates(int handle);
		std::string GetStateName(int handle, size_t io_state);

#pragma endregion "Interface for the emulator"

#pragma region "Interface for Motherboard implementation"

		virtual int GetPorts();
		virtual std::string GetPortName(int port);
		virtual void GetPortSupportedDevices(int port, std::list<DeviceID>& devices);

		virtual void sim(int port);

#pragma endregion "Interface for Motherboard implementation"

	};
}