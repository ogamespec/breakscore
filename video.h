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

class VideoRender
{
	const int ScaleFactor = 2;
	const int SCREEN_WIDTH = 256;
	const int SCREEN_HEIGHT = 240;

	PPUSim::VideoSignalFeatures ppu_features{};
	int SamplesPerScan = 0;
	bool RawMode = false;

	PPUSim::VideoOutSignal* ScanBuffer = nullptr;
	int WritePtr = 0;
	bool SyncFound = false;
	int SyncPos = -1;

	uint32_t* field = nullptr;
	int CurrentScan = 0;

	SDL_Surface* output_surface = nullptr;
	SDL_Window* output_window = nullptr;

	void ProcessScanRAW();
	void VisualizeField();

	int field_counter = 0;

public:
	VideoRender();
	~VideoRender();

	void ProcessSample(PPUSim::VideoOutSignal& sample);
};