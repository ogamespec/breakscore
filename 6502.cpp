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

using namespace BaseLogic;

namespace M6502Core
{
	Decoder::Decoder()
	{
		pla = new PLA(inputs_count, outputs_count, (char*)"Decoder6502.bin");

		// The bitmask corresponds to the values from the Breaking NES Wiki:
		// https://github.com/emu-russia/breaks/blob/master/BreakingNESWiki_DeepL/6502/decoder.md

		static size_t bitmask[outputs_count] = {
			// A
			0b000101100000100100000,
			0b000000010110001000100,
			0b000000011010001001000,
			0b010100011001100100000,
			0b010101011010100100000,
			0b010110000001100100000,

			// B
			0b000000100010000001000,
			0b000001000000100010000,
			0b000000010101001001000,
			0b010101011001100010000,
			0b010110011001100010000,
			0b011010000001100100000,
			0b000101000000100010000,
			0b010101011010100010000,
			0b011001000000100010000,
			0b100110011001100010000,
			0b101010011001100100000,
			0b011001011010100010000,
			0b100100011001100100000,
			0b011001100000100100000,
			0b011001000001100100000,

			// C
			0b011001010101010100000,
			0b000101010101010100001,
			0b010100011001010100000,
			0b001010010101010100010,
			0b001000011001010100100,
			0b000110010101010100001,
			0b001010000000010010000,
			0b000000000000000001000,
			0b010110000000011000000,
			0b000010101001010100000,
			0b000000101001000001000,
			0b010101000000011000000,
			0b000000000100000001000,
			0b010000000000000000000,
			0b000000010001010101000,
			0b000000000001010100100,

			// D
			0b000001010101010100010,
			0b000110010101010100010,
			0b000000010101001000100,
			0b000000010110001000010,
			0b000000010110001001000,
			0b000000001010000000100,
			0b001000011001010100000,
			0b001010000000100010000,
			0b000000010101001000010,
			0b000000010110001000100,
			0b000010010101010100000,
			0b001001010101010101000,
			0b010010000001100100000,
			0b010110000000101000000,
			0b011010000000101000000,
			0b011010000000001000000,
			0b001001000000010010000,

			// E
			0b000010101001010100100,
			0b000001000000010010000,
			0b001001010101010100001,
			0b000000010001010101000,
			0b010101011010100100000,
			0b100000000000011000000,
			0b101010000000001000000,
			0b100000011001010010000,
			0b010101011001100010000,
			0b011010011001010100000,
			0b011001000000101000000,
			0b010000000000001000000,
			0b011001011001100100000,
			0b010000011001010010000,
			0b011001011001100010000,
			0b011001100001010100000,
			0b011001000000011000000,
			0b000000001010000000010,
			0b000000010110001000001,

			// F
			0b010000010110000100000,
			0b000110011001010101000,
			0b010010011001010010000,
			0b000010000000010010000,
			0b000101010101010101000,
			0b001001010101010100100,
			0b000101000000101000000,
			0b000000010110000101000,
			0b000000100100000001000,
			0b000000010100001001000,
			0b000000001000000001000,
			0b001010010101010100001,
			0b000000000000000000010,
			0b000000000000000000100,
			0b010100010101010100000,
			0b010010101001010100000,
			0b000000010101001000001,
			0b000000001000000000100,

			// G
			0b000000010110001000010,
			0b000000001010000000100,
			0b000000010110000100100,
			0b000100010101010100000,
			0b001001010101010100000,
			0b000010101001010100000,
			0b000101000000100000000,
			0b000101010101010100010,
			0b000101011001010101000,
			0b000100011001010101000,
			0b000010101001010100010,
			0b000010010101010100001,
			0b001001010101010100001,

			// H
			0b000110101001010101000,
			0b001000011001010100100,
			0b000010000000000010000,
			0b000001000000010010000,
			0b010010011010010100000,
			0b101001100001010100000,
			0b010001011010010100000,
			0b000000100110000000100,
			0b101010000000001000000,
			0b011001100001010100000,
			0b011001011001010100000,
			0b000110010101010100010,
			0b100110000000101000000,
			0b100010101001100100000,
			0b100001011001010010000,
			0b100010000101100100000,

			// K
			0b010010011010100100000,
			0b000001000000000000000,
			0b000000101001000000100,
			0b000000100101000001000,
			0b000000010100001000001,
			0b000000001010000000010,
			0b000000000000010000000,
			0b001001011010100100000,
			0b000000011000000000000,
			0b000000011001010100000,		// pp
		};

		pla->SetMatrix(bitmask);
	}

	Decoder::~Decoder()
	{
		delete pla;
	}

	void Decoder::sim(size_t input_bits, TriState** outputs)
	{
		pla->sim(input_bits, outputs);
	}

	void IR::sim()
	{
		if (core->wire.PHI1 && core->wire.FETCH)
		{
			ir_latch = core->predecode->n_PD;
			IROut = ~ir_latch;
		}
	}

	PreDecode::PreDecode(M6502* parent)
	{
		core = parent;

		// Pre-calculate the values for n_TWOCYCLE / n_IMPLIED

		for (size_t pd = 0; pd < 0x100; pd++)
		{
			TriState PD0 = pd & 0b00000001 ? TriState::One : TriState::Zero;
			TriState PD1 = pd & 0b00000010 ? TriState::One : TriState::Zero;
			TriState PD2 = pd & 0b00000100 ? TriState::One : TriState::Zero;
			TriState PD3 = pd & 0b00001000 ? TriState::One : TriState::Zero;
			TriState PD4 = pd & 0b00010000 ? TriState::One : TriState::Zero;
			TriState PD5 = pd & 0b00100000 ? TriState::One : TriState::Zero;
			TriState PD6 = pd & 0b01000000 ? TriState::One : TriState::Zero;
			TriState PD7 = pd & 0b10000000 ? TriState::One : TriState::Zero;

			TriState IMPLIED = NOR3(PD0, PD2, NOT(PD3));

			TriState res2 = NOR3(PD1, PD4, PD7);

			TriState in3[4]{};
			in3[0] = NOT(PD0);
			in3[1] = PD2;
			in3[2] = NOT(PD3);
			in3[3] = PD4;
			TriState res3 = NOR4(in3);

			TriState in4[5]{};
			in4[0] = PD0;
			in4[1] = PD2;
			in4[2] = PD3;
			in4[3] = PD4;
			in4[4] = NOT(PD7);
			TriState res4 = NOR5(in4);

			precalc_n_TWOCYCLE[pd] = AND(NAND(IMPLIED, NOT(res2)), NOR(res3, res4));
			precalc_n_IMPLIED[pd] = NOT(IMPLIED);
		}
	}

	void PreDecode::sim(uint8_t* data_bus)
	{
		TriState PHI2 = core->wire.PHI2;
		TriState Z_IR = core->wire.Z_IR;

		if (PHI2 != TriState::One)
		{
			PD = Z_IR ? 0 : ~pd_latch;
			n_PD = ~PD;
			return;
		}

		if (PHI2)
		{
			pd_latch = ~(*data_bus);
		}

		PD = Z_IR ? 0 : ~pd_latch;
		n_PD = ~PD;

		core->wire.n_TWOCYCLE = precalc_n_TWOCYCLE[PD];
		core->wire.n_IMPLIED = precalc_n_IMPLIED[PD];
	}

	void ExtraCounter::sim()
	{
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState T1 = core->disp->getT1();
		TriState TRES2 = core->disp->getTRES2();

		TriState T2;
		TriState T3;
		TriState T4;
		TriState T5;

		if (PHI1 == TriState::One)
		{
			TriState n_ready = core->wire.n_ready;

			t2_latch1.set(MUX(n_ready, t1_latch.nget(), t2_latch2.nget()), PHI1);
			t3_latch1.set(MUX(n_ready, t2_latch2.nget(), t3_latch2.nget()), PHI1);
			t4_latch1.set(MUX(n_ready, t3_latch2.nget(), t4_latch2.nget()), PHI1);
			t5_latch1.set(MUX(n_ready, t4_latch2.nget(), t5_latch2.nget()), PHI1);

			T2 = NOR(t2_latch1.get(), TRES2);
			T3 = NOR(t3_latch1.get(), TRES2);
			T4 = NOR(t4_latch1.get(), TRES2);
			T5 = NOR(t5_latch1.get(), TRES2);
		}
		else
		{
			t1_latch.set(T1, PHI2);

			T2 = NOR(t2_latch1.get(), TRES2);
			t2_latch2.set(T2, PHI2);

			T3 = NOR(t3_latch1.get(), TRES2);
			t3_latch2.set(T3, PHI2);

			T4 = NOR(t4_latch1.get(), TRES2);
			t4_latch2.set(T4, PHI2);

			T5 = NOR(t5_latch1.get(), TRES2);
			t5_latch2.set(T5, PHI2);
		}

		core->wire.n_T2 = NOT(T2);
		core->wire.n_T3 = NOT(T3);
		core->wire.n_T4 = NOT(T4);
		core->wire.n_T5 = NOT(T5);
	}

	void ExtraCounter::sim_HLE()
	{
		TriState PHI1 = core->wire.PHI1;
		TriState T1 = core->disp->getT1();
		TriState TRES2 = core->disp->getTRES2();
		TriState n_ready = core->wire.n_ready;

		// HLE does not use the inverse essence of the shift register latches.

		if (PHI1)
		{
			if (n_ready)
			{
				latch1 = latch2;
			}
			else
			{
				latch1 = (latch2 << 1) | t1_latch.get();
			}
		}
		else
		{
			t1_latch.set(T1, TriState::One);
			latch2 = TRES2 ? 0 : latch1;
		}

		uint8_t Tx = TRES2 ? 0 : latch1;

		core->wire.n_T2 = Tx & 0b0001 ? TriState::Zero : TriState::One;
		core->wire.n_T3 = Tx & 0b0010 ? TriState::Zero : TriState::One;
		core->wire.n_T4 = Tx & 0b0100 ? TriState::Zero : TriState::One;
		core->wire.n_T5 = Tx & 0b1000 ? TriState::Zero : TriState::One;
	}

	void BRKProcessing::sim_BeforeRandom()
	{
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState BRK5 = core->decoder_out[22];
		TriState n_ready = core->wire.n_ready;
		TriState RESP = core->wire.RESP;
		TriState n_NMIP = core->wire.n_NMIP;

		// Interrupt cycle 6-7

		TriState BRK5_RDY = AND(BRK5, NOT(n_ready));
		brk5_latch.set(BRK5_RDY, PHI2);
		brk6_latch1.set(AND(NOT(brk5_latch.get()), NAND(n_ready, brk6_latch1.nget())), PHI1);
		brk6_latch2.set(brk6_latch1.nget(), PHI2);
		TriState BRK6E = NOR(brk6_latch2.nget(), n_ready);
		TriState BRK7 = NOR(brk6_latch1.nget(), BRK5_RDY);

		// Reset FF

		res_latch1.set(RESP, PHI2);
		res_latch2.set(NOR(NOR(res_latch1.get(), res_latch2.get()), BRK6E), PHI1);
		TriState res_out = NOR(res_latch1.get(), res_latch2.get());
		TriState DORES = NOT(res_out);

		// NMI Edge Detect

		brk6e_latch.set(BRK6E, PHI1);
		brk7_latch.set(BRK7, PHI2);
		nmip_latch.set(n_NMIP, PHI1);

		donmi_latch.set(NOR3(brk7_latch.nget(), n_NMIP, NOR(nmip_latch.get(), NOR(ff2_latch.get(), delay_latch2.get()))), PHI1);

		ff1_latch.set(NOR(donmi_latch.get(), NOR(ff1_latch.get(), brk6e_latch.get())), PHI2);
		TriState n_DONMI = NOR(donmi_latch.get(), NOR(ff1_latch.get(), brk6e_latch.get()));

		delay_latch1.set(n_DONMI, PHI2);
		delay_latch2.set(delay_latch1.nget(), PHI1);

		ff2_latch.set(NOR(nmip_latch.get(), NOR(ff2_latch.get(), delay_latch2.get())), PHI2);

		// Interrupt vector

		zadl_latch[0].set(NOT(BRK5_RDY), PHI2);
		zadl_latch[1].set(NOT(NOR(BRK7, NOT(DORES))), PHI2);
		zadl_latch[2].set(NOR3(BRK7, DORES, n_DONMI), PHI2);

		// Outputs

		core->wire.BRK6E = BRK6E;
		core->wire.BRK7 = BRK7;
		core->wire.DORES = DORES;
		core->cmd.Z_ADL0 = zadl_latch[0].nget();
		core->cmd.Z_ADL1 = zadl_latch[1].nget();
		core->cmd.Z_ADL2 = NOT(zadl_latch[2].nget());
		core->wire.n_DONMI = n_DONMI;
		core->wire.BRK5_RDY = BRK5_RDY;
	}

	void BRKProcessing::sim_AfterRandom()
	{
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState T0 = core->wire.T0;
		TriState BR2 = core->decoder_out[80];
		TriState n_I_OUT = core->random->flags->getn_I_OUT(core->wire.BRK6E);
		TriState n_IRQP = core->wire.n_IRQP;
		TriState n_DONMI = core->wire.n_DONMI;
		TriState BRK6E = core->wire.BRK6E;
		TriState DORES = core->wire.DORES;

		// B Flag

		TriState int_set = NAND(
			OR(BR2, T0),
			NAND(n_DONMI, OR(n_IRQP, NOT(n_I_OUT)))
		);

		b_latch2.set(NOR(b_latch1.get(), BRK6E), PHI1);
		b_latch1.set(AND(int_set, NOT(b_latch2.get())), PHI2);
		TriState B_OUT = NOR(DORES, NOR(b_latch1.get(), BRK6E));

		core->wire.B_OUT = B_OUT;
	}

	TriState BRKProcessing::getDORES()
	{
		TriState DORES = NOT(NOR(res_latch1.get(), res_latch2.get()));
		return DORES;
	}

	TriState BRKProcessing::getB_OUT(TriState BRK6E)
	{
		TriState DORES = getDORES();
		TriState B_OUT = NOR(DORES, NOR(b_latch1.get(), BRK6E));
		return B_OUT;
	}

	TriState BRKProcessing::getn_BRK6_LATCH2()
	{
		return brk6_latch2.nget();
	}

