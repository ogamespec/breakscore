// Generic SRAM

// https://github.com/emu-russia/breaks/blob/master/Docs/Famicom/HM6116_SRAM.pdf

#include "pch.h"

using namespace BaseLogic;

namespace BaseBoard
{
	SRAM::SRAM(const char* entity, size_t bits, bool trace)
	{
		memSize = 1LL << bits;
		mem = new uint8_t[memSize];
		memset(mem, 0, memSize);
		do_trace = trace;
		strcpy(sram_name, entity);
	}

	SRAM::~SRAM()
	{
		delete[] mem;
	}

	void SRAM::sim(TriState n_CS, TriState n_WE, TriState n_OE, uint32_t* addr, uint8_t* data, bool& dz)
	{
		if (n_CS == TriState::Zero)
		{
			if (n_WE == TriState::Zero && !dz)
			{
				if (do_trace) {
					printf("%s Write 0x%x = 0x%02x\n", sram_name, *addr, *data);
				}
				mem[*addr] = *data;
			}

			else if (n_OE == TriState::Zero && n_WE == TriState::One)
			{
				*data = mem[*addr];
				dz = false;
			}
		}
	}
}