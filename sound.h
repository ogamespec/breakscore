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

class SoundOutput
{
	SDL_AudioSpec spec{};
	SDL_AudioSpec spec_obtainted{};

	SDL_AudioDeviceID dev_id;

	static void SDLCALL Mixer(void* unused, Uint8* stream, int len);

	void Redecimate();
	void Playback();

	int DecimateEach;
	int DecimateCounter;
	const int OutputSampleRate = 48000;

	int16_t* SampleBuf;
	int SampleBuf_Ptr;
	int SampleBuf_Size;

	bool Dma = false;

public:
	SoundOutput();
	~SoundOutput();

	/// <summary>
	/// This sampler is used for SRC.
	/// The AUX output is sampled at a high frequency, which cannot be played by a ordinary sound card.
	/// Therefore, some of the samples are skipped to match the playback frequency.
	/// </summary>
	void FeedSample();
};