	void Flags::sim_Load()
	{
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState SO = core->wire.SO;
		TriState ACR = core->alu->getACR();
		TriState AVR = core->alu->getAVR();
		TriState n_IR5 = core->wire.n_IR5;

		TriState DB_P = core->cmd.DB_P ? TriState::One : TriState::Zero;
		TriState AVR_V = core->cmd.AVR_V ? TriState::One : TriState::Zero;

		// Z

		if (PHI1)
		{
			TriState DBZ_Z = core->cmd.DBZ_Z ? TriState::One : TriState::Zero;
			TriState DB1 = core->DB & 0b00000010 ? TriState::One : TriState::Zero;
			TriState n_DBZ = NOT(core->DB == 0 ? TriState::One : TriState::Zero);
			TriState z[3]{};
			z[0] = AND(NOT(DB1), DB_P);
			z[1] = AND(n_DBZ, DBZ_Z);
			z[2] = AND(NOR(DB_P, DBZ_Z), z_latch2.get());

			z_latch1.set(NOR3(z[0], z[1], z[2]), PHI1);
		}
		else
		{
			z_latch2.set(z_latch1.nget(), PHI2);
		}

		// N

		if (PHI1)
		{
			TriState DB_N = core->cmd.DB_N ? TriState::One : TriState::Zero;
			TriState DB7 = core->DB & 0b10000000 ? TriState::One : TriState::Zero;
			TriState n[2]{};
			n[0] = AND(NOT(DB7), DB_N);
			n[1] = AND(NOT(DB_N), n_latch2.get());

			n_latch1.set(NOR(n[0], n[1]), PHI1);
		}
		else
		{
			n_latch2.set(n_latch1.nget(), PHI2);
		}

		// C

		if (PHI1)
		{
			TriState IR5_C = core->cmd.IR5_C ? TriState::One : TriState::Zero;
			TriState DB_C = core->cmd.DB_C ? TriState::One : TriState::Zero;
			TriState ACR_C = core->cmd.ACR_C ? TriState::One : TriState::Zero;
			TriState DB0 = core->DB & 0b00000001 ? TriState::One : TriState::Zero;
			TriState c[4]{};
			c[0] = AND(n_IR5, IR5_C);
			c[1] = AND(NOT(ACR), ACR_C);
			c[2] = AND(NOT(DB0), DB_C);
			c[3] = AND(NOR3(DB_C, IR5_C, ACR_C), c_latch2.get());

			c_latch1.set(NOR4(c), PHI1);
		}
		else
		{
			c_latch2.set(c_latch1.nget(), PHI2);
		}

		// D

		if (PHI1)
		{
			TriState IR5_D = core->cmd.IR5_D ? TriState::One : TriState::Zero;
			TriState DB3 = core->DB & 0b00001000 ? TriState::One : TriState::Zero;
			TriState d[3]{};
			d[0] = AND(IR5_D, n_IR5);
			d[1] = AND(NOT(DB3), DB_P);
			d[2] = AND(NOR(IR5_D, DB_P), d_latch2.get());

			d_latch1.set(NOR3(d[0], d[1], d[2]), PHI1);
		}
		else
		{
			d_latch2.set(d_latch1.nget(), PHI2);
		}

		// I

		if (PHI1)
		{
			TriState IR5_I = core->cmd.IR5_I ? TriState::One : TriState::Zero;
			TriState DB2 = core->DB & 0b00000100 ? TriState::One : TriState::Zero;
			TriState i[3]{};
			i[0] = AND(n_IR5, IR5_I);
			i[1] = AND(NOT(DB2), DB_P);
			i[2] = AND(NOR(DB_P, IR5_I), i_latch2.get());

			i_latch1.set(NOR3(i[0], i[1], i[2]), PHI1);
		}
		else
		{
			TriState BRK6E = core->wire.BRK6E;
			i_latch2.set(AND(i_latch1.nget(), NOT(BRK6E)), PHI2);
		}

		// V

		avr_latch.set(AVR_V, PHI2);
		so_latch1.set(NOT(SO), PHI1);
		so_latch2.set(so_latch1.nget(), PHI2);
		so_latch3.set(so_latch2.nget(), PHI1);
		vset_latch.set(NOR(so_latch1.nget(), so_latch3.get()), PHI2);

		if (PHI1)
		{
			TriState DB_V = core->cmd.DB_V ? TriState::One : TriState::Zero;
			TriState Z_V = core->cmd.Z_V ? TriState::One : TriState::Zero;
			TriState DB6 = core->DB & 0b01000000 ? TriState::One : TriState::Zero;
			TriState v[4]{};
			v[0] = AND(NOT(AVR), avr_latch.get());
			v[1] = AND(NOT(DB6), DB_V);
			v[2] = AND(NOR3(DB_V, avr_latch.get(), vset_latch.get()), v_latch2.get());
			v[3] = Z_V;

			v_latch1.set(NOR4(v), PHI1);
		}
		else
		{
			v_latch2.set(v_latch1.nget(), PHI2);
		}
	}

	void Flags::sim_Store()
	{
		if (core->cmd.P_DB)
		{
			core->DB &= 0b00100000;

			core->DB |= NOT(getn_C_OUT()) << 0;

			core->DB |= NOT(getn_Z_OUT()) << 1;

			core->DB |= NOT(getn_I_OUT(core->wire.BRK6E)) << 2;

			core->DB |= NOT(getn_D_OUT()) << 3;

			core->DB |= core->brk->getB_OUT(core->wire.BRK6E) << 4;

			//core->DB[5];

			core->DB |= NOT(getn_V_OUT()) << 6;

			core->DB |= NOT(getn_N_OUT()) << 7;

			core->DB_Dirty = true;
		}
	}

	BaseLogic::TriState Flags::getn_Z_OUT()
	{
		return z_latch1.nget();
	}

	BaseLogic::TriState Flags::getn_N_OUT()
	{
		return n_latch1.nget();
	}

	BaseLogic::TriState Flags::getn_C_OUT()
	{
		return c_latch1.nget();
	}

	BaseLogic::TriState Flags::getn_D_OUT()
	{
		return d_latch1.nget();
	}

	BaseLogic::TriState Flags::getn_I_OUT(TriState BRK6E)
	{
		return AND(i_latch1.nget(), NOT(BRK6E));
	}

	BaseLogic::TriState Flags::getn_V_OUT()
	{
		return v_latch1.nget();
	}

	void Flags::set_Z_OUT(TriState val)
	{
		z_latch1.set(val, TriState::One);
	}

	void Flags::set_N_OUT(TriState val)
	{
		n_latch1.set(val, TriState::One);
	}

	void Flags::set_C_OUT(TriState val)
	{
		c_latch1.set(val, TriState::One);
	}

	void Flags::set_D_OUT(TriState val)
	{
		d_latch1.set(val, TriState::One);
	}

	void Flags::set_I_OUT(TriState val)
	{
		i_latch1.set(val, TriState::One);
	}

	void Flags::set_V_OUT(TriState val)
	{
		v_latch1.set(val, TriState::One);
	}

	RegsControl::RegsControl(M6502* parent)
	{
		core = parent;

		size_t maxVal = 1ULL << 16;
		for (size_t n = 0; n < maxVal; n++)
		{
			uint8_t ir = n & 0xff;
			bool n_T0 = (n >> 8) & 1;
			bool n_T1X = (n >> 9) & 1;
			bool n_T2 = (n >> 10) & 1;
			bool n_T3 = (n >> 11) & 1;
			bool n_T4 = (n >> 12) & 1;
			bool n_T5 = (n >> 13) & 1;
			bool n_ready = (n >> 14) & 1;
			bool n_ready_latch = (n >> 15) & 1;

			temp_tab[n] = PreCalc(ir, n_T0, n_T1X, n_T2, n_T3, n_T4, n_T5, n_ready, n_ready_latch);
		}

		prev_temp.bits = 0xff;
	}

	void RegsControl::sim()
	{
		TriState* d = core->decoder_out;
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState n_ready = core->wire.n_ready;

		nready_latch.set(n_ready, PHI1);

		// Register control commands and auxiliary signals for other parts of the random logic

		if (PHI2 == TriState::One)
		{
			bool nready_latch_val = nready_latch.get();

			size_t n = 0;
			n |= core->ir->IROut;
			n |= ((size_t)core->TxBits << 8);
			n |= ((size_t)n_ready << 14);
			n |= ((size_t)nready_latch_val << 15);

			RegsControl_TempWire temp = temp_tab[n];

			if (prev_temp.bits != temp.bits)
			{
				ysb_latch.set(temp.n_Y_SB ? TriState::One : TriState::Zero, PHI2);
				xsb_latch.set(temp.n_X_SB ? TriState::One : TriState::Zero, PHI2);
				sbx_latch.set(temp.n_SB_X ? TriState::One : TriState::Zero, PHI2);
				sby_latch.set(temp.n_SB_Y ? TriState::One : TriState::Zero, PHI2);
				sbs_latch.set(temp.n_SB_S ? TriState::One : TriState::Zero, PHI2);
				ss_latch.set(NOT(temp.n_SB_S ? TriState::One : TriState::Zero), PHI2);
				sadl_latch.set(temp.n_S_ADL ? TriState::One : TriState::Zero, PHI2);
				prev_temp.bits = temp.bits;
			}

			ssb_latch.set(NOT(d[17]), PHI2);
		}

		// Outputs

		if (PHI2 == TriState::One)
		{
			core->cmd.Y_SB = 0;
			core->cmd.X_SB = 0;
			core->cmd.SB_X = 0;
			core->cmd.SB_Y = 0;
			core->cmd.SB_S = 0;
			core->cmd.S_S = 0;
		}
		else
		{
			core->cmd.Y_SB = NOT(ysb_latch.get());
			core->cmd.X_SB = NOT(xsb_latch.get());
			core->cmd.SB_X = NOT(sbx_latch.get());
			core->cmd.SB_Y = NOT(sby_latch.get());
			core->cmd.SB_S = NOT(sbs_latch.get());
			core->cmd.S_S = NOT(ss_latch.get());
		}

		core->cmd.S_SB = ssb_latch.nget();
		core->cmd.S_ADL = sadl_latch.nget();
	}

	RegsControl_TempWire RegsControl::PreCalc(uint8_t ir, bool n_T0, bool n_T1X, bool n_T2, bool n_T3, bool n_T4, bool n_T5, bool n_ready, bool n_ready_latch)
	{
		TriState* d;
		DecoderInput decoder_in{};
		decoder_in.packed_bits = 0;
		RegsControl_TempWire temp{};
		temp.bits = 0;

		TriState IR0 = ir & 0b00000001 ? TriState::One : TriState::Zero;
		TriState IR1 = ir & 0b00000010 ? TriState::One : TriState::Zero;
		TriState IR2 = ir & 0b00000100 ? TriState::One : TriState::Zero;
		TriState IR3 = ir & 0b00001000 ? TriState::One : TriState::Zero;
		TriState IR4 = ir & 0b00010000 ? TriState::One : TriState::Zero;
		TriState IR5 = ir & 0b00100000 ? TriState::One : TriState::Zero;
		TriState IR6 = ir & 0b01000000 ? TriState::One : TriState::Zero;
		TriState IR7 = ir & 0b10000000 ? TriState::One : TriState::Zero;

		decoder_in.n_IR0 = NOT(IR0);
		decoder_in.n_IR1 = NOT(IR1);
		decoder_in.IR01 = OR(IR0, IR1);
		decoder_in.n_IR2 = NOT(IR2);
		decoder_in.IR2 = IR2;
		decoder_in.n_IR3 = NOT(IR3);
		decoder_in.IR3 = IR3;
		decoder_in.n_IR4 = NOT(IR4);
		decoder_in.IR4 = IR4;
		decoder_in.n_IR5 = NOT(IR5);
		decoder_in.IR5 = IR5;
		decoder_in.n_IR6 = NOT(IR6);
		decoder_in.IR6 = IR6;
		decoder_in.n_IR7 = NOT(IR7);
		decoder_in.IR7 = IR7;

		decoder_in.n_T0 = n_T0;
		decoder_in.n_T1X = n_T1X;
		decoder_in.n_T2 = n_T2;
		decoder_in.n_T3 = n_T3;
		decoder_in.n_T4 = n_T4;
		decoder_in.n_T5 = n_T5;

		core->decoder->sim(decoder_in.packed_bits, &d);

		// Wires

		TriState memop_in[5]{};
		memop_in[0] = d[111];
		memop_in[1] = d[122];
		memop_in[2] = d[123];
		memop_in[3] = d[124];
		memop_in[4] = d[125];
		TriState n_MemOp = NOR5(memop_in);

		TriState n_STORE = NOT(d[97]);
		TriState STOR = NOR(n_MemOp, n_STORE);

		TriState TXS = d[13];

		TriState n1[7]{};
		n1[0] = d[1];
		n1[1] = d[2];
		n1[2] = d[3];
		n1[3] = d[4];
		n1[4] = d[5];
		n1[5] = AND(d[6], d[7]);
		n1[6] = AND(d[0], STOR);
		temp.n_Y_SB = NOR7(n1);

		TriState n2[7]{};
		n2[0] = AND(STOR, d[12]);
		n2[1] = AND(d[6], NOT(d[7]));
		n2[2] = d[8];
		n2[3] = d[9];
		n2[4] = d[10];
		n2[5] = d[11];
		n2[6] = TXS;
		temp.n_X_SB = NOR7(n2);

		temp.n_SB_X = NOR3(d[14], d[15], d[16]);
		temp.n_SB_Y = NOR3(d[18], d[19], d[20]);

		TriState n3[6]{};
		n3[0] = d[21];
		n3[1] = d[22];
		n3[2] = d[23];
		n3[3] = d[24];
		n3[4] = d[25];
		n3[5] = d[26];
		TriState STKOP = NOR(n_ready_latch ? TriState::One : TriState::Zero, NOR6(n3));

		temp.n_SB_S = NOR3(TXS, NOR(NOT(d[48]), n_ready ? TriState::One : TriState::Zero), STKOP);

		temp.n_S_ADL = NOR(AND(d[21], n_ready_latch ? TriState::Zero : TriState::One), d[35]);

		return temp;
	}

	ALUControl::ALUControl(M6502* parent)
	{
		core = parent;

		size_t maxVal = 1ULL << 19;
		for (size_t n = 0; n < maxVal; n++)
		{
			uint8_t ir = n & 0xff;
			bool n_T0 = (n >> 8) & 1;
			bool n_T1X = (n >> 9) & 1;
			bool n_T2 = (n >> 10) & 1;
			bool n_T3 = (n >> 11) & 1;
			bool n_T4 = (n >> 12) & 1;
			bool n_T5 = (n >> 13) & 1;
			bool n_ready = (n >> 14) & 1;
			bool T0 = (n >> 15) & 1;
			bool RMW_T6 = (n >> 16) & 1;
			bool BRFW = (n >> 17) & 1;
			bool n_C_OUT = (n >> 18) & 1;

			temp_tab1[n] = PreCalc1(ir, n_T0, n_T1X, n_T2, n_T3, n_T4, n_T5,
				n_ready, T0, RMW_T6, BRFW, n_C_OUT);
		}

		prev_temp1.bits = 0xff;
	}

