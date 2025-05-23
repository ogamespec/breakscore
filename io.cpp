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

#include "pch.h"

namespace IO
{
	IOSubsystem::IOSubsystem()
	{
	}

	IOSubsystem::~IOSubsystem()
	{
		// Disconnect and remove all devices

		while (!devices.empty()) {

			IOMapped* mapped = devices.back();

			if (mapped->device != nullptr) {
				delete mapped->device;
			}

			devices.pop_back();
			delete mapped;
		}
	}

	int IOSubsystem::CreateInstance(DeviceID device_id)
	{
		IOMapped* mapped = nullptr;
		int num_devices = (int)devices.size();

		switch (device_id)
		{
		case DeviceID::VirtualFamiController_1:
		case DeviceID::FamiController_1:
			mapped = new IOMapped;
			mapped->handle = num_devices;
			mapped->port = -1;
			mapped->device = new IO::FamiController1();
			devices.push_back(mapped);
			return num_devices;

		case DeviceID::VirtualFamiController_2:
		case DeviceID::FamiController_2:
			mapped = new IOMapped;
			mapped->handle = num_devices;
			mapped->port = -1;
			mapped->device = new IO::FamiController2();
			devices.push_back(mapped);
			return num_devices;

		//case DeviceID::VirtualNESController:
		//case DeviceID::NESController:
		//	mapped = new IOMapped;
		//	mapped->handle = num_devices;
		//	mapped->port = -1;
		//	mapped->device = new IO::NESController();
		//	devices.push_back(mapped);
		//	return num_devices;

		case DeviceID::VirtualDendyController:
		case DeviceID::DendyController:
			break;
		}

		return -1;
	}

	void IOSubsystem::DisposeInstance(int handle)
	{
		IOMapped* mapped = GetMappedDeviceByHandle(handle);
		if (mapped != nullptr) {

			if (mapped->device != nullptr) {
				delete mapped->device;
			}

			devices.remove(mapped);
			delete mapped;
		}
	}

	IOMapped* IOSubsystem::GetMappedDeviceByHandle(int handle)
	{
		for (auto it = devices.begin(); it != devices.end(); ++it) {

			IOMapped* mapped = *it;
			if (mapped->handle == handle)
				return mapped;
		}
		return nullptr;
	}

	void IOSubsystem::Attach(int port, int handle)
	{
		if (handle < 0 || port >= GetPorts() || port < 0)
			return;

		IOMapped* mapped = GetMappedDeviceByHandle(handle);
		if (mapped != nullptr) {

			std::list<DeviceID> supported_devices;
			GetPortSupportedDevices(port, supported_devices);

			// Check whether the device can be connected to the specified port by DeviceID

			for (auto it = supported_devices.begin(); it != supported_devices.end(); ++it) {

				if (*it == (DeviceID)mapped->device->GetID()) {

					mapped->port = port;
					break;
				}
			}
		}
	}

	void IOSubsystem::Detach(int port, int handle)
	{
		if (handle < 0 || port >= GetPorts() || port < 0)
			return;

		IOMapped* mapped = GetMappedDeviceByHandle(handle);
		if (mapped != nullptr) {

			mapped->port = -1;
		}
	}

	void IOSubsystem::SetState(int handle, size_t io_state, uint32_t value)
	{
		if (handle < 0)
			return;

		IOMapped* mapped = GetMappedDeviceByHandle(handle);
		if (mapped != nullptr && mapped->port >= 0) {

			mapped->device->SetState(io_state, value);
		}
	}

	uint32_t IOSubsystem::GetState(int handle, size_t io_state)
	{
		if (handle < 0)
			return 0;

		IOMapped* mapped = GetMappedDeviceByHandle(handle);
		if (mapped != nullptr && mapped->port >= 0) {

			return mapped->device->GetState(io_state);
		}

		return 0;
	}

	int IOSubsystem::GetNumStates(int handle)
	{
		if (handle < 0)
			return 0;

		IOMapped* mapped = GetMappedDeviceByHandle(handle);
		if (mapped != nullptr) {

			return mapped->device->GetIOStates();
		}

		return 0;
	}

	std::string IOSubsystem::GetStateName(int handle, size_t io_state)
	{
		if (handle < 0)
			return "";

		IOMapped* mapped = GetMappedDeviceByHandle(handle);
		if (mapped != nullptr) {

			return mapped->device->GetIOStateName(io_state);
		}

		return "";
	}

	// Implemented by the Motherboard instance

	int IOSubsystem::GetPorts()
	{
		return 0;
	}

	std::string IOSubsystem::GetPortName(int port)
	{
		return "";
	}

	void IOSubsystem::GetPortSupportedDevices(int port, std::list<DeviceID>& devices)
	{
		devices.clear();
	}

	void IOSubsystem::sim(int port)
	{
	}
}