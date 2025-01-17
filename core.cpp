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

Breaknes::Board* board = nullptr;

extern "C"
{
	void CreateBoard(char* boardName, char* apu, char* ppu, char* p1)
	{
		if (board == nullptr)
		{
			printf("CreateBoard %s, apu: %s, ppu: %s, cart: %s\n", boardName, apu, ppu, p1);
			Breaknes::BoardFactory bf(boardName, apu, ppu, p1);
			board = bf.CreateInstance();
		}
	}

	void DestroyBoard()
	{
		if (board != nullptr)
		{
			printf("DestroyBoard\n");
			delete board;
			board = nullptr;
		}
	}

	int InsertCartridge(uint8_t* nesImage, size_t size)
	{
		if (board != nullptr)
		{
			printf("InsertCartridge: %zi bytes\n", size);
			return board->InsertCartridge(nesImage, size);
		}
		else
		{
			return -1;
		}
	}

	void EjectCartridge()
	{
		if (board != nullptr)
		{
			printf("EjectCartridge\n");
			board->EjectCartridge();
		}
	}

	void Step()
	{
		if (board != nullptr)
		{
			board->Step();
		}
	}

	void Reset()
	{
		if (board != nullptr)
		{
			board->Reset();
		}
	}

	bool InResetState()
	{
		if (board != nullptr)
		{
			return board->InResetState();
		}
		else
		{
			return false;
		}
	}

	size_t GetACLKCounter()
	{
		if (board != nullptr)
		{
			return board->GetACLKCounter();
		}
		else
		{
			return 0;
		}
	}

	size_t GetPHICounter()
	{
		if (board != nullptr)
		{
			return board->GetPHICounter();
		}
		else
		{
			return 0;
		}
	}

	void SampleAudioSignal(float* sample)
	{
		if (board != nullptr)
		{
			board->SampleAudioSignal(sample);
		}
	}

	void GetApuSignalFeatures(APUSim::AudioSignalFeatures* features)
	{
		if (board != nullptr)
		{
			board->GetApuSignalFeatures(features);
		}
	}

	size_t GetPCLKCounter()
	{
		if (board != nullptr)
		{
			return board->GetPCLKCounter();
		}
		else
		{
			return 0;
		}
	}

	void SampleVideoSignal(PPUSim::VideoOutSignal* sample)
	{
		if (board != nullptr)
		{
			board->SampleVideoSignal(sample);
		}
	}

	size_t GetHCounter()
	{
		if (board != nullptr)
		{
			return board->GetHCounter();
		}
		else
		{
			return 0;
		}
	}

	size_t GetVCounter()
	{
		if (board != nullptr)
		{
			return board->GetVCounter();
		}
		else
		{
			return 0;
		}
	}

	void GetPpuSignalFeatures(PPUSim::VideoSignalFeatures* features)
	{
		if (board != nullptr)
		{
			board->GetPpuSignalFeatures(features);
		}
		else
		{
			// If the Board is not created return the default values for the NTSC PPU.

			features->SamplesPerPCLK = 8;
			features->PixelsPerScan = 341;
			features->ScansPerField = 262;
			features->BackPorchSize = 40;
			features->Composite = true;
			features->BlackLevel = 1.3f;
			features->WhiteLevel = 1.6f;
			features->SyncLevel = 0.781f;
		}
	}

	void ConvertRAWToRGB(uint16_t raw, uint8_t* r, uint8_t* g, uint8_t* b)
	{
		if (board != nullptr)
		{
			board->ConvertRAWToRGB(raw, r, g, b);
		}
		else
		{
			*r = *g = *b = 0;
		}
	}

	void SetRAWColorMode(bool enable)
	{
		if (board != nullptr)
		{
			board->SetRAWColorMode(enable);
		}
	}

	void SetOamDecayBehavior(PPUSim::OAMDecayBehavior behavior)
	{
		if (board != nullptr)
		{
			board->SetOamDecayBehavior(behavior);
		}
	}

	void SetNoiseLevel(float volts)
	{
		if (board != nullptr)
		{
			board->SetNoiseLevel(volts);
		}
	}

	size_t IOCreateInstance(uint32_t device_id)
	{
		if (board != nullptr && board->io != nullptr)
		{
			int handle = board->io->CreateInstance((IO::DeviceID)device_id);
			printf("IOCreateInstance: 0x%08X, handle: %d\n", device_id, handle);
			return handle;
		}
		else {
			return -1;
		}
	}

	void IODisposeInstance(size_t handle)
	{
		if (board != nullptr && board->io != nullptr)
		{
			printf("IODisposeInstance: %d\n", (int)handle);
			board->io->DisposeInstance((int)handle);
		}
	}

	void IOAttach(size_t port, size_t handle)
	{
		if (board != nullptr && board->io != nullptr)
		{
			printf("IOAttach: port: %d, handle: %d\n", (int)port, (int)handle);
			board->io->Attach((int)port, (int)handle);
		}
	}

	void IODetach(size_t port, size_t handle)
	{
		if (board != nullptr && board->io != nullptr)
		{
			printf("IODetach: port: %d, handle: %d\n", (int)port, (int)handle);
			board->io->Detach((int)port, (int)handle);
		}
	}

	void IOSetState(size_t handle, size_t io_state, uint32_t value)
	{
		if (board != nullptr && board->io != nullptr)
		{
			printf("IOSetState: handle: %d, io_state: %d, value: 0x%08X\n", (int)handle, (int)io_state, value);
			board->io->SetState((int)handle, io_state, value);
		}
	}

	uint32_t IOGetState(size_t handle, size_t io_state)
	{
		if (board != nullptr && board->io != nullptr)
		{
			return board->io->GetState((int)handle, io_state);
		}
		else {
			return 0;
		}
	}

	size_t IOGetNumStates(size_t handle)
	{
		if (board != nullptr && board->io != nullptr)
		{
			return board->io->GetNumStates((int)handle);
		}
		else {
			return 0;
		}
	}

	void IOGetStateName(size_t handle, size_t io_state, char* name, size_t name_size)
	{
		if (board != nullptr && board->io != nullptr)
		{
			auto state_name = board->io->GetStateName((int)handle, io_state);
			if (state_name.size() < name_size) {
				strncpy(name, state_name.c_str(), name_size);
				name[state_name.size()] = 0;
			}
		}
	}
};