	void ALUControl::sim()
	{
		TriState* d = core->decoder_out;
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState n_ready = core->wire.n_ready;
		TriState RMW_T6 = core->wire.RMW_T6;
		TriState n_C_OUT = core->random->flags->getn_C_OUT();

		nready_latch.set(n_ready, PHI1);

		if (PHI2)
		{
			TriState n3[6];
			n3[0] = d[21];
			n3[1] = d[22];
			n3[2] = d[23];
			n3[3] = d[24];
			n3[4] = d[25];
			n3[5] = d[26];
			STKOP = NOR(nready_latch.get(), NOR6(n3));
		}

		sim_CarryBCD();

		sim_ALUInput();

		sim_ALUOps();

		// ADD/SB7

		TriState SR = NOT(NOR(d[75], AND(d[76], RMW_T6)));
		TriState n_ROR = NOT(d[27]);
		cout_latch.set(NOT(n_C_OUT), PHI2);
		mux_latch1.set(NOR(nready_latch.get(), NOT(SR)), PHI2);
		sr_latch1.set(SR, PHI2);
		sr_latch2.set(sr_latch1.nget(), PHI1);
		ff_latch1.set(NOT(MUX(mux_latch1.get(), ff_latch2.get(), cout_latch.nget())), PHI1);
		ff_latch2.set(ff_latch1.nget(), PHI2);
		n_ADD_SB7 = NOR3(ff_latch1.nget(), sr_latch2.get(), n_ROR);

		sim_ADDOut();

		// Outputs

		if (PHI2 == TriState::One)
		{
			core->cmd.NDB_ADD = 0;
			core->cmd.DB_ADD = 0;
			core->cmd.Z_ADD = 0;
			core->cmd.SB_ADD = 0;
			core->cmd.ADL_ADD = 0;
		}
		else
		{
			core->cmd.NDB_ADD = NOT(ndbadd_latch.get());
			core->cmd.DB_ADD = NOT(dbadd_latch.get());
			core->cmd.Z_ADD = NOT(zadd_latch.get());
			core->cmd.SB_ADD = NOT(sbadd_latch.get());
			core->cmd.ADL_ADD = NOT(adladd_latch.get());
		}

		core->cmd.ADD_SB7 = addsb7_latch.get();	// care!
		core->cmd.ADD_SB06 = addsb06_latch.nget();
		core->cmd.ADD_ADL = addadl_latch.nget();

		core->cmd.ANDS = ands_latch2.nget();
		core->cmd.EORS = eors_latch2.nget();
		core->cmd.ORS = ors_latch2.nget();
		core->cmd.SRS = srs_latch2.nget();
		core->cmd.SUMS = sums_latch2.nget();

		core->cmd.n_ACIN = NOT(acin_latch5.nget());
		core->cmd.n_DAA = daa_latch2.nget();
		core->cmd.n_DSA = dsa_latch2.nget();
	}

	void ALUControl::sim_CarryBCD()
	{
		TriState* d = core->decoder_out;
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState n_ready = core->wire.n_ready;
		TriState T0 = core->wire.T0;
		TriState RMW_T6 = core->wire.RMW_T6;
		TriState BRFW = core->random->branch_logic->getBRFW();
		TriState n_C_OUT = core->random->flags->getn_C_OUT();
		TriState n_D_OUT = core->random->flags->getn_D_OUT();

		TriState SBC0 = d[51];

		// Additional signals (/ACIN, /DAA, /DSA)

		if (PHI2 == TriState::One)
		{
			// Vector: IR+TX, n_ready, T0, RMW_T6, BRFW, n_C_OUT  (lsb -> msb)

			size_t n = 0;
			n |= core->ir->IROut;
			n |= ((size_t)core->TxBits << 8);
			n |= ((size_t)n_ready << 14);
			n |= ((size_t)T0 << 15);
			n |= ((size_t)RMW_T6 << 16);
			n |= ((size_t)BRFW << 17);
			n |= ((size_t)n_C_OUT << 18);

			CarryBCD_TempWire temp = temp_tab1[n];

			if (prev_temp1.bits != temp.bits)
			{
				acin_latch1.set(temp.n_ADL_ADD_Derived ? TriState::One : TriState::Zero, PHI2);
				acin_latch2.set(temp.INC_SB ? TriState::One : TriState::Zero, PHI2);
				acin_latch3.set(temp.BRX ? TriState::One : TriState::Zero, PHI2);
				acin_latch4.set(temp.CSET ? TriState::One : TriState::Zero, PHI2);
				prev_temp1.bits = temp.bits;
			}

			n_ADL_ADD = prev_temp1.n_ADL_ADD ? TriState::One : TriState::Zero;
			INC_SB = prev_temp1.INC_SB ? TriState::One : TriState::Zero;
			BRX = prev_temp1.BRX ? TriState::One : TriState::Zero;
		}
		else
		{
			TriState acin[4]{};
			acin[0] = acin_latch1.get();
			acin[1] = acin_latch2.get();
			acin[2] = acin_latch3.get();
			acin[3] = acin_latch4.get();
			TriState n_ACIN = NOR4(acin);
			acin_latch5.set(n_ACIN, PHI1);
		}

		TriState D_OUT = NOT(n_D_OUT);
		daa_latch1.set(NOT(NOR(NAND(d[52], D_OUT), SBC0)), PHI2);
		daa_latch2.set(daa_latch1.nget(), PHI1);

		dsa_latch1.set(NAND(SBC0, D_OUT), PHI2);
		dsa_latch2.set(dsa_latch1.nget(), PHI1);
	}

	void ALUControl::sim_ALUInput()
	{
		TriState* d = core->decoder_out;
		TriState PHI2 = core->wire.PHI2;
		TriState n_ready = core->wire.n_ready;
		TriState BRK6E = core->wire.BRK6E;

		TriState SBC0 = d[51];
		TriState JSR_5 = d[56];
		TriState JSR2 = d[48];
		TriState RET = d[47];

		// Setting the ALU input values (NDB/ADD, DB/ADD, 0/ADD, SB/ADD, ADL/ADD)

		if (PHI2 == TriState::One)
		{
			TriState n_NDB_ADD = NAND(OR3(BRX, JSR_5, SBC0), NOT(n_ready));
			ndbadd_latch.set(n_NDB_ADD, PHI2);
			dbadd_latch.set(NAND(n_NDB_ADD, n_ADL_ADD), PHI2);
			adladd_latch.set(n_ADL_ADD, PHI2);

			TriState sbadd[9]{};
			sbadd[0] = STKOP;
			sbadd[1] = d[30];
			sbadd[2] = d[31];
			sbadd[3] = d[45];
			sbadd[4] = JSR2;
			sbadd[5] = INC_SB;
			sbadd[6] = RET;
			sbadd[7] = BRK6E;
			sbadd[8] = n_ready;
			TriState SB_ADD = NOR9(sbadd);
			sbadd_latch.set(NOT(SB_ADD), PHI2);
			zadd_latch.set(SB_ADD, PHI2);
		}
	}

	void ALUControl::sim_ALUOps()
	{
		TriState* d = core->decoder_out;
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState n_ready = core->wire.n_ready;
		TriState RMW_T6 = core->wire.RMW_T6;

		TriState EOR = d[29];
		TriState _OR = NOT(NOR(d[32], n_ready));
		TriState _AND = NOT(NOR(d[69], d[70]));
		TriState SR = NOT(NOR(d[75], AND(d[76], RMW_T6)));

		// ALU operation commands (ANDS, EORS, ORS, SRS, SUMS)

		if (PHI2 == TriState::One)
		{
			ands_latch1.set(_AND, PHI2);
			eors_latch1.set(EOR, PHI2);
			ors_latch1.set(_OR, PHI2);
			srs_latch1.set(SR, PHI2);

			TriState sums[4];
			sums[0] = _AND;
			sums[1] = EOR;
			sums[2] = _OR;
			sums[3] = SR;
			sums_latch1.set(NOR4(sums), PHI2);
		}
		else
		{
			ands_latch2.set(ands_latch1.nget(), PHI1);
			eors_latch2.set(eors_latch1.nget(), PHI1);
			ors_latch2.set(ors_latch1.nget(), PHI1);
			srs_latch2.set(srs_latch1.nget(), PHI1);

			sums_latch2.set(sums_latch1.nget(), PHI1);
		}
	}

	void ALUControl::sim_ADDOut()
	{
		TriState* d = core->decoder_out;
		TriState PHI2 = core->wire.PHI2;
		TriState T1 = core->disp->getT1();
		TriState RMW_T7 = core->wire.RMW_T7;

		TriState RTS_5 = d[84];
		TriState RTI_5 = d[26];
		TriState JSR_5 = d[56];

		TriState BR0 = AND(d[73], NOT(core->wire.n_PRDY));
		TriState PGX = NAND(NOR(d[71], d[72]), NOT(BR0));

		// Control commands of the intermediate ALU result (ADD/SB06, ADD/SB7, ADD/ADL)

		if (PHI2 == TriState::One)
		{
			TriState sb06[5]{};
			sb06[0] = RMW_T7;
			sb06[1] = STKOP;
			sb06[2] = JSR_5;
			sb06[3] = T1;
			sb06[4] = PGX;
			TriState n_ADD_SB06 = NOR5(sb06);
			addsb06_latch.set(n_ADD_SB06, PHI2);

			addsb7_latch.set(NOR(n_ADD_SB06, n_ADD_SB7), PHI2);

			TriState noadl[7]{};
			noadl[0] = RTS_5;
			noadl[1] = d[85];
			noadl[2] = d[86];
			noadl[3] = d[87];
			noadl[4] = d[88];
			noadl[5] = d[89];
			noadl[6] = RTI_5;
			TriState NOADL = NOR7(noadl);
			addadl_latch.set(NOT(NOR(NOADL, PGX)), PHI2);
		}
	}

	CarryBCD_TempWire ALUControl::PreCalc1(uint8_t ir, bool n_T0, bool n_T1X, bool n_T2, bool n_T3, bool n_T4, bool n_T5,
		bool n_ready, bool T0, bool RMW_T6, bool BRFW, bool n_C_OUT)
	{
		TriState* d;
		DecoderInput decoder_in{};
		decoder_in.packed_bits = 0;
		CarryBCD_TempWire temp{};
		temp.bits = 0;

		TriState IR0 = ir & 0b00000001 ? TriState::One : TriState::Zero;
		TriState IR1 = ir & 0b00000010 ? TriState::One : TriState::Zero;
		TriState IR2 = ir & 0b00000100 ? TriState::One : TriState::Zero;
		TriState IR3 = ir & 0b00001000 ? TriState::One : TriState::Zero;
		TriState IR4 = ir & 0b00010000 ? TriState::One : TriState::Zero;
		TriState IR5 = ir & 0b00100000 ? TriState::One : TriState::Zero;
		TriState IR6 = ir & 0b01000000 ? TriState::One : TriState::Zero;
		TriState IR7 = ir & 0b10000000 ? TriState::One : TriState::Zero;

		decoder_in.n_IR0 = NOT(IR0);
		decoder_in.n_IR1 = NOT(IR1);
		decoder_in.IR01 = OR(IR0, IR1);
		decoder_in.n_IR2 = NOT(IR2);
		decoder_in.IR2 = IR2;
		decoder_in.n_IR3 = NOT(IR3);
		decoder_in.IR3 = IR3;
		decoder_in.n_IR4 = NOT(IR4);
		decoder_in.IR4 = IR4;
		decoder_in.n_IR5 = NOT(IR5);
		decoder_in.IR5 = IR5;
		decoder_in.n_IR6 = NOT(IR6);
		decoder_in.IR6 = IR6;
		decoder_in.n_IR7 = NOT(IR7);
		decoder_in.IR7 = IR7;

		decoder_in.n_T0 = n_T0;
		decoder_in.n_T1X = n_T1X;
		decoder_in.n_T2 = n_T2;
		decoder_in.n_T3 = n_T3;
		decoder_in.n_T4 = n_T4;
		decoder_in.n_T5 = n_T5;

		core->decoder->sim(decoder_in.packed_bits, &d);

		// Wires

		TriState RET = d[47];
		TriState BR3 = d[93];

		TriState adladd[7]{};
		adladd[0] = AND(d[33], NOT(d[34]));
		adladd[1] = d[35];	// STK2
		adladd[2] = d[36];
		adladd[3] = d[37];
		adladd[4] = d[38];
		adladd[5] = d[39];
		adladd[6] = n_ready ? TriState::One : TriState::Zero;
		temp.n_ADL_ADD = NOR7(adladd);

		TriState incsb[6]{};
		incsb[0] = d[39];
		incsb[1] = d[40];
		incsb[2] = d[41];
		incsb[3] = d[42];
		incsb[4] = d[43];
		incsb[5] = AND(RMW_T6 ? TriState::One : TriState::Zero, d[44]);
		temp.INC_SB = NOT(NOR6(incsb));

		temp.BRX = NOT(NOR3(d[49], d[50], NOR(NOT(BR3), BRFW ? TriState::One : TriState::Zero)));

		temp.CSET = NAND(NAND(NOR(n_C_OUT ? TriState::One : TriState::Zero, NOR(T0 ? TriState::One : TriState::Zero, RMW_T6 ? TriState::One : TriState::Zero)), OR(d[52], d[53])), NOT(d[54]));

		temp.n_ADL_ADD_Derived = NOR(temp.n_ADL_ADD ? TriState::One : TriState::Zero, NOT(RET));

		return temp;
	}

