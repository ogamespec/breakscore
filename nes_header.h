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

#pragma pack(push, 1)
struct NESHeader
{
	uint8_t Sign[4];
	uint8_t PRGSize;
	uint8_t CHRSize;
	uint8_t Flags_6;
	uint8_t Flags_7;
	uint8_t Flags_8;
	uint8_t Flags_9;
	uint8_t Flags_10;
	uint8_t padding[5];
};
#pragma pack(pop)

#define NES_TRAINER_SIZE 512