	void BusControl::sim()
	{
		TriState* d = core->decoder_out;
		TriState PHI2 = core->wire.PHI2;

		if (PHI2 == TriState::One)
		{
			TriState BR0 = AND(d[73], NOT(core->wire.n_PRDY));
			TriState _AND = NOT(NOR(d[69], d[70]));
			TriState RMW_T7 = core->wire.RMW_T7;

			TriState n_SB_X = NOR3(d[14], d[15], d[16]);
			TriState n_SB_Y = NOR3(d[18], d[19], d[20]);
			TriState n_SBXY = NAND(n_SB_X, n_SB_Y);

			TriState PGX = NAND(NOR(d[71], d[72]), NOT(BR0));

			TriState PHI1 = core->wire.PHI1;
			TriState STOR = core->disp->getSTOR(d);
			TriState STXY = NOR(AND(STOR, d[0]), AND(STOR, d[12]));
			TriState Z_ADL0 = core->cmd.Z_ADL0 ? TriState::One : TriState::Zero;
			TriState ACRL2 = core->wire.ACRL2;

			TriState JB = NOR3(d[94], d[95], d[96]);
			TriState DL_PCH = NOR(NOT(core->wire.T0), JB);

			TriState n_ready = core->wire.n_ready;

			TriState incsb[6]{};
			incsb[0] = d[39];
			incsb[1] = d[40];
			incsb[2] = d[41];
			incsb[3] = d[42];
			incsb[4] = d[43];
			incsb[5] = AND(core->wire.RMW_T6, d[44]);
			TriState INC_SB = NOT(NOR6(incsb));

			TriState BRK6E = core->wire.BRK6E;
			TriState T0 = core->wire.T0;
			TriState T1 = core->disp->getT1();
			TriState RMW_T6 = core->wire.RMW_T6;
			TriState IR0 = core->ir->IROut & 1 ? TriState::One : TriState::Zero;

			TriState n_DL_ADL = NOR(d[81], d[82]);
			TriState RTS_5 = d[84];
			TriState BR2 = d[80];
			TriState BR3 = d[93];
			TriState T2 = d[28];
			TriState JSR2 = d[48];
			TriState JSR_5 = d[56];
			TriState JMP_4 = d[101];
			TriState pp = d[129];
			TriState JSXY = NAND(NOT(JSR2), STXY);

			TriState ind[4]{};
			ind[0] = d[89];
			ind[1] = AND(d[90], NOT(pp));
			ind[2] = d[91];
			ind[3] = RTS_5;
			TriState IND = NOT(NOR4(ind));

			TriState IMPLIED = AND(d[128], NOT(pp));
			IMPLIED = AND(IMPLIED, NOT(IR0));
			TriState ABS_2 = AND(d[83], NOT(pp));
			TriState imp_abs = NOR(NOR(ABS_2, T0), IMPLIED);

			TriState nap[6]{};
			nap[0] = RTS_5;
			nap[1] = ABS_2;
			nap[2] = T0;
			nap[3] = T1;
			nap[4] = BR2;
			nap[5] = BR3;
			TriState n_ADH_PCH = NOR6(nap);
			TriState n_PCH_PCH = NOT(n_ADH_PCH);

			nready_latch.set(n_ready, PHI1);
			TriState n_SB_ADH = NOR(PGX, BR3);
			TriState SBA = NOR(n_SB_ADH, NAND(ACRL2, nready_latch.nget()));

			// External address bus control

			TriState n_ADL_ABL = NAND(NOR(RMW_T6, RMW_T7), NOR(NOT(NOR(d[71], d[72])), n_ready));
			adl_abl_latch.set(n_ADL_ABL, PHI2);

			TriState n1[4];
			n1[0] = IND;
			n1[1] = T2;
			n1[2] = n_PCH_PCH;
			n1[3] = JSR_5;
			TriState n_ADH_ABH = NOR(Z_ADL0, AND(OR(SBA, NOR(n_ready, NOR4(n1))), NOT(BR3)));
			adh_abh_latch.set(n_ADH_ABH, PHI2);

			// ALU connection to SB, DB buses (AC/DB, SB/AC, AC/SB)

			TriState sbac[7]{};
			sbac[0] = d[58];
			sbac[1] = d[59];
			sbac[2] = d[60];
			sbac[3] = d[61];
			sbac[4] = d[62];
			sbac[5] = d[63];
			sbac[6] = d[64];
			TriState n_SB_AC = NOR7(sbac);
			sb_ac_latch.set(n_SB_AC, PHI2);

			TriState acsb[5]{};
			acsb[0] = AND(NOT(d[64]), d[65]);
			acsb[1] = d[66];
			acsb[2] = d[67];
			acsb[3] = d[68];
			acsb[4] = _AND;
			TriState n_AC_SB = NOR5(acsb);
			ac_sb_latch.set(n_AC_SB, PHI2);

			ac_db_latch.set(NOR(d[74], AND(d[79], STOR)), PHI2);

			// Control of the SB, DB and ADH internal buses (SB/DB, SB/ADH, 0/ADH0, 0/ADH17)

			z_adh0_latch.set(n_DL_ADL, PHI2);
			z_adh17_latch.set(NOR(d[57], NOT(n_DL_ADL)), PHI2);

			TriState ztst[4]{};
			ztst[0] = n_SBXY;
			ztst[1] = NOT(n_SB_AC);
			ztst[2] = RMW_T7;
			ztst[3] = _AND;
			TriState n_ZTST = NOR4(ztst);

			TriState sbdb[6]{};
			sbdb[0] = NOT(NAND(RMW_T6, d[55]));
			sbdb[1] = NOR(n_ZTST, _AND);
			sbdb[2] = d[67];
			sbdb[3] = T1;
			sbdb[4] = BR2;
			sbdb[5] = JSXY;
			TriState n_SB_DB = NOR6(sbdb);
			sb_db_latch.set(n_SB_DB, PHI2);

			sb_adh_latch.set(n_SB_ADH, PHI2);

			// External data bus control

			dl_adh_latch.set(NOR(DL_PCH, IND), PHI2);
			dl_adl_latch.set(n_DL_ADL, PHI2);

			TriState n2[6]{};
			n2[0] = INC_SB;
			n2[1] = d[45];
			n2[2] = BRK6E;
			n2[3] = d[46];
			n2[4] = d[47];
			n2[5] = JSR2;

			TriState n3[5]{};
			n3[0] = BR2;
			n3[1] = imp_abs;
			n3[2] = NOT(NOR6(n2));
			n3[3] = JMP_4;
			n3[4] = RMW_T6;
			TriState n_DL_DB = NOR5(n3);
			dl_db_latch.set(n_DL_DB, PHI2);
		}

		// Outputs

		core->cmd.ADL_ABL = adl_abl_latch.nget();
		core->cmd.ADH_ABH = adh_abh_latch.nget();

		core->cmd.AC_DB = NOR(ac_db_latch.get(), PHI2);
		core->cmd.SB_AC = NOR(sb_ac_latch.get(), PHI2);
		core->cmd.AC_SB = NOR(ac_sb_latch.get(), PHI2);

		core->cmd.SB_DB = sb_db_latch.nget();
		core->cmd.SB_ADH = sb_adh_latch.nget();
		core->cmd.Z_ADH0 = z_adh0_latch.nget();
		core->cmd.Z_ADH17 = z_adh17_latch.nget();

		core->cmd.DL_ADL = dl_adl_latch.nget();
		core->cmd.DL_ADH = dl_adh_latch.nget();
		core->cmd.DL_DB = dl_db_latch.nget();
	}

	void PC_Control::sim()
	{
		TriState* d = core->decoder_out;
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState n_ready = core->wire.n_ready;

		// These two wires are used in the dispatcher, but are only needed during PHI2.

		TriState PC_DB = TriState::Zero;
		TriState n_ADL_PCL = TriState::Zero;

		if (PHI2 == TriState::One)
		{
			TriState T0 = core->wire.T0;
			TriState T1 = core->disp->getT1();
			TriState BR0 = AND(d[73], NOT(core->wire.n_PRDY));

			TriState JSR_5 = d[56];
			TriState RTS_5 = d[84];
			TriState pp = d[129];
			TriState BR2 = d[80];
			TriState BR3 = d[93];

			TriState ABS_2 = AND(d[83], NOT(pp));
			TriState JB = NOR3(d[94], d[95], d[96]);

			// DB

			TriState n_PCH_DB = NOR(d[77], d[78]);
			pch_db_latch1.set(n_PCH_DB, PHI2);
			TriState n_PCL_DB = pcl_db_latch1.nget();
			PC_DB = NAND(n_PCL_DB, n_PCH_DB);
			pcl_db_latch2.set(n_PCL_DB, PHI2);
			pch_db_latch2.set(n_PCH_DB, PHI2);

			// ADL

			TriState n1[5]{};
			n1[0] = T1;
			n1[1] = JSR_5;
			n1[2] = ABS_2;
			n1[3] = NOR(NOR(JB, nready_latch.get()), NOT(T0));
			n1[4] = BR2;
			TriState n_PCL_ADL = NOR5(n1);
			pcl_adl_latch.set(n_PCL_ADL, PHI2);

			TriState n2[4]{};
			n2[0] = NOT(n_PCL_ADL);
			n2[1] = RTS_5;
			n2[2] = T0;
			n2[3] = AND(NOT(nready_latch.get()), BR3);
			n_ADL_PCL = NOR4(n2);
			pcl_pcl_latch.set(NOT(n_ADL_PCL), PHI2);
			adl_pcl_latch.set(n_ADL_PCL, PHI2);

			// ADH

			TriState DL_PCH = NOR(NOT(T0), JB);
			TriState n_PCH_ADH = NOR(NOR3(n_PCL_ADL, DL_PCH, BR0), BR3);
			pch_adh_latch.set(n_PCH_ADH, PHI2);

			TriState n3[6]{};
			n3[0] = RTS_5;
			n3[1] = ABS_2;
			n3[2] = T0;
			n3[3] = T1;
			n3[4] = BR2;
			n3[5] = BR3;
			TriState n_ADH_PCH = NOR6(n3);
			adh_pch_latch.set(n_ADH_PCH, PHI2);
			TriState n_PCH_PCH = NOT(n_ADH_PCH);
			pch_pch_latch.set(n_PCH_PCH, PHI2);
		}
		else
		{
			// All you have to do during PHI1 is to update these 2 latches.

			nready_latch.set(n_ready, PHI1);
			pcl_db_latch1.set(NOR(pch_db_latch1.get(), n_ready), PHI1);
		}

		// Outputs

		core->cmd.PCL_DB = pcl_db_latch2.nget();
		core->cmd.PCH_DB = pch_db_latch2.nget();
		core->cmd.PCL_ADL = pcl_adl_latch.nget();
		core->cmd.PCH_ADH = pch_adh_latch.nget();

		if (PHI2 == TriState::One)
		{
			core->cmd.PCL_PCL = 0;
			core->cmd.ADL_PCL = 0;
			core->cmd.ADH_PCH = 0;
			core->cmd.PCH_PCH = 0;
		}
		else
		{
			core->cmd.PCL_PCL = NOT(pcl_pcl_latch.get());
			core->cmd.ADL_PCL = NOT(adl_pcl_latch.get());
			core->cmd.ADH_PCH = NOT(adh_pch_latch.get());
			core->cmd.PCH_PCH = NOT(pch_pch_latch.get());
		}

		// For dispatcher

		core->wire.PC_DB = PC_DB;
		core->wire.n_ADL_PCL = n_ADL_PCL;
	}

	// The dispatcher simulation is divided into three stages: before decoder, before random logic, and after random logic.

	// So far in this form (critical mass of code). In the process of debugging, it is possible to rearrange some sections.

	void Dispatcher::sim_BeforeDecoder()
	{
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState RDY = core->wire.RDY;
		TriState DORES = core->brk->getDORES();
		TriState ACR = core->alu->getACR();

		// Processor Readiness

		TriState WR = NOR3(NOT(ready_latch1.nget()), wr_latch.get(), DORES);
		ready_latch2.set(WR, PHI1);
		ready_latch1.set(NOR(RDY, ready_latch2.get()), PHI2);
		TriState n_ready = NOT(ready_latch1.nget());

		// Ready Delay

		rdydelay_latch1.set(n_ready, PHI1);
		rdydelay_latch2.set(rdydelay_latch1.nget(), PHI2);
		TriState NotReadyPhi1 = rdydelay_latch2.nget();

		// ACRL

		TriState ACRL1 = NOR(AND(NOT(ACR), NOT(NotReadyPhi1)), NOR(NOT(NotReadyPhi1), acr_latch2.nget()));
		acr_latch1.set(ACRL1, PHI1);
		acr_latch2.set(acr_latch1.nget(), PHI2);
		TriState ACRL2 = acr_latch2.nget();

		// Short Cycle Counter

		t1x_latch.set(NOR(t0_latch.get(), n_ready), PHI1);
		TriState n_T0 = NOR(NOR(comp_latch1.get(), AND(comp_latch2.get(), comp_latch3.get())), NOR(t0_latch.get(), t1x_latch.get()));
		t0_latch.set(n_T0, PHI2);
		TriState T0 = NOT(n_T0);
		TriState n_T1X = t1x_latch.nget();

		// Opcode Fetch

		TriState BRK6E = NOR(core->brk->getn_BRK6_LATCH2(), n_ready);
		TriState B_OUT = core->brk->getB_OUT(BRK6E);

		TriState T1 = t1_latch.nget();
		fetch_latch.set(T1, PHI2);
		TriState FETCH = NOR(fetch_latch.nget(), n_ready);
		TriState Z_IR = NAND(B_OUT, FETCH);

		// Outputs

		core->wire.T0 = T0;
		core->wire.n_T0 = n_T0;
		core->wire.n_T1X = n_T1X;
		core->wire.Z_IR = Z_IR;
		core->wire.FETCH = FETCH;
		core->wire.n_ready = n_ready;
		core->wire.ACRL1 = ACRL1;
		core->wire.ACRL2 = ACRL2;
	}

	void Dispatcher::sim_BeforeRandomLogic()
	{
		TriState* d = core->decoder_out;
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState n_ready = core->wire.n_ready;

		TriState n_SHIFT = NOR(d[106], d[107]);

		TriState memop_in[5]{};
		memop_in[0] = d[111];
		memop_in[1] = d[122];
		memop_in[2] = d[123];
		memop_in[3] = d[124];
		memop_in[4] = d[125];
		TriState n_MemOp = NOR5(memop_in);

		// T6 / T7 (RMW)

		t6_latch1.set(NOR(AND(t6_latch2.get(), n_ready), t67_latch.get()), PHI1);
		t7_latch2.set(t7_latch1.nget(), PHI1);
		t67_latch.set(NOR3(n_SHIFT, n_MemOp, n_ready), PHI2);
		t6_latch2.set(t6_latch1.nget(), PHI2);
		TriState RMW_T6 = t6_latch1.nget();
		t7_latch1.set(NAND(RMW_T6, NOT(n_ready)), PHI2);
		TriState RMW_T7 = NOT(t7_latch2.nget());

		core->wire.RMW_T6 = RMW_T6;
		core->wire.RMW_T7 = RMW_T7;
	}

	void Dispatcher::sim_AfterRandomLogic()
	{
		TriState* d = core->decoder_out;
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState BRK6E = core->wire.BRK6E;
		TriState RESP = core->wire.RESP;
		TriState ACR = core->alu->getACR();
		TriState BRFW = core->wire.BRFW;
		TriState n_BRTAKEN = core->wire.n_BRTAKEN;
		TriState n_TWOCYCLE = core->wire.n_TWOCYCLE;
		TriState n_IMPLIED = core->wire.n_IMPLIED;
		TriState PC_DB = core->wire.PC_DB;
		TriState n_ADL_PCL = core->wire.n_ADL_PCL;
		TriState n_ready = core->wire.n_ready;
		TriState T0 = core->wire.T0;
		TriState B_OUT = core->brk->getB_OUT(BRK6E);
		TriState RMW_T6 = core->wire.RMW_T6;
		TriState RMW_T7 = core->wire.RMW_T7;
		TriState ACRL1 = core->wire.ACRL1;
		TriState ACRL2 = core->wire.ACRL2;
		TriState RDY = core->wire.RDY;
		TriState DORES = core->wire.DORES;

		TriState BR2 = d[80];
		TriState BR3 = d[93];

		TriState n_SHIFT = NOR(d[106], d[107]);

		TriState n_STORE = NOT(d[97]);
		TriState REST = NAND(n_SHIFT, n_STORE);

		// Ready Delay (get)

		TriState NotReadyPhi1 = rdydelay_latch2.nget();

		// Increment PC

		br_latch1.set(NOR(AND(n_BRTAKEN, BR2), NOR(n_ADL_PCL, NOT(NOR(BR2, BR3)))), PHI2);
		br_latch2.set(NOR(NOT(BR3), NotReadyPhi1), PHI2);
		TriState ipc_temp = AND(XOR(BRFW, NOT(ACR)), NOT(br_latch2.nget()));
		ipc_latch1.set(B_OUT, PHI1);
		ipc_latch2.set(ipc_temp, PHI1);
		ipc_latch3.set(NOR3(n_ready, br_latch1.get(), NOT(n_IMPLIED)), PHI1);
		TriState n_1PC = NAND(ipc_latch1.get(), OR(ipc_latch2.get(), ipc_latch3.get()));

		// Step T1

		nready_latch.set(NOT(n_ready), PHI1);
		step_latch2.set(NOR(step_latch1.get(), ipc_temp), PHI1);
		step_latch1.set(NOR3(nready_latch.get(), RESP, step_latch2.get()), PHI2);

		// Instruction Completion

		TriState ENDS = NOR(ends_latch1.get(), ends_latch2.get());
		TriState n_TRES1 = NOR(NOR(NOR(step_latch1.get(), ipc_temp), n_ready), ENDS);
		t1_latch.set(n_TRES1, PHI1);
		TriState T1 = t1_latch.nget();
		ends_latch1.set(MUX(n_ready, NOR(T0, AND(n_BRTAKEN, BR2)), NOT(T1)), PHI2);
		ends_latch2.set(RESP, PHI2);
		TriState TRES1 = NOT(n_TRES1);

		tresx_latch1.set(NOR(d[91], d[92]), PHI2);

		TriState ENDX;
		TriState n_MemOp;

		if (PHI2)
		{
			TriState memop_in[5]{};
			memop_in[0] = d[111];
			memop_in[1] = d[122];
			memop_in[2] = d[123];
			memop_in[3] = d[124];
			memop_in[4] = d[125];
			n_MemOp = NOR5(memop_in);

			TriState endx_1[6]{};
			endx_1[0] = d[100];
			endx_1[1] = d[101];
			endx_1[2] = d[102];
			endx_1[3] = d[103];
			endx_1[4] = d[104];
			endx_1[5] = d[105];

			TriState endx_2[4]{};
			endx_2[0] = NOR3(d[96], NOT(n_SHIFT), n_MemOp);
			endx_2[1] = RMW_T7;
			endx_2[2] = NOT(NOR6(endx_1));
			endx_2[3] = BR3;

			ENDX = NOR4(endx_2);

			tresx_latch2.set(NOR3(RESP, ENDS, NOR(n_ready, ENDX)), PHI2);
		}
		else
		{
			ENDX = core->wire.ENDX;
		}

		TriState tresx_nor[4]{};
		tresx_nor[0] = ACRL1;
		tresx_nor[1] = tresx_latch1.get();
		tresx_nor[2] = n_ready;
		tresx_nor[3] = REST;
		TriState n_TRESX = NOR3(NOR4(tresx_nor), BRK6E, tresx_latch2.nget());
		tres2_latch.set(n_TRESX, PHI1);

		comp_latch1.set(TRES1, PHI1);
		comp_latch2.set(n_TWOCYCLE, PHI1);
		comp_latch3.set(n_TRESX, PHI1);

		// WR

		if (PHI2)
		{
			TriState STOR = NOR(n_MemOp, n_STORE);

			TriState wr_in[6]{};
			wr_in[0] = STOR;
			wr_in[1] = PC_DB;
			wr_in[2] = d[98];
			wr_in[3] = d[100];
			wr_in[4] = RMW_T6;
			wr_in[5] = RMW_T7;
			wr_latch.set(NOR6(wr_in), PHI2);
		}

		// Processor Readiness

		TriState WR = NOR3(NOT(ready_latch1.nget()), wr_latch.get(), DORES);
		ready_latch2.set(WR, PHI1);
		ready_latch1.set(NOR(RDY, ready_latch2.get()), PHI2);

		// Outputs

		core->wire.n_1PC = n_1PC;
		core->wire.WR = WR;

		core->wire.ENDS = ENDS;
		core->wire.ENDX = ENDX;
		core->wire.TRES1 = TRES1;
		core->wire.n_TRESX = n_TRESX;
	}

	TriState Dispatcher::getTRES2()
	{
		return tres2_latch.nget();
	}

	TriState Dispatcher::getT1()
	{
		return t1_latch.nget();
	}

	TriState Dispatcher::getSTOR(TriState d[])
	{
		TriState memop_in[5];
		memop_in[0] = d[111];
		memop_in[1] = d[122];
		memop_in[2] = d[123];
		memop_in[3] = d[124];
		memop_in[4] = d[125];
		TriState n_MemOp = NOR5(memop_in);

		TriState n_STORE = NOT(d[97]);
		TriState STOR = NOR(n_MemOp, n_STORE);

		return STOR;
	}

	FlagsControl::FlagsControl(M6502* parent)
	{
		core = parent;

		size_t maxVal = 1ULL << 16;
		for (size_t n = 0; n < maxVal; n++)
		{
			uint8_t ir = n & 0xff;
			bool n_T0 = (n >> 8) & 1;
			bool n_T1X = (n >> 9) & 1;
			bool n_T2 = (n >> 10) & 1;
			bool n_T3 = (n >> 11) & 1;
			bool n_T4 = (n >> 12) & 1;
			bool n_T5 = (n >> 13) & 1;
			bool T5 = (n >> 14) & 1;
			bool T6 = (n >> 15) & 1;

			temp_tab[n] = PreCalc(ir, n_T0, n_T1X, n_T2, n_T3, n_T4, n_T5, T5, T6);
		}

		prev_temp.bits = 0xff;
	}

	void FlagsControl::sim()
	{
		TriState* d = core->decoder_out;
		TriState PHI2 = core->wire.PHI2;
		TriState n_ready = core->wire.n_ready;
		TriState DB_P;

		if (PHI2 == TriState::One)
		{
			TriState RMW_T6 = core->wire.RMW_T6;
			TriState RMW_T7 = core->wire.RMW_T7;

			size_t n = 0;
			n |= core->ir->IROut;
			n |= ((size_t)core->TxBits << 8);
			n |= ((size_t)RMW_T6 << 14);
			n |= ((size_t)RMW_T7 << 15);

			FlagsControl_TempWire temp = temp_tab[n];

			// Latches

			if (prev_temp.bits != temp.bits)
			{
				pdb_latch.set(temp.n_POUT ? TriState::One : TriState::Zero, PHI2);
				acrc_latch.set(temp.n_ARIT ? TriState::One : TriState::Zero, PHI2);
				pin_latch.set(temp.n_PIN ? TriState::One : TriState::Zero, PHI2);
				prev_temp.bits = temp.bits;
			}

			iri_latch.set(NOT(d[108]), PHI2);
			irc_latch.set(NOT(d[110]), PHI2);
			ird_latch.set(NOT(d[120]), PHI2);
			zv_latch.set(NOT(d[127]), PHI2);
			dbz_latch.set(NOR3(acrc_latch.nget(), temp.ZTST ? TriState::One : TriState::Zero, d[109]), PHI2);
			dbn_latch.set(d[109], PHI2);
			DB_P = NOR(pin_latch.get(), n_ready);
			dbc_latch.set(NOR(DB_P, temp.SR ? TriState::One : TriState::Zero), PHI2);
			bit_latch.set(NOT(d[113]), PHI2);
		}
		else
		{
			DB_P = NOR(pin_latch.get(), n_ready);
		}

		// Outputs

		core->cmd.P_DB = pdb_latch.nget();
		core->cmd.IR5_I = iri_latch.nget();
		core->cmd.IR5_C = irc_latch.nget();
		core->cmd.IR5_D = ird_latch.nget();
		core->cmd.AVR_V = d[112];
		core->cmd.Z_V = zv_latch.nget();
		core->cmd.ACR_C = acrc_latch.nget();
		core->cmd.DBZ_Z = dbz_latch.nget();
		core->cmd.DB_N = NOR(AND(dbz_latch.get(), pin_latch.get()), dbn_latch.get());
		core->cmd.DB_P = DB_P;
		core->cmd.DB_C = dbc_latch.nget();
		core->cmd.DB_V = NAND(pin_latch.get(), bit_latch.get());
	}

	FlagsControl_TempWire FlagsControl::PreCalc(uint8_t ir, bool n_T0, bool n_T1X, bool n_T2, bool n_T3, bool n_T4, bool n_T5, bool T5, bool T6)
	{
		TriState* d;
		DecoderInput decoder_in{};
		decoder_in.packed_bits = 0;
		FlagsControl_TempWire temp{};
		temp.bits = 0;

		TriState IR0 = ir & 0b00000001 ? TriState::One : TriState::Zero;
		TriState IR1 = ir & 0b00000010 ? TriState::One : TriState::Zero;
		TriState IR2 = ir & 0b00000100 ? TriState::One : TriState::Zero;
		TriState IR3 = ir & 0b00001000 ? TriState::One : TriState::Zero;
		TriState IR4 = ir & 0b00010000 ? TriState::One : TriState::Zero;
		TriState IR5 = ir & 0b00100000 ? TriState::One : TriState::Zero;
		TriState IR6 = ir & 0b01000000 ? TriState::One : TriState::Zero;
		TriState IR7 = ir & 0b10000000 ? TriState::One : TriState::Zero;

		decoder_in.n_IR0 = NOT(IR0);
		decoder_in.n_IR1 = NOT(IR1);
		decoder_in.IR01 = OR(IR0, IR1);
		decoder_in.n_IR2 = NOT(IR2);
		decoder_in.IR2 = IR2;
		decoder_in.n_IR3 = NOT(IR3);
		decoder_in.IR3 = IR3;
		decoder_in.n_IR4 = NOT(IR4);
		decoder_in.IR4 = IR4;
		decoder_in.n_IR5 = NOT(IR5);
		decoder_in.IR5 = IR5;
		decoder_in.n_IR6 = NOT(IR6);
		decoder_in.IR6 = IR6;
		decoder_in.n_IR7 = NOT(IR7);
		decoder_in.IR7 = IR7;

		decoder_in.n_T0 = n_T0;
		decoder_in.n_T1X = n_T1X;
		decoder_in.n_T2 = n_T2;
		decoder_in.n_T3 = n_T3;
		decoder_in.n_T4 = n_T4;
		decoder_in.n_T5 = n_T5;

		core->decoder->sim(decoder_in.packed_bits, &d);

		// Wires

		TriState sbac[7]{};
		sbac[0] = d[58];
		sbac[1] = d[59];
		sbac[2] = d[60];
		sbac[3] = d[61];
		sbac[4] = d[62];
		sbac[5] = d[63];
		sbac[6] = d[64];
		TriState n_SB_AC = NOR7(sbac);

		TriState _AND = NOT(NOR(d[69], d[70]));

		TriState n_SB_X = NOR3(d[14], d[15], d[16]);
		TriState n_SB_Y = NOR3(d[18], d[19], d[20]);
		TriState n_SBXY = NAND(n_SB_X, n_SB_Y);

		TriState ztst[4]{};
		ztst[0] = n_SBXY;
		ztst[1] = NOT(n_SB_AC);
		ztst[2] = T6 ? TriState::One : TriState::Zero;
		ztst[3] = _AND;
		TriState n_ZTST = NOR4(ztst);

		temp.ZTST = NOT(n_ZTST);
		temp.SR = NOT(NOR(d[75], AND(d[76], T5 ? TriState::One : TriState::Zero)));

		temp.n_POUT = NOR(d[98], d[99]);
		temp.n_PIN = NOR(d[114], d[115]);

		TriState n1[6]{};
		n1[0] = AND(d[107], T6 ? TriState::One : TriState::Zero);
		n1[1] = d[112];		// AVR/V
		n1[2] = d[116];
		n1[3] = d[117];
		n1[4] = d[118];
		n1[5] = d[119];
		temp.n_ARIT = NOR6(n1);

		return temp;
	}

	void BranchLogic::sim()
	{
		TriState* d = core->decoder_out;
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState n_IR5 = core->wire.n_IR5;
		TriState n_C_OUT = core->random->flags->getn_C_OUT();
		TriState n_V_OUT = core->random->flags->getn_V_OUT();
		TriState n_N_OUT = core->random->flags->getn_N_OUT();
		TriState n_Z_OUT = core->random->flags->getn_Z_OUT();
		TriState DB7 = core->DB & 0x80 ? TriState::One : TriState::Zero;
		TriState BR2 = d[80];

		// BRTAKEN

		TriState n_IR6 = d[121];
		TriState n_IR7 = d[126];

		TriState res_C = NOR3(n_C_OUT, NOT(n_IR6), n_IR7);
		TriState res_V = NOR3(n_V_OUT, n_IR6, NOT(n_IR7));
		TriState res_N = NOR3(n_N_OUT, NOT(n_IR6), NOT(n_IR7));
		TriState res_Z = NOR3(n_Z_OUT, n_IR6, n_IR7);

		TriState in1[4]{};
		in1[0] = res_C;
		in1[1] = res_V;
		in1[2] = res_N;
		in1[3] = res_Z;
		TriState n_BRTAKEN = XOR(NOR4(in1), n_IR5);

		// BRFW

		TriState n_DB7 = DB7 == TriState::Z ? TriState::One : NOT(DB7);
		brfw_latch1.set(NOT(MUX(br2_latch.get(), brfw_latch2.get(), n_DB7)), PHI1);
		br2_latch.set(BR2, PHI2);
		brfw_latch2.set(brfw_latch1.nget(), PHI2);
		TriState BRFW = NOT(brfw_latch1.nget());

		core->wire.n_BRTAKEN = n_BRTAKEN;
		core->wire.BRFW = BRFW;
	}

	TriState BranchLogic::getBRFW()
	{
		return NOT(brfw_latch1.nget());
	}

	RandomLogic::RandomLogic(M6502* parent)
	{
		core = parent;
		regs_control = new RegsControl(core);
		alu_control = new ALUControl(core);
		pc_control = new PC_Control(core);
		bus_control = new BusControl(core);
		flags_control = new FlagsControl(core);
		flags = new Flags(core);
		branch_logic = new BranchLogic(core);
	}

	RandomLogic::~RandomLogic()
	{
		delete regs_control;
		delete alu_control;
		delete pc_control;
		delete bus_control;
		delete flags_control;
		delete flags;
		delete branch_logic;
	}

	void RandomLogic::sim()
	{
		// Register control

		regs_control->sim();

		// ALU control

		alu_control->sim();

		// Program counter (PC) control

		pc_control->sim();

		// Bus control

		bus_control->sim();

		// Flags control logic

		flags_control->sim();

		// The processing of loading flags has moved to the bottom part.

		// Conditional branch logic

		branch_logic->sim();
	}

	void AddressBus::sim_ConstGen()
	{
		bool Z_ADL0 = core->cmd.Z_ADL0;
		bool Z_ADL1 = core->cmd.Z_ADL1;
		bool Z_ADL2 = core->cmd.Z_ADL2;
		bool Z_ADH0 = core->cmd.Z_ADH0;
		bool Z_ADH17 = core->cmd.Z_ADH17;

		if (Z_ADL0)
		{
			core->ADL &= ~0b00000001;
		}

		if (Z_ADL1)
		{
			core->ADL &= ~0b00000010;
		}

		if (Z_ADL2)
		{
			core->ADL &= ~0b00000100;
		}

		if (Z_ADH0)
		{
			core->ADH &= ~0b00000001;
		}

		if (Z_ADH17)
		{
			core->ADH &= ~0b11111110;
		}
	}

	void AddressBus::sim_Output(uint16_t* addr_bus)
	{
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		bool ADL_ABL = core->cmd.ADL_ABL;
		bool ADH_ABH = core->cmd.ADH_ABH;

		// The address bus is set during PHI1 only

		if (PHI1 == TriState::One && ADL_ABL)
		{
			ABL = core->ADL;
		}

		if (PHI1 == TriState::One && ADH_ABH)
		{
			ABH = core->ADH;
		}

		uint16_t addr = ((uint16_t)ABH << 8) | ABL;
		*addr_bus = addr;
	}

	uint8_t AddressBus::getABL()
	{
		return ABL;
	}

	uint8_t AddressBus::getABH()
	{
		return ABH;
	}

	void AddressBus::setABL(uint8_t val)
	{
		ABL = val;
	}

	void AddressBus::setABH(uint8_t val)
	{
		ABH = val;
	}

	void Regs::sim_LoadSB()
	{
		TriState PHI2 = core->wire.PHI2;
		bool SB_Y = core->cmd.SB_Y;
		bool SB_X = core->cmd.SB_X;
		bool SB_S = core->cmd.SB_S;
		bool S_S = core->cmd.S_S;

		if (PHI2 == TriState::One)
		{
			S_out = ~S_in;
		}
		else
		{
			if (SB_Y)
			{
				Y = core->SB;
			}

			if (SB_X)
			{
				X = core->SB;
			}

			if (S_S)
			{
				S_in = ~S_out;
			}

			if (SB_S)
			{
				S_in = core->SB;
			}
		}
	}

	void Regs::sim_StoreSB()
	{
		TriState PHI2 = core->wire.PHI2;
		bool Y_SB = core->cmd.Y_SB;
		bool X_SB = core->cmd.X_SB;
		bool S_SB = core->cmd.S_SB;

		if (PHI2)
		{
			S_out = ~S_in;
		}

		if (S_SB)
		{
			if (core->SB_Dirty)
			{
				core->SB = core->SB & (~S_out);
			}
			else
			{
				core->SB = ~S_out;
				core->SB_Dirty = true;
			}
		}

		if (Y_SB)
		{
			if (core->SB_Dirty)
			{
				core->SB = core->SB & Y;
			}
			else
			{
				core->SB = Y;
				core->SB_Dirty = true;
			}
		}

		if (X_SB)
		{
			if (core->SB_Dirty)
			{
				core->SB = core->SB & X;
			}
			else
			{
				core->SB = X;
				core->SB_Dirty = true;
			}
		}
	}

	void Regs::sim_StoreOldS()
	{
		bool S_ADL = core->cmd.S_ADL;

		if (S_ADL)
		{
			if (core->ADL_Dirty)
			{
				core->ADL = core->ADL & ~S_out;
			}
			else
			{
				core->ADL = ~S_out;
				core->ADL_Dirty = true;
			}
		}
	}

	uint8_t Regs::getY()
	{
		return Y;
	}

	uint8_t Regs::getX()
	{
		return X;
	}

	uint8_t Regs::getS()
	{
		return ~S_out;
	}

	void Regs::setY(uint8_t val)
	{
		Y = val;
	}

	void Regs::setX(uint8_t val)
	{
		X = val;
	}

	void Regs::setS(uint8_t val)
	{
		S_out = ~val;
	}

	void ALU::sim()
	{
		TriState PHI2 = core->wire.PHI2;
		bool ANDS = core->cmd.ANDS;
		bool EORS = core->cmd.EORS;
		bool ORS = core->cmd.ORS;
		bool SRS = core->cmd.SRS;
		bool SUMS = core->cmd.SUMS;
		bool SB_AC = core->cmd.SB_AC;
		TriState n_ACIN = core->cmd.n_ACIN ? TriState::One : TriState::Zero;
		TriState n_DAA = core->cmd.n_DAA ? TriState::One : TriState::Zero;
		TriState n_DSA = core->cmd.n_DSA ? TriState::One : TriState::Zero;

		if (BCD_Hack)
		{
			n_DAA = n_DSA = TriState::One;
		}

		// Computational Part

		// The computational part saves its result on ADD only during PHI2.
		// The previous result from ADD (+BCD correction) is saved to the accumulator during PHI1.
		// This is done so that during PHI1 you can load new operands and simultaneously save the previous calculation result to AC.

		if (PHI2 == TriState::One)
		{
			TriState nors[8]{}, nands[8]{}, eors[8]{}, sums[8]{}, n_res[8] = { TriState::Z };
			TriState carry[8]{};		// Inverted carry chain. Even bits in direct logic, odd bits in inverted logic.
			TriState DC3, DC7;
			TriState n4[4]{}, t1, t2;

			TriState cin = n_ACIN;

			for (size_t n = 0; n < 8; n++)
			{
				TriState AIn = AI & (1 << n) ? TriState::One : TriState::Zero;
				TriState BIn = BI & (1 << n) ? TriState::One : TriState::Zero;

				nands[n] = NAND(AIn, BIn);
				nors[n] = NOR(AIn, BIn);
				eors[n] = XOR(AIn, BIn);		// In a real processor, eors alternate with enors to calculate sums. But we will omit that.

				if (n & 1)
				{
					// Odd
					carry[n] = AND(NAND(cin, NOT(nors[n])), NOT(NOT(nands[n])));
					sums[n] = XOR(NOT(cin), eors[n]);
				}
				else
				{
					// Even
					carry[n] = AND(NAND(cin, nands[n]), NOT(nors[n]));
					sums[n] = XOR(NOT(cin), NOT(eors[n]));
				}

				// Fast BCD Carry

				if (n == 3 && !BCD_Hack)
				{
#if 0 // ttlworks
					TriState Ci1_ = AND(NOT(nors[0]), OR(NOT(n_ACIN), NOT(nands[0])));
					t1 = AND3(Ci1_, NOT(nors[2]), NOT(nands[1]));
					n4[0] = Ci1_;
					n4[1] = NOT(nands[1]);
					n4[2] = eors[1];
					n4[3] = eors[2];
					t2 = OR(NOT(nands[2]), eors[3]);
					DC3 = AND(NOT(n_DAA), OR(t1, AND(NOT(NOR4(n4)), t2)));
#else
					n4[0] = AND(NAND(n_ACIN, nands[0]), NOT(nors[0]));
					n4[1] = NOR(NOT(nands[2]), nors[2]);
					n4[2] = NOT(nands[1]);
					n4[3] = eors[1];
					t1 = NOR(NOR(NOT(nands[2]), eors[3]), NOR4(n4));
					t2 = NOR(nors[2], NAND(NOT(nands[1]), n4[0]));
					DC3 = AND(OR(t1, t2), NOT(n_DAA));
#endif

					carry[3] = AND(carry[3], NOT(DC3));
				}

				if (n == 7)
				{
					if (!BCD_Hack)
					{
#if 0 // ttlworks
						t1 = AND3(carry[4], NOT(nands[5]), eors[6]);
						n4[0] = carry[4];
						n4[1] = NOT(nands[5]);
						n4[2] = eors[6];
						n4[3] = eors[5];
						t2 = OR(NOT(nands[6]), eors[7]);
						DC7 = AND(NOT(n_DAA), OR(t1, AND(NOT(NOR4(n4)), t2)));
#else
						n4[0] = carry[4];
						n4[1] = NOT(nands[5]);
						n4[2] = eors[5];
						n4[3] = eors[6];
						t1 = NOR(NOR(eors[7], NOT(nands[6])), NOR4(n4));
						t2 = NOR(NOT(eors[6]), NAND(NOT(nands[5]), carry[4]));
						DC7 = AND(OR(t1, t2), NOT(n_DAA));
#endif
					}
					else
					{
						DC7 = TriState::Zero;
					}
				}

				cin = carry[n];
			}

			for (size_t n = 0; n < 8; n++)
			{
				// The above random logic is arranged so that these commands are singleton (only one of them can be set).

				// Sometimes the processor goes "crazy" and this is possible (for example after Power Up)

#if 0
				size_t checkSum = (size_t)ANDS + (size_t)EORS + (size_t)ORS + (size_t)SRS + (size_t)SUMS;
				if (checkSum > 1)
				{
					throw "When simulating ALU Control, something went wrong. There should only be one ALU operation command.";
				}
#endif

				if (ANDS == TriState::One)
				{
					n_res[n] = nands[n];
				}
				if (EORS == TriState::One)
				{
					n_res[n] = NOT(eors[n]);
				}
				if (ORS == TriState::One)
				{
					n_res[n] = nors[n];
				}
				if (SRS == TriState::One)
				{
					if (n != 7)
					{
						n_res[n] = nands[n + 1];
					}
					else
					{
						n_res[n] = TriState::One;
					}
				}
				if (SUMS == TriState::One)
				{
					n_res[n] = sums[n];
				}

				// Technically it may happen that no ALU operation has been set and `res` is in floating state.
				// In this case the value of the latch does not change.

				n_ADD = 0;
				for (size_t n = 0; n < 8; n++)
				{
					if (n_res[n])
					{
						n_ADD |= 1 << n;
					}
				}
			}

			// ACR, AVR

			DCLatch.set(DC7, PHI2);
			ACLatch.set(NOT(carry[7]), PHI2);
			AVRLatch.set(NOR(NOR(carry[6], nands[7]), AND(carry[6], nors[7])), PHI2);		// AVR = C6 ^ C7

			if (!BCD_Hack)
			{
				daal_latch.set(NAND(NOT(n_DAA), NOT(carry[3])), PHI2);
				daah_latch.set(NOT(n_DAA), PHI2);
				dsal_latch.set(NOR(NOT(carry[3]), n_DSA), PHI2);
				dsah_latch.set(NOT(n_DSA), PHI2);
			}

			//AC[n].set(NOT(AC[n].nget()), PHI2);
		}
		else
		{
			TriState ACR = NOT(NOR(DCLatch.get(), ACLatch.get()));
			TriState AVR = AVRLatch.nget();

			// BCD Correction

			if (!BCD_Hack)
			{
				TriState bcd_out[8]{};

				TriState DAAL = daal_latch.nget();
				TriState DAAH = NOR(NOT(ACR), daah_latch.nget());
				TriState DSAL = dsal_latch.get();
				TriState DSAH = NOR(ACR, dsah_latch.nget());

				TriState SB0 = core->SB & 0b00000001 ? TriState::One : TriState::Zero;
				TriState SB1 = core->SB & 0b00000010 ? TriState::One : TriState::Zero;
				TriState SB2 = core->SB & 0b00000100 ? TriState::One : TriState::Zero;
				TriState SB3 = core->SB & 0b00001000 ? TriState::One : TriState::Zero;
				TriState SB4 = core->SB & 0b00010000 ? TriState::One : TriState::Zero;
				TriState SB5 = core->SB & 0b00100000 ? TriState::One : TriState::Zero;
				TriState SB6 = core->SB & 0b01000000 ? TriState::One : TriState::Zero;
				TriState SB7 = core->SB & 0b10000000 ? TriState::One : TriState::Zero;

				TriState n_ADD0 = n_ADD & 0b00000001 ? TriState::One : TriState::Zero;
				TriState n_ADD1 = n_ADD & 0b00000010 ? TriState::One : TriState::Zero;
				TriState n_ADD2 = n_ADD & 0b00000100 ? TriState::One : TriState::Zero;
				TriState n_ADD3 = n_ADD & 0b00001000 ? TriState::One : TriState::Zero;
				TriState n_ADD4 = n_ADD & 0b00010000 ? TriState::One : TriState::Zero;
				TriState n_ADD5 = n_ADD & 0b00100000 ? TriState::One : TriState::Zero;
				TriState n_ADD6 = n_ADD & 0b01000000 ? TriState::One : TriState::Zero;
				TriState n_ADD7 = n_ADD & 0b10000000 ? TriState::One : TriState::Zero;

				bcd_out[0] = SB0;
				bcd_out[1] = XOR(NOR(DSAL, DAAL), NOT(SB1));
				bcd_out[2] = XOR(AND(NAND(n_ADD1, DAAL), NAND(NOT(n_ADD1), DSAL)), NOT(SB2));
				bcd_out[3] = XOR(AND(NAND(NOT(NOR(n_ADD1, n_ADD2)), DSAL), NAND(NAND(n_ADD1, n_ADD2), DAAL)), NOT(SB3));

				bcd_out[4] = SB4;
				bcd_out[5] = XOR(NOR(DAAH, DSAH), NOT(SB5));
				bcd_out[6] = XOR(AND(NAND(n_ADD5, DAAH), NAND(NOT(n_ADD5), DSAH)), NOT(SB6));
				bcd_out[7] = XOR(AND(NAND(NAND(n_ADD5, n_ADD6), DAAH), NAND(NOT(NOR(n_ADD5, n_ADD6)), DSAH)), NOT(SB7));

				// BCD correction via SB bus: SB_AC

				if (SB_AC)
				{
					AC = 0;

					for (size_t n = 0; n < 8; n++)
					{
						if (bcd_out[n])
						{
							AC |= (1 << n);
						}
					}
				}
			}
			else
			{
				if (SB_AC)
				{
					AC = core->SB;
				}
			}
		}
	}

	void ALU::sim_HLE()
	{
		if (!BCD_Hack)
		{
			sim();
			return;
		}

		TriState PHI2 = core->wire.PHI2;
		bool ANDS = core->cmd.ANDS;
		bool EORS = core->cmd.EORS;
		bool ORS = core->cmd.ORS;
		bool SRS = core->cmd.SRS;
		bool SUMS = core->cmd.SUMS;
		bool SB_AC = core->cmd.SB_AC;
		TriState n_ACIN = core->cmd.n_ACIN ? TriState::One : TriState::Zero;

		if (PHI2 == TriState::One)
		{
			int resInt = 0;
			uint8_t res = 0;
			bool saveRes = false;

			if (ANDS == TriState::One)
			{
				res = AI & BI;
				saveRes = true;
			}
			if (EORS == TriState::One)
			{
				res = AI ^ BI;
				saveRes = true;
			}
			if (ORS == TriState::One)
			{
				res = AI | BI;
				saveRes = true;
			}
			if (SRS == TriState::One)
			{
				res = (AI & BI) >> 1;
				saveRes = true;
			}
			if (SUMS == TriState::One)
			{
				resInt = (uint16_t)AI + (uint16_t)BI + (n_ACIN == TriState::Zero ? 1 : 0);
				res = (uint8_t)resInt;
				saveRes = true;
			}

			if (saveRes)
			{
				n_ADD = ~res;
			}

			// ACR, AVR

			DCLatch.set(TriState::Zero, PHI2);
			ACLatch.set((resInt >> 8) != 0 ? TriState::One : TriState::Zero, PHI2);
			AVRLatch.set((~(AI ^ BI) & (AI ^ resInt) & 0x80) != 0 ? TriState::Zero : TriState::One, PHI2);
		}
		else
		{
			if (SB_AC)
			{
				AC = core->SB;
			}
		}
	}

	void ALU::sim_BusMux()
	{
		bool SB_DB = core->cmd.SB_DB;
		bool SB_ADH = core->cmd.SB_ADH;

		// This is where the very brainy construction related to the connection of the busses is located.

		if (SB_DB)
		{
			if (core->SB_Dirty && !core->DB_Dirty)
			{
				core->DB = core->SB;
				core->DB_Dirty = true;
			}
			else if (!core->SB_Dirty && core->DB_Dirty)
			{
				core->SB = core->DB;
				core->SB_Dirty = true;
			}
			else if (!core->SB_Dirty && !core->DB_Dirty)
			{
				// z
			}
			else
			{
				uint8_t tmp = core->SB & core->DB;
				core->SB = tmp;
				core->DB = tmp;
			}
		}

		if (SB_ADH)
		{
			if (core->SB_Dirty && !core->ADH_Dirty)
			{
				core->ADH = core->SB;
				core->ADH_Dirty = true;
			}
			else if (!core->SB_Dirty && core->ADH_Dirty)
			{
				core->SB = core->ADH;
				core->SB_Dirty = true;
			}
			else if (!core->SB_Dirty && !core->ADH_Dirty)
			{
				// z
			}
			else
			{
				uint8_t tmp = core->SB & core->ADH;
				core->SB = tmp;
				core->ADH = tmp;
			}
		}
	}

	void ALU::sim_Load()
	{
		bool NDB_ADD = core->cmd.NDB_ADD;
		bool DB_ADD = core->cmd.DB_ADD;
		bool Z_ADD = core->cmd.Z_ADD;
		bool SB_ADD = core->cmd.SB_ADD;
		bool ADL_ADD = core->cmd.ADL_ADD;

		// Special case when SB/ADD and 0/ADD are active at the same time (this only happens with Power Up).

		if (SB_ADD == TriState::One && Z_ADD == TriState::One)
		{
			core->SB = 0;
			core->SB_Dirty = true;
		}

		// ALU input value loading commands are active only during PHI1.
		// Although the ALU "counts", its output value is not saved on ADD. Saving to ADD happens during PHI2 (when all operand loading commands are inactive).

		if (SB_ADD)
		{
			AI = core->SB;
		}
		if (Z_ADD)
		{
			AI = 0;
		}
		if (DB_ADD)
		{
			BI = core->DB;
		}
		if (NDB_ADD)
		{
			BI = ~core->DB;
		}
		if (ADL_ADD)
		{
			BI = core->ADL;
		}
	}

	void ALU::sim_StoreADD()
	{
		bool ADD_SB06 = core->cmd.ADD_SB06;
		bool ADD_SB7 = core->cmd.ADD_SB7;
		bool ADD_ADL = core->cmd.ADD_ADL;

		// Intermediate Result (ADD) Output

		if (ADD_ADL)
		{
			if (core->ADL_Dirty)
			{
				core->ADL = core->ADL & ~n_ADD;
			}
			else
			{
				core->ADL = ~n_ADD;
				core->ADL_Dirty = true;
			}
		}

		if (ADD_SB06)
		{
			core->SB &= 0x80;
			core->SB |= ~n_ADD & 0x7f;
			core->SB_Dirty = true;
		}

		if (ADD_SB7)
		{
			core->SB &= ~0x80;
			core->SB |= ~n_ADD & 0x80;
			core->SB_Dirty = true;
		}
	}

	void ALU::sim_StoreAC()
	{
		bool AC_SB = core->cmd.AC_SB;
		bool AC_DB = core->cmd.AC_DB;

		// Accumulator (AC) Output

		if (AC_SB)
		{
			if (core->SB_Dirty)
			{
				core->SB = core->SB & AC;
			}
			else
			{
				core->SB = AC;
				core->SB_Dirty = true;
			}
		}

		if (AC_DB)
		{
			if (core->DB_Dirty)
			{
				core->DB = core->DB & AC;
			}
			else
			{
				core->DB = AC;
				core->DB_Dirty = true;
			}
		}
	}

	TriState ALU::getACR()
	{
		return (NOT(NOR(DCLatch.get(), ACLatch.get())));
	}

	TriState ALU::getAVR()
	{
		return AVRLatch.nget();
	}

	void ALU::setAVR(TriState val)
	{
		AVRLatch.set(NOT(val), TriState::One);
	}

	uint8_t ALU::getAI()
	{
		return AI;
	}

	uint8_t ALU::getBI()
	{
		return BI;
	}

	uint8_t ALU::getADD()
	{
		return ~n_ADD;
	}

	uint8_t ALU::getAC()
	{
		return AC;
	}

	void ALU::setAI(uint8_t val)
	{
		AI = val;
	}

	void ALU::setBI(uint8_t val)
	{
		BI = val;
	}

	void ALU::setADD(uint8_t val)
	{
		n_ADD = ~val;
	}

	void ALU::setAC(uint8_t val)
	{
		AC = val;
	}

	void ProgramCounter::sim_EvenBit(TriState PHI2, TriState cin, TriState& cout, TriState& sout, size_t n, DLatch PCx[], DLatch PCxS[])
	{
		sout = PCxS[n].nget();
		cout = NOR(cin, sout);
		PCx[n].set(NOR(AND(sout, cin), cout), PHI2);
	}

	void ProgramCounter::sim_OddBit(TriState PHI2, TriState cin, TriState& cout, TriState& sout, size_t n, DLatch PCx[], DLatch PCxS[])
	{
		sout = PCxS[n].nget();
		cout = NAND(cin, NOT(sout));
		PCx[n].set(NAND(OR(NOT(sout), cin), cout), PHI2);
	}

	void ProgramCounter::sim()
	{
		if (HLE)
		{
			sim_HLE();
			return;
		}

		TriState PHI2 = core->wire.PHI2;
		TriState n_1PC = core->wire.n_1PC;

		if (PHI2 != TriState::One)
		{
			// The quick way out. Nothing is saved anywhere during PHI1.

			return;
		}

		// PCL

		TriState cin = n_1PC;
		TriState souts[9]{};
		souts[0] = n_1PC;

		for (size_t n = 0; n < 8; n++)
		{
			if (n & 1)
			{
				sim_OddBit(PHI2, cin, cin, souts[n + 1], n, PCL, PCLS);
			}
			else
			{
				sim_EvenBit(PHI2, cin, cin, souts[n + 1], n, PCL, PCLS);
			}
		}

		TriState PCLC = NOR9(souts);

		// PCH

		// The circuits for the even bits (0, 2, ...) of the PCH repeat the circuits for the odd bits (1, 3, ...) of the PCL.
		// Similarly, circuits for odd bits (1, 3, ...) of PCH repeat circuits for even bits (0, 2, ...) of PCL.

		cin = PCLC;
		TriState pchc[5]{};
		pchc[0] = NOT(PCLC);

		for (size_t n = 0; n < 4; n++)
		{
			if (n & 1)
			{
				sim_EvenBit(PHI2, cin, cin, pchc[n + 1], n, PCH, PCHS);
			}
			else
			{
				sim_OddBit(PHI2, cin, cin, pchc[n + 1], n, PCH, PCHS);
			}
		}

		TriState PCHC = NOR5(pchc);
		cin = PCHC;
		TriState bogus;

		for (size_t n = 4; n < 8; n++)
		{
			if (n & 1)
			{
				sim_EvenBit(PHI2, cin, cin, bogus, n, PCH, PCHS);
			}
			else
			{
				sim_OddBit(PHI2, cin, cin, bogus, n, PCH, PCHS);
			}
		}
	}

	void ProgramCounter::sim_HLE()
	{
		TriState PHI2 = core->wire.PHI2;
		TriState n_1PC = core->wire.n_1PC;

		if (PHI2 == TriState::One)
		{
			uint16_t pc = ((uint16_t)PackedPCHS << 8) | PackedPCLS;

			if (n_1PC == TriState::Zero)
			{
				pc++;
			}

			PackedPCL = pc & 0xff;
			PackedPCH = pc >> 8;
		}
	}

	void ProgramCounter::sim_Load()
	{
		if (HLE)
		{
			sim_LoadHLE();
			return;
		}

		TriState ADL_PCL = core->cmd.ADL_PCL ? TriState::One : TriState::Zero;
		TriState PCL_PCL = core->cmd.PCL_PCL ? TriState::One : TriState::Zero;
		TriState ADH_PCH = core->cmd.ADH_PCH ? TriState::One : TriState::Zero;
		TriState PCH_PCH = core->cmd.PCH_PCH ? TriState::One : TriState::Zero;

		if (PCL_PCL || PCH_PCH)
		{
			for (size_t n = 0; n < 8; n++)
			{
				if (n & 1)
				{
					PCLS[n].set(PCL[n].nget(), PCL_PCL);
					PCHS[n].set(NOT(PCH[n].nget()), PCH_PCH);
				}
				else
				{
					PCLS[n].set(NOT(PCL[n].nget()), PCL_PCL);
					PCHS[n].set(PCH[n].nget(), PCH_PCH);
				}
			}
		}

		if (ADL_PCL || ADH_PCH)
		{
			for (size_t n = 0; n < 8; n++)
			{
				PCLS[n].set(core->ADL & (1 << n) ? TriState::One : TriState::Zero, ADL_PCL);
				PCHS[n].set(core->ADH & (1 << n) ? TriState::One : TriState::Zero, ADH_PCH);
			}
		}
	}

	void ProgramCounter::sim_Store()
	{
		if (HLE)
		{
			sim_StoreHLE();
			return;
		}

		bool PCL_ADL = core->cmd.PCL_ADL;
		bool PCL_DB = core->cmd.PCL_DB;
		bool PCH_ADH = core->cmd.PCH_ADH;
		bool PCH_DB = core->cmd.PCH_DB;

		if (PCL_DB || PCL_ADL)
		{
			uint8_t out = 0;

			for (size_t n = 0; n < 8; n++)
			{
				if (n & 1)
				{
					out |= PCL[n].nget() << n;
				}
				else
				{
					out |= NOT(PCL[n].nget()) << n;
				}
			}

			if (PCL_DB)
			{
				if (core->DB_Dirty)
				{
					core->DB = core->DB & out;
				}
				else
				{
					core->DB = out;
					core->DB_Dirty = true;
				}
			}

			if (PCL_ADL)
			{
				if (core->ADL_Dirty)
				{
					core->ADL = core->ADL & out;
				}
				else
				{
					core->ADL = out;
					core->ADL_Dirty = true;
				}
			}
		}

		if (PCH_DB || PCH_ADH)
		{
			uint8_t out = 0;

			for (size_t n = 0; n < 8; n++)
			{
				if (n & 1)
				{
					out |= NOT(PCH[n].nget()) << n;
				}
				else
				{
					out |= PCH[n].nget() << n;
				}
			}

			if (PCH_DB)
			{
				if (core->DB_Dirty)
				{
					core->DB = core->DB & out;
				}
				else
				{
					core->DB = out;
					core->DB_Dirty = true;
				}
			}

			if (PCH_ADH)
			{
				if (core->ADH_Dirty)
				{
					core->ADH = core->ADH & out;
				}
				else
				{
					core->ADH = out;
					core->ADH_Dirty = true;
				}
			}
		}
	}

	void ProgramCounter::sim_LoadHLE()
	{
		bool ADL_PCL = core->cmd.ADL_PCL;
		bool PCL_PCL = core->cmd.PCL_PCL;
		bool ADH_PCH = core->cmd.ADH_PCH;
		bool PCH_PCH = core->cmd.PCH_PCH;

		if (PCL_PCL)
		{
			PackedPCLS = PackedPCL;
		}

		if (PCH_PCH)
		{
			PackedPCHS = PackedPCH;
		}

		if (ADL_PCL)
		{
			PackedPCLS = core->ADL;
		}

		if (ADH_PCH)
		{
			PackedPCHS = core->ADH;
		}
	}

	void ProgramCounter::sim_StoreHLE()
	{
		bool PCL_ADL = core->cmd.PCL_ADL;
		bool PCL_DB = core->cmd.PCL_DB;
		bool PCH_ADH = core->cmd.PCH_ADH;
		bool PCH_DB = core->cmd.PCH_DB;

		if (PCL_DB)
		{
			if (core->DB_Dirty)
			{
				core->DB = core->DB & PackedPCL;
			}
			else
			{
				core->DB = PackedPCL;
				core->DB_Dirty = true;
			}
		}

		if (PCL_ADL)
		{
			if (core->ADL_Dirty)
			{
				core->ADL = core->ADL & PackedPCL;
			}
			else
			{
				core->ADL = PackedPCL;
				core->ADL_Dirty = true;
			}
		}

		if (PCH_DB)
		{
			if (core->DB_Dirty)
			{
				core->DB = core->DB & PackedPCH;
			}
			else
			{
				core->DB = PackedPCH;
				core->DB_Dirty = true;
			}
		}

		if (PCH_ADH)
		{
			if (core->ADH_Dirty)
			{
				core->ADH = core->ADH & PackedPCH;
			}
			else
			{
				core->ADH = PackedPCH;
				core->ADH_Dirty = true;
			}
		}
	}

	uint8_t ProgramCounter::getPCL()
	{
		if (HLE)
			return PackedPCL;

		TriState v[8]{};

		// The value stored in PCL alternates between inversion, because of the inverted carry chain.

		for (size_t n = 0; n < 8; n++)
		{
			if (n & 1)
			{
				v[n] = PCL[n].nget();
			}
			else
			{
				v[n] = NOT(PCL[n].nget());
			}
		}

		return Pack(v);
	}

	uint8_t ProgramCounter::getPCH()
	{
		if (HLE)
			return PackedPCH;

		TriState v[8]{};

		// The value stored in PCH alternates between inversion, because of the inverted carry chain.
		// The storage inversion is different from the storage inversion of the PCL register (because of the alteration of the PCL and PCH bit circuits).

		for (size_t n = 0; n < 8; n++)
		{
			if (n & 1)
			{
				v[n] = NOT(PCH[n].nget());
			}
			else
			{
				v[n] = PCH[n].nget();
			}
		}

		return Pack(v);
	}

	uint8_t ProgramCounter::getPCLS()
	{
		if (HLE)
			return PackedPCLS;

		TriState v[8]{};

		for (size_t n = 0; n < 8; n++)
		{
			v[n] = PCLS[n].get();
		}

		return Pack(v);
	}

	uint8_t ProgramCounter::getPCHS()
	{
		if (HLE)
			return PackedPCHS;

		TriState v[8]{};

		for (size_t n = 0; n < 8; n++)
		{
			v[n] = PCHS[n].get();
		}

		return Pack(v);
	}

	void ProgramCounter::setPCL(uint8_t val)
	{
		if (HLE)
		{
			PackedPCL = val;
			return;
		}

		TriState v[8]{};
		Unpack(val, v);

		// The value stored in PCL alternates between inversion, because of the inverted carry chain.

		for (size_t n = 0; n < 8; n++)
		{
			if (n & 1)
			{
				PCL[n].set(NOT(v[n]), TriState::One);
			}
			else
			{
				PCL[n].set(v[n], TriState::One);
			}
		}
	}

	void ProgramCounter::setPCH(uint8_t val)
	{
		if (HLE)
		{
			PackedPCH = val;
			return;
		}

		TriState v[8]{};
		Unpack(val, v);

		// The value stored in PCH alternates between inversion, because of the inverted carry chain.
		// The storage inversion is different from the storage inversion of the PCL register (because of the alteration of the PCL and PCH bit circuits).

		for (size_t n = 0; n < 8; n++)
		{
			if (n & 1)
			{
				PCH[n].set(v[n], TriState::One);
			}
			else
			{
				PCH[n].set(NOT(v[n]), TriState::One);
			}
		}
	}

	void ProgramCounter::setPCLS(uint8_t val)
	{
		if (HLE)
		{
			PackedPCLS = val;
			return;
		}

		TriState v[8]{};
		Unpack(val, v);

		for (size_t n = 0; n < 8; n++)
		{
			PCLS[n].set(v[n], TriState::One);
		}
	}

	void ProgramCounter::setPCHS(uint8_t val)
	{
		if (HLE)
		{
			PackedPCHS = val;
			return;
		}

		TriState v[8]{};
		Unpack(val, v);

		for (size_t n = 0; n < 8; n++)
		{
			PCHS[n].set(v[n], TriState::One);
		}
	}

	void DataBus::sim_SetExternalBus(uint8_t* data_bus)
	{
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState WR = core->wire.WR;

		rd_latch.set(WR, PHI1);
		TriState RD = NOT(NOR(NOT(PHI2), rd_latch.nget()));

		if (PHI1)
		{
			DOR = ~(core->DB);
		}

		if (RD == TriState::Zero)
		{
			*data_bus = ~DOR;
		}
	}

	void DataBus::sim_GetExternalBus(uint8_t* data_bus)
	{
		TriState PHI1 = core->wire.PHI1;
		TriState PHI2 = core->wire.PHI2;
		TriState WR = core->wire.WR;
		bool DL_ADL = core->cmd.DL_ADL;
		bool DL_ADH = core->cmd.DL_ADH;
		bool DL_DB = core->cmd.DL_DB;

		rd_latch.set(WR, PHI1);
		TriState RD = NOT(NOR(NOT(PHI2), rd_latch.nget()));

		if (PHI2 == TriState::One)
		{
			DL = ~(*data_bus);
		}

		if (PHI1 == TriState::One)
		{
			if (DL_ADL)
			{
				if (core->ADL_Dirty)
				{
					core->ADL = core->ADL & ~DL;
				}
				else
				{
					core->ADL = ~DL;
					core->ADL_Dirty = true;
				}
			}

			if (DL_ADH)
			{
				if (core->ADH_Dirty)
				{
					core->ADH = core->ADH & ~DL;
				}
				else
				{
					core->ADH = ~DL;
					core->ADH_Dirty = true;
				}
			}

			if (DL_DB)
			{
				if (core->DB_Dirty)
				{
					core->DB = core->DB & ~DL;
				}
				else
				{
					core->DB = ~DL;
					core->DB_Dirty = true;
				}
			}
		}
	}

	uint8_t DataBus::getDL()
	{
		return ~DL;
	}

	uint8_t DataBus::getDOR()
	{
		return ~DOR;
	}

	void DataBus::setDL(uint8_t val)
	{
		DL = ~val;
	}

	void DataBus::setDOR(uint8_t val)
	{
		DOR = ~val;
	}

	// 6502 Main

	M6502::M6502(bool HLE, bool BCD_Hack)
	{
		HLE_Mode = HLE;

		decoder = new Decoder;
		predecode = new PreDecode(this);
		ir = new IR(this);
		ext = new ExtraCounter(this);
		brk = new BRKProcessing(this);
		disp = new Dispatcher(this);
		random = new RandomLogic(this);

		addr_bus = new AddressBus(this);
		regs = new Regs(this);
		alu = new ALU(this);
		alu->SetBCDHack(BCD_Hack);
		pc = new ProgramCounter(this, HLE_Mode);
		data_bus = new DataBus(this);
	}

	M6502::~M6502()
	{
		delete decoder;
		delete predecode;
		delete ir;
		delete ext;
		delete brk;
		delete disp;
		delete random;

		delete addr_bus;
		delete regs;
		delete alu;
		delete pc;
		delete data_bus;
	}

	void M6502::sim_Top(TriState inputs[], uint8_t* data_bus)
	{
		wire.n_NMI = inputs[(size_t)InputPad::n_NMI];
		wire.n_IRQ = inputs[(size_t)InputPad::n_IRQ];
		wire.n_RES = inputs[(size_t)InputPad::n_RES];
		wire.PHI0 = inputs[(size_t)InputPad::PHI0];
		wire.RDY = inputs[(size_t)InputPad::RDY];
		wire.SO = inputs[(size_t)InputPad::SO];

		// Logic associated with external input terminals

		wire.PHI1 = NOT(wire.PHI0);
		wire.PHI2 = wire.PHI0;

		prdy_latch1.set(NOT(wire.RDY), wire.PHI2);
		prdy_latch2.set(prdy_latch1.nget(), wire.PHI1);
		wire.n_PRDY = prdy_latch2.nget();

		if (wire.PHI2 == TriState::One && nNMI_Cache != wire.n_NMI)
		{
			nmip_ff.set(NOR(NOR(nmip_ff.get(), AND(NOT(wire.n_NMI), wire.PHI2)), AND(wire.n_NMI, wire.PHI2)));
			nNMI_Cache = wire.n_NMI;
		}
		wire.n_NMIP = NOT(nmip_ff.get());

		if (wire.PHI2 == TriState::One && nIRQ_Cache != wire.n_IRQ)
		{
			irqp_ff.set(NOR(NOR(irqp_ff.get(), AND(NOT(wire.n_IRQ), wire.PHI2)), AND(wire.n_IRQ, wire.PHI2)));
			nIRQ_Cache = wire.n_IRQ;
		}
		irqp_latch.set(irqp_ff.get(), wire.PHI1);
		wire.n_IRQP = irqp_latch.nget();

		if (wire.PHI2 == TriState::One && nRES_Cache != wire.n_RES)
		{
			resp_ff.set(NOR(NOR(resp_ff.get(), AND(wire.n_RES, wire.PHI2)), AND(NOT(wire.n_RES), wire.PHI2)));
			nRES_Cache = wire.n_RES;
		}
		resp_latch.set(resp_ff.get(), wire.PHI1);
		wire.RESP = resp_latch.nget();

		// Dispatcher and other auxiliary logic

		disp->sim_BeforeDecoder();

		predecode->sim(data_bus);

		ir->sim();

		if (HLE_Mode)
		{
			ext->sim_HLE();
		}
		else
		{
			ext->sim();
		}

		DecoderInput decoder_in{};

		uint8_t IR = ir->IROut;

		TriState IR0 = IR & 0b00000001 ? TriState::One : TriState::Zero;
		TriState IR1 = IR & 0b00000010 ? TriState::One : TriState::Zero;
		TriState IR2 = IR & 0b00000100 ? TriState::One : TriState::Zero;
		TriState IR3 = IR & 0b00001000 ? TriState::One : TriState::Zero;
		TriState IR4 = IR & 0b00010000 ? TriState::One : TriState::Zero;
		TriState IR5 = IR & 0b00100000 ? TriState::One : TriState::Zero;
		TriState IR6 = IR & 0b01000000 ? TriState::One : TriState::Zero;
		TriState IR7 = IR & 0b10000000 ? TriState::One : TriState::Zero;

		wire.n_IR5 = NOT(IR5);

		decoder_in.n_IR0 = NOT(IR0);
		decoder_in.n_IR1 = NOT(IR1);
		decoder_in.IR01 = OR(IR0, IR1);
		decoder_in.n_IR2 = NOT(IR2);
		decoder_in.IR2 = IR2;
		decoder_in.n_IR3 = NOT(IR3);
		decoder_in.IR3 = IR3;
		decoder_in.n_IR4 = NOT(IR4);
		decoder_in.IR4 = IR4;
		decoder_in.n_IR5 = wire.n_IR5;
		decoder_in.IR5 = IR5;
		decoder_in.n_IR6 = NOT(IR6);
		decoder_in.IR6 = IR6;
		decoder_in.n_IR7 = NOT(IR7);
		decoder_in.IR7 = IR7;

		decoder_in.n_T0 = wire.n_T0;
		decoder_in.n_T1X = wire.n_T1X;
		decoder_in.n_T2 = wire.n_T2;
		decoder_in.n_T3 = wire.n_T3;
		decoder_in.n_T4 = wire.n_T4;
		decoder_in.n_T5 = wire.n_T5;

		TxBits = 0;
		TxBits |= ((size_t)wire.n_T0 << 0);
		TxBits |= ((size_t)wire.n_T1X << 1);
		TxBits |= ((size_t)wire.n_T2 << 2);
		TxBits |= ((size_t)wire.n_T3 << 3);
		TxBits |= ((size_t)wire.n_T4 << 4);
		TxBits |= ((size_t)wire.n_T5 << 5);

		decoder->sim(decoder_in.packed_bits, &decoder_out);

		// Interrupt handling

		brk->sim_BeforeRandom();

		// Random Logic

		disp->sim_BeforeRandomLogic();

		random->sim();

		brk->sim_AfterRandom();

		disp->sim_AfterRandomLogic();
	}

	void M6502::sim_Bottom(TriState inputs[], TriState outputs[], uint16_t* ext_addr_bus, uint8_t* ext_data_bus)
	{
		// Bottom Part

		// When you simulate the lower part, you have to turn on the man in you to the fullest and imagine that you are possessed by Chuck Peddle.
		// From the development history of the 6502, we know that one developer did the upper part and another (Chuck) did the lower part. 
		// So to simulate the lower part you have to abstract as much as possible and work only with what you have (registers, buses and set of control commands from the upper part).

		// Bus load from DL: DL_DB, DL_ADL, DL_ADH

		data_bus->sim_GetExternalBus(ext_data_bus);

		// Registers to the SB bus: Y_SB, X_SB, S_SB

		regs->sim_StoreSB();

		// ADD saving on SB/ADL: ADD_SB7, ADD_SB06, ADD_ADL

		alu->sim_StoreADD();

		// Saving AC: AC_SB, AC_DB

		alu->sim_StoreAC();

		// Saving flags on DB bus: P_DB

		random->flags->sim_Store();

		// Stack pointer saving on ADL bus: S_ADL

		regs->sim_StoreOldS();

		// Increment PC: n_1PC

		pc->sim();

		// Saving PC to buses: PCL_ADL, PCH_ADH, PCL_DB, PCH_DB

		pc->sim_Store();

		// Bus multiplexing: SB_DB, SB_ADH

		alu->sim_BusMux();

		// Constant generator: Z_ADL0, Z_ADL1, Z_ADL2, Z_ADH0, Z_ADH17

		addr_bus->sim_ConstGen();

		// Loading ALU operands: NDB_ADD, DB_ADD, Z_ADD, SB_ADD, ADL_ADD

		alu->sim_Load();

		// ALU operation: ANDS, EORS, ORS, SRS, SUMS, n_ACIN, n_DAA, n_DSA
		// BCD correction via SB bus: SB_AC

		if (HLE_Mode)
		{
			alu->sim_HLE();
		}
		else
		{
			alu->sim();
		}

		// Load flags: DB_P, DBZ_Z, DB_N, IR5_C, DB_C, IR5_D, IR5_I, DB_V, Z_V, ACR_C, AVR_V

		random->flags->sim_Load();

		// Load registers: SB_X, SB_Y, SB_S / S_S

		regs->sim_LoadSB();

		// PC loading from buses / keep: ADH_PCH/PCH_PCH, ADL_PCL/PCL_PCL

		pc->sim_Load();

		// Saving DB to DOR

		data_bus->sim_SetExternalBus(ext_data_bus);

		// Set external address bus: ADH_ABH, ADL_ABL

		addr_bus->sim_Output(ext_addr_bus);

		// Outputs

		rw_latch.set(wire.WR, wire.PHI1);

		outputs[(size_t)OutputPad::PHI1] = wire.PHI1;
		outputs[(size_t)OutputPad::PHI2] = wire.PHI2;
		outputs[(size_t)OutputPad::RnW] = rw_latch.nget();
		outputs[(size_t)OutputPad::SYNC] = disp->getT1();
	}

	void M6502::sim(TriState inputs[], TriState outputs[], uint16_t* addr_bus, uint8_t* data_bus)
	{
		TriState PHI0 = inputs[(size_t)InputPad::PHI0];
		TriState PHI2 = PHI0;

		// Precharge internal buses.
		// This operation is critical because it is used to form the interrupt address and the stack pointer (and many other cases).

		if (PHI2 == TriState::One)
		{
			SB = 0xff;
			DB = 0xff;
			ADL = 0xff;
			ADH = 0xff;
		}

		// These variables are used to mark the "dirty" bits of the internal buses. This is used to resolve conflicts, according to the "Ground wins" rule.

		SB_Dirty = false;
		DB_Dirty = false;
		ADL_Dirty = false;
		ADH_Dirty = false;

		// To stabilize latches, both parts are simulated twice.

		sim_Top(inputs, data_bus);
		sim_Bottom(inputs, outputs, addr_bus, data_bus);
		sim_Top(inputs, data_bus);
		sim_Bottom(inputs, outputs, addr_bus, data_bus);
	}
}