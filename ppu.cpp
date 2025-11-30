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

namespace PPUSim
{
	// Background Color (BG COL)

	BGCol::BGCol(PPU* parent)
	{
		ppu = parent;
	}

	BGCol::~BGCol()
	{
	}

	void BGCol::sim()
	{
		sim_Control();
		sim_BGC0();
		sim_BGC1();
		sim_BGC2();
		sim_BGC3();
		sim_Output();
	}

	void BGCol::sim_Control()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState H0_DD = ppu->wire.H0_Dash2;
		TriState F_TA = ppu->fsm.FTA;
		TriState F_TB = ppu->fsm.FTB;
		TriState n_FO = ppu->fsm.nFO;
		TriState F_AT = ppu->fsm.FAT;
		TriState THO1 = ppu->wire.THO[1];

		fat_latch.set(NOT(F_AT), PCLK);
		tho1_latch.set(THO1, PCLK);

		auto nH0DD = NOT(H0_DD);

		PD_SR = NOR3(NOT(F_TA), nH0DD, PCLK);
		SRLOAD = NOR3(NOT(F_TB), nH0DD, PCLK);
		STEP = NOR3(PCLK, NOR(NOT(F_TB), nH0DD), NOT(n_FO));
		STEP2 = NOR(PCLK, NOT(n_FO));
		PD_SEL = NOR3(fat_latch.get(), PCLK, nH0DD);
		NEXT = NOR3(STEP, STEP2, n_PCLK);
		H01 = NOT(tho1_latch.nget());
	}

	void BGCol::sim_BGC0()
	{
		TriState sr_in[8]{};
		TriState sr_out1[8]{};
		TriState sr_out2[8]{};

		for (size_t n = 0; n < 8; n++)
		{
			BGC0_Latch[n].set(ppu->GetPDBit(n), PD_SR);
			sr_in[7 - n] = BGC0_Latch[n].nget();
		}

		BGC0_SR1.sim(sr_in, TriState::One, SRLOAD, STEP, NEXT, sr_out1);
		BGC0_SR2.sim(unused, sr_out1[0], TriState::Zero, STEP2, NEXT, sr_out2);
		n_BGC0_Out = MUX3(ppu->wire.FH, sr_out2);
	}

	void BGCol::sim_BGC1()
	{
		TriState sr_in[8]{};
		TriState sr_out1[8]{};
		TriState sr_out2[8]{};

		for (size_t n = 0; n < 8; n++)
		{
			sr_in[7 - n] = ppu->GetPDBit(n);
		}

		BGC1_SR1.sim(sr_in, TriState::One, SRLOAD, STEP, NEXT, sr_out1);
		BGC1_SR2.sim(unused, sr_out1[0], TriState::Zero, STEP2, NEXT, sr_out2);
		BGC1_Out = MUX3(ppu->wire.FH, sr_out2);
	}

	void BGCol::sim_BGC2()
	{
		pd_latch[0].set(ppu->GetPDBit(0), PD_SEL);
		pd_latch[2].set(ppu->GetPDBit(2), PD_SEL);
		pd_latch[4].set(ppu->GetPDBit(4), PD_SEL);
		pd_latch[6].set(ppu->GetPDBit(6), PD_SEL);

		TriState mux_sel[2]{};
		mux_sel[0] = H01;
		mux_sel[1] = ppu->wire.TVO[1];

		TriState mux_in[4]{};
		mux_in[0] = pd_latch[0].nget();
		mux_in[1] = pd_latch[2].nget();
		mux_in[2] = pd_latch[4].nget();
		mux_in[3] = pd_latch[6].nget();

		TriState shift_out;
		BGC2_SRBit1.sim(TriState::Z, MUX2(mux_sel, mux_in), SRLOAD, TriState::Zero, NEXT, shift_out);

		TriState sr_out[8]{};
		BGC2_SR1.sim(unused, shift_out, TriState::Zero, STEP2, NEXT, sr_out);

		n_BGC2_Out = MUX3(ppu->wire.FH, sr_out);
	}

	void BGCol::sim_BGC3()
	{
		pd_latch[1].set(ppu->GetPDBit(1), PD_SEL);
		pd_latch[3].set(ppu->GetPDBit(3), PD_SEL);
		pd_latch[5].set(ppu->GetPDBit(5), PD_SEL);
		pd_latch[7].set(ppu->GetPDBit(7), PD_SEL);

		TriState mux_sel[2]{};
		mux_sel[0] = H01;
		mux_sel[1] = ppu->wire.TVO[1];

		TriState mux_in[4]{};
		mux_in[0] = pd_latch[1].nget();
		mux_in[1] = pd_latch[3].nget();
		mux_in[2] = pd_latch[5].nget();
		mux_in[3] = pd_latch[7].nget();

		TriState shift_out;
		BGC3_SRBit1.sim(TriState::Z, MUX2(mux_sel, mux_in), SRLOAD, TriState::Zero, NEXT, shift_out);

		TriState sr_out[8]{};
		BGC3_SR1.sim(unused, shift_out, TriState::Zero, STEP2, NEXT, sr_out);

		n_BGC3_Out = MUX3(ppu->wire.FH, sr_out);
	}

	void BGCol::sim_Output()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_CLPB = ppu->wire.n_CLPB;

		clpb_latch.set(n_CLPB, PCLK);
		auto clip = clpb_latch.nget();

		bgc0_latch[0].set(NOT(n_BGC0_Out), n_PCLK);
		bgc0_latch[1].set(bgc0_latch[0].nget(), PCLK);

		bgc1_latch[0].set(BGC1_Out, n_PCLK);
		bgc1_latch[1].set(bgc1_latch[0].nget(), PCLK);

		bgc2_latch[0].set(NOT(n_BGC2_Out), n_PCLK);
		bgc2_latch[1].set(bgc2_latch[0].nget(), PCLK);

		bgc3_latch[0].set(NOT(n_BGC3_Out), n_PCLK);
		bgc3_latch[1].set(bgc3_latch[0].nget(), PCLK);

		ppu->wire.BGC[0] = NOR(bgc0_latch[1].get(), clip);
		ppu->wire.BGC[1] = NOR(bgc1_latch[1].get(), clip);
		ppu->wire.BGC[2] = NOR(bgc2_latch[1].get(), clip);
		ppu->wire.BGC[3] = NOR(bgc3_latch[1].get(), clip);
	}

	void BGC_SRBit::sim(TriState shift_in, TriState val_in,
		TriState Load, TriState Step, TriState Next,
		TriState& shift_out)
	{
		in_latch.set(MUX(Step, MUX(Load, TriState::Z, val_in), shift_in), TriState::One);
		out_latch.set(in_latch.nget(), Next);
		shift_out = out_latch.nget();
	}

	void BGC_SR8::sim(TriState val[8], TriState sin,
		TriState Load, TriState Step, TriState Next,
		TriState sout[8])
	{
		TriState shift_in = sin;

		for (int n = 7; n >= 0; n--)
		{
			sr[n].sim(shift_in, val[n], Load, Step, Next, sout[n]);
			shift_in = sout[n];
		}
	}

	// Color Generator RAM (Palette)

	void CBBit::sim(size_t bit_num, TriState* cell, TriState n_OE)
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_DB_CB = ppu->wire.n_DB_CB;
		TriState n_CB_DB = ppu->wire.n_CB_DB;

		if (n_DB_CB == TriState::Zero)
		{
			TriState DBBit = ppu->GetDBBit(bit_num);

			if (*cell != TriState::Z && DBBit != TriState::Z)
			{
				*cell = DBBit;
			}
		}

		if (PCLK == TriState::One)
		{
			ff.set(TriState::Zero);
		}
		else
		{
			ff.set(*cell);
		}

		TriState CBOut = NOT(NAND(ff.get(), n_OE));

		latch1.set(CBOut, n_PCLK);
		latch2.set(latch1.nget(), PCLK);

		TriState DBOut = MUX(NOT(n_CB_DB), TriState::Z, latch2.nget());
		ppu->SetDBBit(bit_num, DBOut);
	}

	TriState CBBit::get_CBOut(TriState n_OE)
	{
		return NOT(NAND(ff.get(), n_OE));
	}

	CRAM::CRAM(PPU* parent)
	{
		ppu = parent;

		for (size_t n = 0; n < 6; n++)
		{
			switch (ppu->rev)
			{
				// TBD: Check how things are in other RGB PPUs.

			case Revision::RP2C04_0003:
				cb[n] = new CBBit_RGB(ppu);
				break;

			default:
				cb[n] = new CBBit(ppu);
				break;
			}
		}
	}

	CRAM::~CRAM()
	{
		for (size_t n = 0; n < 6; n++)
		{
			delete cb[n];
		}
	}

	void CRAM::sim()
	{
		sim_CRAMControl();
		sim_CRAMDecoder();
		sim_ColorBuffer();
	}

	void CRAM::sim_CRAMControl()
	{
		TriState n_R7 = ppu->wire.n_R7;
		TriState n_DBE = ppu->wire.n_DBE;
		TriState TH_MUX = ppu->wire.TH_MUX;
		TriState n_PICTURE = ppu->fsm.n_PICTURE;

		switch (ppu->rev)
		{
			// TBD: Add the remaining RGB PPUs

		case Revision::RP2C04_0003:
		{
			dbpar_latch.set(ppu->wire.DB_PAR, ppu->wire.PCLK);
			ppu->wire.n_BW = NOR(n_PICTURE, ppu->wire.BnW);
			ppu->wire.n_DB_CB = NAND(dbpar_latch.get(), TH_MUX);
		}
		break;

		default:
		{
			dbpar_latch.set(ppu->wire.DB_PAR, ppu->wire.PCLK);
			ppu->wire.n_CB_DB = NOT(NOR3(n_R7, n_DBE, NOT(TH_MUX)));
			ppu->wire.n_BW = NOR(AND(ppu->wire.n_CB_DB, n_PICTURE), ppu->wire.BnW);
			ppu->wire.n_DB_CB = NAND(dbpar_latch.get(), TH_MUX);
		}
		break;
		}
	}

	void CRAM::sim_CRAMDecoder()
	{
		TriState col_in[2]{};
		col_in[0] = ppu->wire.CGA[2];
		col_in[1] = ppu->wire.CGA[3];
		DMX2(col_in, COL);

		TriState row_in[3]{};
		row_in[0] = ppu->wire.CGA[0];
		row_in[1] = ppu->wire.CGA[1];
		row_in[2] = ppu->wire.CGA[4];
		DMX3(row_in, ROW);

		for (size_t n = 0; n < 8; n++)
		{
			ROW[n] = AND(ROW[n], NOT(ppu->wire.PCLK));
		}
	}

	TriState* CRAM::AddressCell(size_t bit_num)
	{
		size_t row;
		size_t col;
		bool anyRow = false;
		bool anyCol = false;

		// Select row

		if (ROW[0] == TriState::One || ROW[4] == TriState::One)
		{
			row = 0;
			anyRow = true;
		}
		else if (ROW[6] == TriState::One)
		{
			row = 1;
			anyRow = true;
		}
		else if (ROW[2] == TriState::One)
		{
			row = 2;
			anyRow = true;
		}
		else if (ROW[5] == TriState::One)
		{
			row = 3;
			anyRow = true;
		}
		else if (ROW[1] == TriState::One)
		{
			row = 4;
			anyRow = true;
		}
		else if (ROW[7] == TriState::One)
		{
			row = 5;
			anyRow = true;
		}
		else if (ROW[3] == TriState::One)
		{
			row = 6;
			anyRow = true;
		}

		// Select column

		if (anyRow)
		{
			if (COL[0] == TriState::One)
			{
				col = 0;
				anyCol = true;
			}
			else if (COL[1] == TriState::One)
			{
				col = 1;
				anyCol = true;
			}
			else if (COL[2] == TriState::One)
			{
				col = 2;
				anyCol = true;
			}
			else if (COL[3] == TriState::One)
			{
				col = 3;
				anyCol = true;
			}
		}

		// Address cell

		if (anyRow && anyCol)
		{
			return &cram[row * cram_lane_cols + col][bit_num];
		}

		return &z_cell;
	}

	void CRAM::sim_ColorBuffer()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;

		for (size_t bit_num = 0; bit_num < cb_num; bit_num++)
		{
			cb[bit_num]->sim(
				bit_num, AddressCell(bit_num),
				bit_num < 4 ? ppu->wire.n_BW : TriState::One);
		}

		// A chain of output latches.

		LL0_latch[0].set(cb[4]->get_CBOut(TriState::One), n_PCLK);
		LL0_latch[1].set(LL0_latch[0].nget(), PCLK);
		LL0_latch[2].set(LL0_latch[1].nget(), n_PCLK);
		ppu->wire.n_LL[0] = LL0_latch[2].nget();

		LL1_latch[0].set(cb[5]->get_CBOut(TriState::One), n_PCLK);
		LL1_latch[1].set(LL1_latch[0].nget(), PCLK);
		LL1_latch[2].set(LL1_latch[1].nget(), n_PCLK);
		ppu->wire.n_LL[1] = LL1_latch[2].nget();

		for (size_t n = 0; n < 4; n++)
		{
			CC_latch[n].set(cb[n]->get_CBOut(ppu->wire.n_BW), n_PCLK);
			ppu->wire.n_CC[n] = CC_latch[n].nget();
		}
	}

	size_t CRAM::MapRow(size_t rowNum)
	{
		switch (rowNum)
		{
		case 0:
		case 4:
			return 0;
		case 6:
			return 1;
		case 2:
			return 2;
		case 5:
			return 3;
		case 1:
			return 4;
		case 7:
			return 5;
		case 3:
			return 6;
		default:
			return rowNum;
		}
	}

	uint8_t CRAM::Dbg_CRAMReadByte(size_t addr)
	{
		uint8_t val = 0;

		size_t row = (addr & 3) | ((addr & 0x10) ? 4 : 0);
		size_t col = (addr >> 2) & 3;

		row = MapRow(row);

		for (size_t n = 0; n < 6; n++)
		{
			val |= (cram[row * cram_lane_cols + col][n] == TriState::One ? 1 : 0) << n;
		}

		return val;
	}

	void CRAM::Dbg_CRAMWriteByte(size_t addr, uint8_t val)
	{
		size_t row = (addr & 3) | ((addr & 0x10) ? 4 : 0);
		size_t col = (addr >> 2) & 3;

		row = MapRow(row);

		for (size_t n = 0; n < 6; n++)
		{
			TriState bit_val = ((val >> n) & 1) ? TriState::One : TriState::Zero;
			cram[row * cram_lane_cols + col][n] = bit_val;
		}
	}

	/// <summary>
	/// The different version for RGB PPU (the one studied) is that CRAM is Write-Only.
	/// </summary>
	void CBBit_RGB::sim(size_t bit_num, TriState* cell, TriState n_OE)
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_DB_CB = ppu->wire.n_DB_CB;

		if (n_DB_CB == TriState::Zero)
		{
			TriState DBBit = ppu->GetDBBit(bit_num);

			if (*cell != TriState::Z && DBBit != TriState::Z)
			{
				*cell = DBBit;
			}
		}

		if (PCLK == TriState::One)
		{
			ff.set(TriState::Zero);
		}
		else
		{
			ff.set(*cell);
		}
	}

	// Object FIFO (Motion picture buffer memory)

	FIFO::FIFO(PPU* parent)
	{
		ppu = parent;

		for (size_t n = 0; n < 8; n++)
		{
			lane[n] = new FIFOLane(ppu);
		}
	}

	FIFO::~FIFO()
	{
		for (size_t n = 0; n < 8; n++)
		{
			delete lane[n];
		}
	}

	void FIFO::sim()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState PCLK = ppu->wire.PCLK;
		TriState Z_HPOS = ppu->fsm.ZHPOS;

		zh_latch1.set(Z_HPOS, n_PCLK);
		zh_latch2.set(zh_latch1.nget(), PCLK);
		zh_latch3.set(zh_latch2.nget(), n_PCLK);
		ppu->wire.n_ZH = zh_latch3.nget();

		sim_HInv();
		BitRev(n_TX);
		sim_Lanes();
		sim_Prio();
	}

	void FIFO::sim_HInv()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_OBJ_RD_ATTR = ppu->wire.n_OBJ_RD_ATTR;
		TriState OB6 = ppu->wire.OB[6];
		TriState PD_FIFO = ppu->wire.PD_FIFO;

		HINV_FF.set(MUX(NOR(n_PCLK, n_OBJ_RD_ATTR), NOT(NOT(HINV_FF.get())), OB6));

		auto HINV = HINV_FF.get();

		for (size_t n = 0; n < 8; n++)
		{
			tout_latch[n].set(MUX(HINV, ppu->GetPDBit(n), ppu->GetPDBit(7 - n)), n_PCLK);
			n_TX[n] = NAND(tout_latch[n].get(), PD_FIFO);
		}
	}

	/// <summary>
	/// Generate LaneOut outputs for the priority circuit.
	/// </summary>
	void FIFO::sim_Lanes()
	{
		TriState in[3]{};
		TriState HSel[8];

		in[0] = ppu->wire.H3_Dash2;
		in[1] = ppu->wire.H4_Dash2;
		in[2] = ppu->wire.H5_Dash2;

		DMX3(in, HSel);

		for (size_t n = 0; n < 8; n++)
		{
			lane[n]->sim(HSel[n], n_TX, packed_nTX, LaneOut[n]);
		}
	}

	/// <summary>
	/// Based on the priorities, select one of the LaneOut values.
	/// </summary>
	void FIFO::sim_Prio()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState CLPO = ppu->wire.CLPO;
		TriState Z[8]{};
		TriState in[10]{};

		Z[0] = NOR3(AND(get_nZCOL0(0), get_nZCOL1(0)), CLPO, get_nxEN(0));

		// You can get the `/SPR0HIT` signal immediately

		s0_latch.set(Z[0], PCLK);
		ppu->wire.n_SPR0HIT = s0_latch.nget();

		in[0] = AND(get_nZCOL0(1), get_nZCOL1(1));
		in[1] = CLPO;
		in[2] = get_nxEN(1);
		in[3] = Z[0];
		Z[1] = NOR4(in);

		in[0] = AND(get_nZCOL0(2), get_nZCOL1(2));
		in[1] = CLPO;
		in[2] = get_nxEN(2);
		in[3] = Z[0];
		in[4] = Z[1];
		Z[2] = NOR5(in);

		in[0] = AND(get_nZCOL0(3), get_nZCOL1(3));
		in[1] = CLPO;
		in[2] = get_nxEN(3);
		in[3] = Z[0];
		in[4] = Z[1];
		in[5] = Z[2];
		Z[3] = NOR6(in);

		in[0] = AND(get_nZCOL0(4), get_nZCOL1(4));
		in[1] = CLPO;
		in[2] = get_nxEN(4);
		in[3] = Z[0];
		in[4] = Z[1];
		in[5] = Z[2];
		in[6] = Z[3];
		Z[4] = NOR7(in);

		in[0] = AND(get_nZCOL0(5), get_nZCOL1(5));
		in[1] = CLPO;
		in[2] = get_nxEN(5);
		in[3] = Z[0];
		in[4] = Z[1];
		in[5] = Z[2];
		in[6] = Z[3];
		in[7] = Z[4];
		Z[5] = NOR8(in);

		in[0] = AND(get_nZCOL0(6), get_nZCOL1(6));
		in[1] = CLPO;
		in[2] = get_nxEN(6);
		in[3] = Z[0];
		in[4] = Z[1];
		in[5] = Z[2];
		in[6] = Z[3];
		in[7] = Z[4];
		in[8] = Z[5];
		Z[6] = NOR9(in);

		in[0] = AND(get_nZCOL0(7), get_nZCOL1(7));
		in[1] = CLPO;
		in[2] = get_nxEN(7);
		in[3] = Z[0];
		in[4] = Z[1];
		in[5] = Z[2];
		in[6] = Z[3];
		in[7] = Z[4];
		in[8] = Z[5];
		in[9] = Z[6];
		Z[7] = NOR10(in);

		// The multiplexer locomotive is simulated by the ordinary for-loop.

		size_t run = 0;

		for (run = 0; run < 8; run++)
		{
			if (Z[run] == TriState::One)
			{
				ppu->wire.n_ZCOL0 = get_nZCOL0(run);
				ppu->wire.n_ZCOL1 = get_nZCOL1(run);
				ppu->wire.ZCOL2 = LaneOut[run].Z_COL2;
				ppu->wire.ZCOL3 = LaneOut[run].Z_COL3;
				ppu->wire.n_ZPRIO = LaneOut[run].nZ_PRIO;

				break;
			}
		}

		// When no Z is active, hidden latches are activated, which are between FIFO and MUX.
		// `/ZCOL0/1` are forcibly set to `1` (there is a corresponding nor).

		if (run == 8)
		{
			ppu->wire.n_ZCOL0 = TriState::One;
			ppu->wire.n_ZCOL1 = TriState::One;
			ppu->wire.ZCOL2 = col2_latch.get();
			ppu->wire.ZCOL3 = col3_latch.get();
			ppu->wire.n_ZPRIO = prio_latch.get();
		}
		else
		{
			col2_latch.set(ppu->wire.ZCOL2, TriState::One);
			col3_latch.set(ppu->wire.ZCOL3, TriState::One);
			prio_latch.set(ppu->wire.n_ZPRIO, TriState::One);
		}
	}

	TriState FIFO::get_nZCOL0(size_t lane)
	{
		return LaneOut[lane].nZ_COL0;
	}

	TriState FIFO::get_nZCOL1(size_t lane)
	{
		return LaneOut[lane].nZ_COL1;
	}

	TriState FIFO::get_nxEN(size_t lane)
	{
		return LaneOut[lane].n_xEN;
	}

	void FIFO::sim_SpriteH()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState OBJ_READ = ppu->fsm.OBJ_READ;
		TriState H0_DD = ppu->wire.H0_Dash2;
		TriState H1_DD = ppu->wire.H1_Dash2;
		TriState H2_DD = ppu->wire.H2_Dash2;

		TriState in[3]{};
		TriState dec_out[8]{};
		in[0] = H0_DD;
		in[1] = H1_DD;
		in[2] = H2_DD;

		DMX3(in, dec_out);

		sh2_latch.set(MUX(OBJ_READ, TriState::Zero, dec_out[2]), n_PCLK);
		sh3_latch.set(MUX(OBJ_READ, TriState::Zero, dec_out[3]), n_PCLK);
		sh5_latch.set(MUX(OBJ_READ, TriState::Zero, dec_out[5]), n_PCLK);
		sh7_latch.set(MUX(OBJ_READ, TriState::Zero, dec_out[7]), n_PCLK);

		ppu->wire.n_OBJ_RD_ATTR = sh2_latch.nget();
		ppu->wire.n_OBJ_RD_X = sh3_latch.nget();
		ppu->wire.n_OBJ_RD_A = sh5_latch.nget();
		ppu->wire.n_OBJ_RD_B = sh7_latch.nget();
	}

#pragma region "FIFO Lane"

	FIFOLane::FIFOLane(PPU* parent)
	{
		ppu = parent;
	}

	FIFOLane::~FIFOLane()
	{
	}

	void FIFOLane::sim_LaneControl(TriState HSel)
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_OBJ_RD_ATTR = ppu->wire.n_OBJ_RD_ATTR;
		TriState n_OBJ_RD_X = ppu->wire.n_OBJ_RD_X;
		TriState n_OBJ_RD_A = ppu->wire.n_OBJ_RD_A;
		TriState n_OBJ_RD_B = ppu->wire.n_OBJ_RD_B;

		hsel_latch.set(HSel, n_PCLK);

		LDAT = NOR3(n_PCLK, hsel_latch.nget(), n_OBJ_RD_ATTR);
		LOAD = NOR3(n_PCLK, hsel_latch.nget(), n_OBJ_RD_X);
		T_SR0 = NOR3(n_PCLK, hsel_latch.nget(), n_OBJ_RD_A);
		T_SR1 = NOR3(n_PCLK, hsel_latch.nget(), n_OBJ_RD_B);

		ob0_latch[0].set(ppu->wire.OB[0], LDAT);
		ob1_latch[0].set(ppu->wire.OB[1], LDAT);
		ob5_latch[0].set(ppu->wire.OB[5], LDAT);

		ob0_latch[1].set(ob0_latch[0].nget(), n_PCLK);
		ob1_latch[1].set(ob1_latch[0].nget(), n_PCLK);
		ob5_latch[1].set(ob5_latch[0].nget(), n_PCLK);

		Z_COL2 = ob0_latch[1].nget();
		Z_COL3 = ob1_latch[1].nget();
		nZ_PRIO = ob5_latch[1].nget();
	}

	void FIFOLane::sim_CounterControl()
	{
		TriState PCLK = ppu->wire.PCLK;

		STEP = NOR(PCLK, ZH_FF.get());
		UPD = NOR(LOAD, STEP);
		// LOAD is obtained in the Lane Control circuit.
	}

	/// <summary>
	/// The optimized carry chain IS used.
	/// </summary>
	/// <returns></returns>
	TriState FIFOLane::sim_Counter()
	{
		TriState carry = TriState::One;
		TriState val_out[5]{};
		TriState unused{};

		for (size_t n = 0; n < 5; n++)
		{
			carry = down_cnt[n].sim(UPD, LOAD, STEP, ppu->wire.OB[n], carry, val_out[n]);
		}

		carry = NOR5(val_out);

		for (size_t n = 5; n < 8; n++)
		{
			carry = down_cnt[n].sim(UPD, LOAD, STEP, ppu->wire.OB[n], carry, unused);
		}

		return carry;
	}

	/// <summary>
	/// Switch the state of the circuit after the operation of the counter.
	/// </summary>
	/// <param name="Carry"></param>
	void FIFOLane::sim_CounterCarry(TriState Carry)
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState PCLK = ppu->wire.PCLK;
		TriState n_ZH = ppu->wire.n_ZH;

		ZH_FF.set(NOR(NOR3(n_PCLK, n_ZH, Carry), NOR(ZH_FF.get(), AND(PCLK, Carry))));
	}

	void FIFOLane::sim_PairedSREnable()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_VIS = ppu->fsm.nVIS;

		en_latch.set(NOR(ZH_FF.nget(), n_VIS), n_PCLK);
		n_EN = en_latch.nget();
		SR_EN = NOR(n_PCLK, n_EN);
	}

	void FIFOLane::sim_PairedSR(TriState n_TX[8], uint8_t packed_nTX)
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState shift_out[2]{};

		shift_out[0] = TriState::One;
		shift_out[1] = TriState::One;

		for (int n = 7; n >= 0; n--)
		{
			shift_out[0] = paired_sr[0][n].sim(n_PCLK, T_SR0, SR_EN, n_TX[n], shift_out[0]);
			shift_out[1] = paired_sr[1][n].sim(n_PCLK, T_SR1, SR_EN, n_TX[n], shift_out[1]);
		}

		nZ_COL0 = shift_out[0];
		nZ_COL1 = shift_out[1];
	}

	void FIFOLane::sim(TriState HSel, TriState n_TX[8], uint8_t packed_nTX, FIFOLaneOutput& ZOut)
	{
		sim_LaneControl(HSel);
		sim_CounterControl();

		auto CarryOut = sim_Counter();
		sim_CounterCarry(CarryOut);

		sim_PairedSREnable();
		sim_PairedSR(n_TX, packed_nTX);

		ZOut.nZ_COL0 = nZ_COL0;
		ZOut.nZ_COL1 = nZ_COL1;
		ZOut.Z_COL2 = Z_COL2;
		ZOut.Z_COL3 = Z_COL3;
		ZOut.nZ_PRIO = nZ_PRIO;
		ZOut.n_xEN = n_EN;
	}

	size_t FIFOLane::get_Counter()
	{
		size_t val = 0;
		for (size_t n = 0; n < 8; n++)
		{
			val |= ((down_cnt[n].get() == TriState::One) ? 1ULL : 0) << n;
		}
		return val;
	}

	TriState FIFO_CounterBit::sim(
		TriState Clock, TriState Load, TriState Step,
		TriState val_in,
		TriState carry_in,
		TriState& val_out)
	{
		keep_ff.set(MUX(Step,
			MUX(Load, MUX(Clock, TriState::Z, keep_ff.get()), val_in),
			step_latch.nget()));
		step_latch.set(MUX(carry_in, keep_ff.nget(), keep_ff.get()), Clock);
		TriState carry_out = NOR(keep_ff.get(), NOT(carry_in));
		val_out = keep_ff.get();
		return carry_out;
	}

	TriState FIFO_CounterBit::get()
	{
		return keep_ff.get();
	}

	TriState FIFO_SRBit::sim(TriState n_PCLK, TriState T_SR, TriState SR_EN,
		TriState nTx, TriState shift_in)
	{
		in_latch.set(MUX(SR_EN, MUX(T_SR, TriState::Z, nTx), shift_in), TriState::One);
		out_latch.set(in_latch.nget(), n_PCLK);
		TriState shift_out = out_latch.nget();
		return shift_out;
	}

#pragma endregion "FIFO Lane"

	// PPU FSM

	FSM::FSM(PPU* parent)
	{
		ppu = parent;
	}

	FSM::~FSM()
	{
	}

	void FSM::sim(TriState* HPLA, TriState* VPLA)
	{
		sim_DelayedH();
		sim_HPosLogic(HPLA, VPLA);
		sim_VPosLogic(VPLA);
		sim_VBlankInt();
		sim_EvenOdd(HPLA, VPLA);
		sim_CountersControl(HPLA, VPLA);

		prev_hpla = HPLA;
		prev_vpla = VPLA;
		Prev_n_OBCLIP = ppu->wire.n_OBCLIP;
		Prev_n_BGCLIP = ppu->wire.n_BGCLIP;
		Prev_BLACK = ppu->wire.BLACK;
	}

	void FSM::sim_DelayedH()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;

		for (size_t n = 0; n < 6; n++)
		{
			TriState H_bit = ppu->h->getBit(n);
			h_latch1[n].set(H_bit, n_PCLK);
			h_latch2[n].set(h_latch1[n].nget(), PCLK);
		}

		ppu->wire.H0_Dash = NOT(h_latch1[0].nget());
		ppu->wire.H0_Dash2 = h_latch2[0].nget();
		ppu->wire.nH1_Dash = h_latch1[1].nget();
		ppu->wire.H1_Dash2 = h_latch2[1].nget();
		ppu->wire.nH2_Dash = h_latch1[2].nget();
		ppu->wire.H2_Dash2 = h_latch2[2].nget();
		ppu->wire.H3_Dash2 = h_latch2[3].nget();
		ppu->wire.H4_Dash2 = h_latch2[4].nget();
		ppu->wire.H5_Dash2 = h_latch2[5].nget();
	}

	void FSM::sim_HPosLogic(TriState* HPLA, TriState* VPLA)
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_OBCLIP = ppu->wire.n_OBCLIP;
		TriState n_BGCLIP = ppu->wire.n_BGCLIP;
		TriState BLACK = ppu->wire.BLACK;

		// Front Porch appendix

		fp_latch1.set(HPLA[0], n_PCLK);
		fp_latch2.set(HPLA[1], n_PCLK);
		FPORCH_FF.set(NOR(fp_latch2.get(), NOR(fp_latch1.get(), FPORCH_FF.get())));
		TriState n_FPORCH = FPORCH_FF.get();

		// Finite States

		sev_latch1.set(HPLA[2], n_PCLK);
		sev_latch2.set(sev_latch1.nget(), PCLK);
		ppu->fsm.SEV = sev_latch2.nget();

		clip_latch1.set(HPLA[3], n_PCLK);
		clip_latch2.set(HPLA[4], n_PCLK);
		TriState temp1 = NOR(clip_latch1.get(), clip_latch2.nget());
		clpo_latch.set(temp1, PCLK);
		clpb_latch.set(temp1, PCLK);
		ppu->fsm.CLIP_O = NOR(n_OBCLIP, clpo_latch.get());
		ppu->fsm.CLIP_B = NOR(n_BGCLIP, clpb_latch.get());

		hpos_latch1.set(HPLA[5], n_PCLK);
		hpos_latch2.set(hpos_latch1.nget(), PCLK);
		ppu->fsm.ZHPOS = hpos_latch2.nget();

		eval_latch1.set(HPLA[6], n_PCLK);
		eev_latch1.set(HPLA[7], n_PCLK);
		TriState temp2 = NOR3(hpos_latch1.get(), eval_latch1.get(), eev_latch1.get());
		eval_latch2.set(temp2, PCLK);
		ppu->fsm.n_EVAL = NOT(eval_latch2.nget());

		eev_latch2.set(eev_latch1.nget(), PCLK);
		ppu->fsm.EEV = eev_latch2.nget();

		ioam_latch1.set(HPLA[8], n_PCLK);
		ioam_latch2.set(ioam_latch1.nget(), PCLK);
		ppu->fsm.IOAM2 = ioam_latch2.nget();

		objrd_latch1.set(HPLA[9], n_PCLK);
		objrd_latch2.set(objrd_latch1.nget(), PCLK);
		ppu->fsm.OBJ_READ = objrd_latch2.nget();

		nvis_latch1.set(HPLA[10], n_PCLK);
		nvis_latch2.set(nvis_latch1.nget(), PCLK);
		ppu->fsm.nVIS = NOT(nvis_latch2.nget());

		fnt_latch1.set(HPLA[11], n_PCLK);
		fnt_latch2.set(fnt_latch1.nget(), PCLK);
		ppu->fsm.nFNT = NOT(fnt_latch2.nget());

		ftb_latch1.set(HPLA[12], n_PCLK);
		fta_latch1.set(HPLA[13], n_PCLK);
		fo_latch1.set(HPLA[14], n_PCLK);
		fo_latch2.set(HPLA[15], n_PCLK);

		ftb_latch2.set(ftb_latch1.nget(), PCLK);
		fta_latch2.set(fta_latch1.nget(), PCLK);
		TriState temp3 = NOR(fo_latch1.get(), fo_latch2.get());
		fo_latch3.set(temp3, PCLK);
		ppu->fsm.nFO = fo_latch3.nget();
		ppu->fsm.FTB = NOR(ftb_latch2.get(), fo_latch3.get());
		ppu->fsm.FTA = NOR(fta_latch2.get(), fo_latch3.get());

		fat_latch1.set(HPLA[16], n_PCLK);
		ppu->fsm.FAT = NOR(temp3, fat_latch1.nget());

		// Video signal features

		bp_latch1.set(HPLA[17], n_PCLK);
		bp_latch2.set(HPLA[18], n_PCLK);
		BPORCH_FF.set(NOR(bp_latch2.get(), NOR(bp_latch1.get(), BPORCH_FF.get())));
		ppu->fsm.BPORCH = BPORCH_FF.get();

		hb_latch1.set(HPLA[19], n_PCLK);
		hb_latch2.set(HPLA[20], n_PCLK);
		HBLANK_FF.set(NOR(hb_latch2.get(), NOR(hb_latch1.get(), HBLANK_FF.get())));
		ppu->fsm.SCCNT = NOR(HBLANK_FF.nget(), BLACK);
		ppu->fsm.nHB = HBLANK_FF.get();

		sim_VSYNCEarly(VPLA);
		TriState VSYNC = ppu->fsm.VSYNC;

		cb_latch1.set(HPLA[21], n_PCLK);
		cb_latch2.set(HPLA[22], n_PCLK);
		BURST_FF.set(NOR(cb_latch2.get(), NOR(cb_latch1.get(), BURST_FF.get())));

		sync_latch1.set(BURST_FF.get(), PCLK);
		sync_latch2.set(NOT(n_FPORCH), PCLK);
		ppu->fsm.SYNC = NOR(sync_latch2.get(), VSYNC);
		ppu->fsm.BURST = NOR(sync_latch1.get(), ppu->fsm.SYNC);
	}

	/// <summary>
	/// The VSYNC signal is a uroboros that must be propagated as soon as the /HB signal is applied.
	/// </summary>
	/// <param name="VPLA"></param>
	void FSM::sim_VSYNCEarly(TriState* VPLA)
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState nHB = ppu->fsm.nHB;

		VSYNC_FF.set(NOR(AND(nHB, VPLA[0]), NOR(AND(nHB, VPLA[1]), VSYNC_FF.get())));
		vsync_latch1.set(NOR(nHB, VSYNC_FF.get()), PCLK);
		ppu->fsm.VSYNC = vsync_latch1.get();
	}

	void FSM::sim_VPosLogic(TriState* VPLA)
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState BPORCH = ppu->fsm.BPORCH;
		TriState BLACK = ppu->wire.BLACK;

		PICTURE_FF.set(NOR(AND(BPORCH, VPLA[2]), NOR(AND(BPORCH, VPLA[3]), PICTURE_FF.get())));
		pic_latch1.set(PICTURE_FF.get(), PCLK);
		pic_latch2.set(BPORCH, PCLK);
		ppu->fsm.n_PICTURE = NOT(NOR(pic_latch1.get(), pic_latch2.get()));

		vset_latch1.set(VPLA[4], n_PCLK);
		ppu->fsm.nVSET = vset_latch1.nget();

		vb_latch1.set(VPLA[5], n_PCLK);
		vb_latch2.set(VPLA[6], n_PCLK);
		VB_FF.set(NOR(vb_latch1.get(), NOR(vb_latch2.get(), VB_FF.get())));
		ppu->fsm.VB = NOT(VB_FF.nget());

		blnk_latch1.set(VPLA[7], n_PCLK);
		BLNK_FF.set(NOR(blnk_latch1.get(), NOR(vb_latch2.get(), BLNK_FF.get())));
		ppu->fsm.BLNK = NAND(NOT(BLNK_FF.get()), NOT(BLACK));

		vclr_latch1.set(VPLA[8], n_PCLK);
		// The second half for propagation to PCLK=1 is in sim_RESCL_early
	}

	void FSM::sim_VBlankInt()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_VSET = ppu->fsm.nVSET;
		TriState VBL = ppu->wire.VBL;
		TriState RESCL = ppu->fsm.RESCL;	// RESCL can already be used (simulated)
		TriState n_R2 = ppu->wire.n_R2;
		TriState n_DBE = ppu->wire.n_DBE;

		TriState R2_Enable = NOR(n_R2, n_DBE);

		edge_vset_latch1.set(n_VSET, PCLK);
		edge_vset_latch2.set(edge_vset_latch1.nget(), n_PCLK);
		TriState setFF = NOR3(n_PCLK, n_VSET, edge_vset_latch2.get());

		INT_FF.set(NOR3(NOR(setFF, INT_FF.get()), RESCL, R2_Enable));

		ppu->fsm.INT = NOR(NOT(VBL), NOT(INT_FF.get()));
		db_latch.set(INT_FF.get(), NOT(R2_Enable));
		TriState DB7 = MUX(R2_Enable, TriState::Z, db_latch.nget());
		ppu->SetDBBit(7, DB7);
	}

	/// <summary>
	/// The Even/Odd circuit is to the right of the V Decoder and does different things in different PPUs.
	/// </summary>
	void FSM::sim_EvenOdd(TriState* HPLA, TriState* VPLA)
	{
		switch (ppu->rev)
		{
		case Revision::RP2C02G:
		{
			TriState V8 = ppu->v->getBit(8);
			TriState RES = ppu->wire.RES;
			TriState RESCL = ppu->fsm.RESCL;	// RESCL can already be used (simulated)

			EvenOdd_FF1.set(NOT(NOT(MUX(V8, EvenOdd_FF2.get(), EvenOdd_FF1.get()))));
			TriState temp = NOR(RES, EvenOdd_FF2.get());
			EvenOdd_FF2.set(NOT(MUX(V8, temp, EvenOdd_FF1.get())));

			ppu->wire.EvenOddOut = NOR3(temp, NOT(HPLA[5]), NOT(RESCL));
			break;
		}

		case Revision::RP2C07_0:
		{
			TriState n_PCLK = ppu->wire.n_PCLK;
			TriState PCLK = ppu->wire.PCLK;
			TriState BLNK = ppu->fsm.BLNK;
			TriState H0_D = ppu->wire.H0_Dash;

			EvenOdd_latch1.set(VPLA[9], n_PCLK);
			EvenOdd_latch2.set(VPLA[8], n_PCLK);
			EvenOdd_FF1.set(NOR(EvenOdd_latch2.get(), NOR(EvenOdd_FF1.get(), EvenOdd_latch1.get())));
			EvenOdd_latch3.set(EvenOdd_FF1.nget(), PCLK);

			ppu->wire.EvenOddOut = NOR3(NOT(H0_D), NAND(BLNK, EvenOdd_latch3.nget()), n_PCLK);
			break;
		}

		// TBD: Only RP2C04-0003 has a photo so far. The Even/Odd circuit is obviously switched off, as the Dot Crawl is not required.
		// In other RGB PPU probably similar, but until there is no photo, we will not engage in speculation.

		case Revision::RP2C04_0003:
		{
			ppu->wire.EvenOddOut = TriState::Zero;
			break;
		}

		case Revision::UMC_UA6538:
		{
			ppu->wire.EvenOddOut = TriState::Zero;
			break;
		}
		}
	}

	void FSM::sim_CountersControl(TriState* HPLA, TriState* VPLA)
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState EvenOddOut = ppu->wire.EvenOddOut;

		switch (ppu->rev)
		{
			// PAL PPU does not use EvenOddOut.

		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
		{
			ctrl_latch1.set(NOT(HPLA[23]), n_PCLK);
			ctrl_latch2.set(VPLA[8], n_PCLK);
		}
		break;

		default:
		{
			ctrl_latch1.set(NOR(HPLA[23], EvenOddOut), n_PCLK);
			ctrl_latch2.set(VPLA[2], n_PCLK);
		}
		break;
		}

		ppu->wire.HC = ctrl_latch1.nget();
		ppu->wire.VC = NOR(NOT(ppu->wire.HC), ctrl_latch2.nget());
	}

	/// <summary>
	/// The RESCL signal is required very early for Reset_FF, so this part of the circuit is called as early as possible.
	/// </summary>
	void FSM::sim_RESCL_early()
	{
		vclr_latch2.set(vclr_latch1.nget(), ppu->wire.PCLK);
		ppu->fsm.RESCL = vclr_latch2.nget();
	}

	TriState FSM::get_VB()
	{
		return NOT(VB_FF.nget());
	}

	TriState FSM::get_BLNK(TriState BLACK)
	{
		return NAND(NOT(BLNK_FF.get()), NOT(BLACK));
	}

	// H/V Counters

	TriState HVCounterBit::sim(TriState Carry, TriState CLR)
	{
		// The CLR actually makes sense as `Load`. But in the PPU all the inputs for the `Load` equivalent are connected to Vss, so we use the name CLR.

		ff.set(MUX(ppu->wire.PCLK, NOR(NOT(ff.get()), ppu->wire.RES), NOR(latch.get(), CLR)));
		latch.set(MUX(Carry, NOT(ff.get()), ff.get()), NOT(ppu->wire.PCLK));
		TriState CarryOut = NOR(NOT(ff.get()), NOT(Carry));
		return CarryOut;
	}

	TriState HVCounterBit::getOut()
	{
		TriState out = NOR(NOT(ff.get()), ppu->wire.RES);
		return out;
	}

	void HVCounterBit::set(TriState val)
	{
		ff.set(val);
	}

	HVCounter::HVCounter(PPU* parent, size_t bits)
	{
		assert(bits <= bitCountMax);

		ppu = parent;
		bitCount = bits;

		for (size_t n = 0; n < bitCount; n++)
		{
			bit[n] = new HVCounterBit(parent);
		}
	}

	HVCounter::~HVCounter()
	{
		for (size_t n = 0; n < bitCount; n++)
		{
			delete bit[n];
		}
	}

	void HVCounter::sim(TriState Carry, TriState CLR)
	{
		for (size_t n = 0; n < bitCount; n++)
		{
			Carry = bit[n]->sim(Carry, CLR);
		}
	}

	size_t HVCounter::get()
	{
		size_t val = 0;

		for (size_t n = 0; n < bitCount; n++)
		{
			auto bitVal = bit[n]->getOut();
			if (bitVal == TriState::One)
			{
				val |= (1ULL << n);
			}
		}

		return val;
	}

	void HVCounter::set(size_t val)
	{
		for (size_t n = 0; n < bitCount; n++)
		{
			auto bitVal = (val >> n) & 1 ? TriState::One : TriState::Zero;
			bit[n]->set(bitVal);
		}
	}

	TriState HVCounter::getBit(size_t n)
	{
		return bit[n]->getOut();
	}

	// H/V Decoder

	HVDecoder::HVDecoder(PPU* parent)
	{
		char hplaName[0x20] = { 0 };
		char vplaName[0x20] = { 0 };

		ppu = parent;

		// Select the number of outputs. Among the PPUs studied there are some that differ in the number of outputs.

		switch (ppu->rev)
		{
		case Revision::RP2C02G:
		case Revision::RP2C04_0003:
			vpla_outputs = 9;
			break;
		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
			// An additional output is used for modified EVEN/ODD logic.
			vpla_outputs = 10;
			break;
		}

		// Generate PLA dump name according to the PPU revision.

		sprintf(hplaName, "HPLA_%s.bin", ppu->RevisionToStr(ppu->rev));
		sprintf(vplaName, "VPLA_%s.bin", ppu->RevisionToStr(ppu->rev));

		// Create PLA instances

		hpla = new PLA(hpla_inputs, hpla_outputs, hplaName);
		vpla = new PLA(vpla_inputs, vpla_outputs, vplaName);

		// Set matrix

		size_t* h_bitmask = nullptr;
		size_t* v_bitmask = nullptr;

		switch (ppu->rev)
		{
		case Revision::RP2C02G:
		case Revision::RP2C04_0003:
		{
			static size_t RP2C02G_HDecoder[] = {		// Undefined behavior in GCC if you do not specify static
				0b01101010011001010100,
				0b01101010101010101000,
				0b10100110101010100101,
				0b00101010101000000000,
				0b10000000000000000010,
				0b01100110011010010101,
				0b10101001010101010101,
				0b00010101010101010101,
				0b10101000000000000001,
				0b01101000000000000001,
				0b10000000000000000011,
				0b00000000000010100001,
				0b00000000000001010000,
				0b00000000000001100000,
				0b01000110100000000001,
				0b10000000000000000001,
				0b00000000000010010000,
				0b01101010100101011000,
				0b01100110100110101000,
				0b01101010011001010100,
				0b01101001011010101000,
				0b01100110101010010100,
				0b01101001011001101000,
				0b01100110011001101000,
			};

			static size_t RP2C02G_VDecoder[] = {
				0b000101010110010101,
				0b000101010110011010,
				0b011010101010011001,
				0b000101010110101001,
				0b000101010110101001,
				0b101010101010101010,
				0b000101010110101010,
				0b011010101010011001,
				0b011010101010011001,
			};

			h_bitmask = RP2C02G_HDecoder;
			v_bitmask = RP2C02G_VDecoder;
		}
		break;

		case Revision::RP2C07_0:
		{
			static size_t RP2C07_HDecoder[] = {
				0b01101010011001100100,
				0b01101010101010101000,
				0b10100110101010100101,
				0b00101010101000000000,
				0b10000000000000000010,
				0b01100110011010010101,
				0b10101001010101010101,
				0b00010101010101010101,
				0b10101000000000000001,
				0b01101000000000000001,
				0b10000000000000000011,
				0b00000000000010100001,
				0b00000000000001010000,
				0b00000000000001100000,
				0b01000110100000000001,
				0b10000000000000000001,
				0b00000000000010010000,
				0b01101010101010101000,
				0b10101010101001101000,
				0b01101010011001100100,
				0b01101001100101011000,
				0b01100110101010100100,
				0b01101001011010011000,
				0b01100110011001101000,
			};

			static size_t RP2C07_VDecoder[] = {
				0b011010100110101010,
				0b011010101001011001,
				0b101010101010101001,
				0b000101010110101010,
				0b000101010110101001,
				0b101010101010101010,
				0b000101010110101010,
				0b011010010110010101,
				0b011010010110010101,
				0b011010101001101001,
			};

			h_bitmask = RP2C07_HDecoder;
			v_bitmask = RP2C07_VDecoder;
		}
		break;

		case Revision::UMC_UA6538:
		{
			static size_t UMC_UA6538_HDecoder[] = {		// Same as PAL PPU
				0b01101010011001100100,
				0b01101010101010101000,
				0b10100110101010100101,
				0b00101010101000000000,
				0b10000000000000000010,
				0b01100110011010010101,
				0b10101001010101010101,
				0b00010101010101010101,
				0b10101000000000000001,
				0b01101000000000000001,
				0b10000000000000000011,
				0b00000000000010100001,
				0b00000000000001010000,
				0b00000000000001100000,
				0b01000110100000000001,
				0b10000000000000000001,
				0b00000000000010010000,
				0b01101010101010101000,
				0b10101010101001101000,
				0b01101010011001100100,
				0b01101001100101011000,
				0b01100110101010100100,
				0b01101001011010011000,
				0b01100110011001101000,
			};

			static size_t UMC_UA6538_VDecoder[] = {
				0b011010100110101010,
				0b011010101001011001,
				0b101010101010101001,
				0b000101010110101010,
				0b011010011010100101,		// triggered 50 lines later
				0b101010101010101010,
				0b000101010110101010,
				0b011010010110010101,
				0b011010010110010101,
				0b011010101001101001,
			};

			h_bitmask = UMC_UA6538_HDecoder;
			v_bitmask = UMC_UA6538_VDecoder;
		}
		break;

		// TBD: Add PLA for the rest of the PPU studied.
		}

		if (h_bitmask != nullptr)
		{
			hpla->SetMatrix(h_bitmask);
		}

		if (v_bitmask != nullptr)
		{
			vpla->SetMatrix(v_bitmask);
		}
	}

	HVDecoder::~HVDecoder()
	{
		delete hpla;
		delete vpla;
	}

	void HVDecoder::sim_HDecoder(TriState VB, TriState BLNK, TriState** outputs)
	{
		HDecoderInput input{};

		input.H8 = ppu->h->getBit(8);
		input.n_H8 = NOT(ppu->h->getBit(8));
		input.H7 = ppu->h->getBit(7);
		input.n_H7 = NOT(ppu->h->getBit(7));
		input.H6 = ppu->h->getBit(6);
		input.n_H6 = NOT(ppu->h->getBit(6));
		input.H5 = ppu->h->getBit(5);
		input.n_H5 = NOT(ppu->h->getBit(5));
		input.H4 = ppu->h->getBit(4);
		input.n_H4 = NOT(ppu->h->getBit(4));
		input.H3 = ppu->h->getBit(3);
		input.n_H3 = NOT(ppu->h->getBit(3));
		input.H2 = ppu->h->getBit(2);
		input.n_H2 = NOT(ppu->h->getBit(2));
		input.H1 = ppu->h->getBit(1);
		input.n_H1 = NOT(ppu->h->getBit(1));
		input.H0 = ppu->h->getBit(0);
		input.n_H0 = NOT(ppu->h->getBit(0));

		input.VB = VB == TriState::One ? 1 : 0;
		input.BLNK = BLNK == TriState::One ? 1 : 0;

		hpla->sim(input.packed_bits, outputs);
	}

	void HVDecoder::sim_VDecoder(TriState** outputs)
	{
		VDecoderInput input{};

		input.V8 = ppu->v->getBit(8);
		input.n_V8 = NOT(ppu->v->getBit(8));
		input.V7 = ppu->v->getBit(7);
		input.n_V7 = NOT(ppu->v->getBit(7));
		input.V6 = ppu->v->getBit(6);
		input.n_V6 = NOT(ppu->v->getBit(6));
		input.V5 = ppu->v->getBit(5);
		input.n_V5 = NOT(ppu->v->getBit(5));
		input.V4 = ppu->v->getBit(4);
		input.n_V4 = NOT(ppu->v->getBit(4));
		input.V3 = ppu->v->getBit(3);
		input.n_V3 = NOT(ppu->v->getBit(3));
		input.V2 = ppu->v->getBit(2);
		input.n_V2 = NOT(ppu->v->getBit(2));
		input.V1 = ppu->v->getBit(1);
		input.n_V1 = NOT(ppu->v->getBit(1));
		input.V0 = ppu->v->getBit(0);
		input.n_V0 = NOT(ppu->v->getBit(0));

		vpla->sim(input.packed_bits, outputs);
	}

	// Multiplexer

	void Mux::sim()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_ZCOL0 = ppu->wire.n_ZCOL0;
		TriState n_ZCOL1 = ppu->wire.n_ZCOL1;
		TriState ZCOL2 = ppu->wire.ZCOL2;
		TriState ZCOL3 = ppu->wire.ZCOL3;
		TriState TH_MUX = ppu->wire.TH_MUX;

		step1[0].set(n_ZCOL0, PCLK);
		step1[1].set(n_ZCOL1, PCLK);
		step1[2].set(NOT(ZCOL2), PCLK);
		step1[3].set(NOT(ZCOL3), PCLK);

		sim_MuxControl();

		for (size_t n = 0; n < 4; n++)
		{
			dir_color[n].set(ppu->wire.THO[n], PCLK);
			ppu->wire.n_EXT_Out[n] = NOT(MUX(OCOL, ppu->wire.BGC[n], step1[n].nget()));
			step2[n].set(ppu->wire.n_EXT_Out[n], n_PCLK);
			step3[n].set(MUX(EXT, step2[n].nget(), ppu->wire.EXT_In[n]), PCLK);
			ppu->wire.CGA[n] = NOT(MUX(TH_MUX, step3[n].nget(), dir_color[n].nget()));
		}

		tho4_latch.set(ppu->wire.THO[4], PCLK);
		pal4_latch.set(MUX(TH_MUX, n_PAL4, tho4_latch.nget()), PCLK);
		ppu->wire.CGA[4] = pal4_latch.nget();

		sim_Spr0Hit();
	}

	void Mux::sim_MuxControl()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_ZPRIO = ppu->wire.n_ZPRIO;
		TriState OBJC[4]{};

		for (size_t n = 0; n < 4; n++)
		{
			OBJC[n] = step1[n].nget();
		}

		zprio_latch.set(NOT(n_ZPRIO), PCLK);
		auto bgc01 = NOR(ppu->wire.BGC[0], ppu->wire.BGC[1]);
		auto objc01 = NOR(OBJC[0], OBJC[1]);
		OCOL = AND(NAND(NOT(bgc01), zprio_latch.nget()), NOT(objc01));

		bgc_latch.set(NOT(bgc01), n_PCLK);
		zcol_latch.set(NOT(objc01), n_PCLK);
		EXT = NOR(bgc_latch.get(), zcol_latch.get());

		ocol_latch.set(OCOL, n_PCLK);
		n_PAL4 = ocol_latch.nget();
	}

	void Mux::sim_Spr0Hit()
	{
		TriState n[5]{};
		n[0] = ppu->wire.PCLK;
		n[1] = NOR(ppu->wire.BGC[0], ppu->wire.BGC[1]);
		n[2] = ppu->wire.n_SPR0HIT;
		n[3] = ppu->wire.n_SPR0_EV;
		n[4] = ppu->fsm.nVIS;
		TriState STRIKE = NOR5(n);

		StrikeFF.set(NOR(ppu->fsm.RESCL, NOR(StrikeFF.get(), STRIKE)));
		TriState enable = NOT(NOT(NOR(ppu->wire.n_R2, ppu->wire.n_DBE)));
		TriState DB6 = MUX(enable, TriState::Z, StrikeFF.get());
		ppu->SetDBBit(6, DB6);
	}

	// OAM

	OAM::OAM(PPU* parent)
	{
		ppu = parent;

		for (size_t n = 0; n < 8; n++)
		{
			switch (ppu->rev)
			{
				// TBD: Check how things are in other RGB PPUs.

			case Revision::RP2C04_0003:
				ob[n] = new OAMBufferBit_RGB(ppu);
				break;

			default:
				ob[n] = new OAMBufferBit(ppu);
				break;
			}
		}

		for (size_t n = 0; n < num_lanes; n++)
		{
			lane[n] = new OAMLane(ppu, n == 2 || n == 6);
		}
	}

	OAM::~OAM()
	{
		for (size_t n = 0; n < 8; n++)
		{
			delete ob[n];
		}

		for (size_t n = 0; n < num_lanes; n++)
		{
			delete lane[n];
		}
	}

	void OAM::sim()
	{
		OAMLane* lane = sim_AddressDecoder();
		sim_OBControl();
		sim_OB(lane);
	}

	/// <summary>
	/// From the input OAM address (n_OAM0-7 + OAM8) determines the column (lane) and row number (ROW).
	/// During PCLK the precharge is made and all ROWx outputs are 0. This situation is handled by returning nullptr instead of lane.
	/// </summary>
	/// <returns></returns>
	OAMLane* OAM::sim_AddressDecoder()
	{
		size_t col = 0;

		if (ppu->wire.OAM8 == TriState::One)
		{
			col = 8;
		}
		else
		{
			for (size_t n = 0; n < 3; n++)
			{
				if (ppu->wire.n_OAM[n] == TriState::Zero)
				{
					col |= (1ULL << n);
				}
			}
		}

		ROW = 0;

		for (size_t n = 0; n < 5; n++)
		{
			if (ppu->wire.n_OAM[3 + n] == TriState::Zero)
			{
				ROW |= (1ULL << n);
			}
		}

		ROW = RowMap(ROW);

		return ppu->wire.PCLK == TriState::One ? nullptr : lane[col];
	}

	void OAM::sim_OBControl()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState BLNK = ppu->fsm.BLNK;
		TriState SPR_OV = ppu->wire.SPR_OV;
		TriState OAMCTR2 = ppu->wire.OAMCTR2;
		TriState H0_DD = ppu->wire.H0_Dash2;
		TriState n_VIS = ppu->fsm.nVIS;

		OB_OAM = NOR(n_PCLK, BLNK);

		TriState temp[6]{};
		temp[0] = BLNK;
		temp[1] = SPR_OV;
		temp[2] = PCLK;
		temp[3] = NOT(H0_DD);
		temp[4] = OAMCTR2;
		temp[5] = n_VIS;
		ppu->wire.n_WE = NOR(ppu->wire.OFETCH, NOR6(temp));
	}

	/// <summary>
	/// The OFETCH signal must be applied before the Sprite Eval simulation.
	/// </summary>
	void OAM::sim_OFETCH()
	{
		switch (ppu->rev)
		{
			// TBD: Find out how the rest of the RGB PPUs are doing

		case Revision::RP2C04_0003:
			sim_OFETCH_RGB_PPU();
			break;

		default:
			sim_OFETCH_Default();
			break;
		}
	}

	void OAM::sim_OFETCH_Default()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_W4 = ppu->wire.n_W4;
		TriState n_DBE = ppu->wire.n_DBE;
		auto W4_Enable = NOR(n_W4, n_DBE);

		latch[0].set(OFETCH_FF.nget(), n_PCLK);
		latch[1].set(latch[0].nget(), PCLK);
		latch[2].set(latch[1].nget(), n_PCLK);
		latch[3].set(latch[2].nget(), PCLK);

		ppu->wire.OFETCH = NOR(latch[1].nget(), latch[3].get());

		W4_FF.set(NOR(W4_Enable, NOR(W4_FF.get(), latch[2].nget())));

		// OFETCH FF can be simulated after all because it is updated during PCLK = 1 and the input latch on its output is during PCLK = 0.

		OFETCH_FF.set(MUX(PCLK, NOT(NOT(OFETCH_FF.get())), NOR(W4_Enable, W4_FF.get())));
	}

	void OAM::sim_OFETCH_RGB_PPU()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_W4 = ppu->wire.n_W4;
		TriState n_DBE = ppu->wire.n_DBE;
		auto W4_Enable = NOR(n_W4, n_DBE);

		// The number of latches is 2 less. W4_FF is missing.

		latch[2].set(OFETCH_FF.nget(), n_PCLK);
		latch[3].set(latch[2].nget(), PCLK);

		ppu->wire.OFETCH = NOR(OFETCH_FF.nget(), latch[3].get());

		OFETCH_FF.set(MUX(PCLK, NOT(NOT(OFETCH_FF.get())), W4_Enable));
	}

	void OAM::sim_OB(OAMLane* lane)
	{
		TriState n_WE = ppu->wire.n_WE;

		for (size_t n = 0; n < 8; n++)
		{
			ob[n]->sim(lane, ROW, n, OB_OAM, n_WE);
		}
	}

	TriState OAMCell::get()
	{
		size_t pclkNow = ppu->GetPCLKCounter();
		if (pclkNow >= (savedPclk + pclksToDecay))
		{
			switch (ppu->oam->GetOamDecayBehavior())
			{
			case OAMDecayBehavior::Keep:
				return decay_ff.get();

			case OAMDecayBehavior::ToZero:
				return TriState::Zero;

			case OAMDecayBehavior::ToOne:
				return TriState::One;

			case OAMDecayBehavior::Evaporate:
				return TriState::Z;

			case OAMDecayBehavior::Randomize:
#ifdef _WIN32
				// Year 2022. C++ still doesn't contain a standard way to get TimeStamp.
				return FromByte(__rdtsc() & 1);
#else
				// TBD
				return TriState::Zero;
#endif

			default:
				break;
			}

			return TriState::Z;
		}
		else
		{
			return decay_ff.get();
		}
	}

	void OAMCell::set(TriState val)
	{
		savedPclk = ppu->GetPCLKCounter();
		DetermineDecay();
		decay_ff.set(val);
	}

	void OAMCell::DetermineDecay()
	{
		// TBD: You can tweak individual statistical behavior (PRNG, in range, depending on ambient "temperature", etc.)

		pclksToDecay = 1000000;	// now just a constant
	}

	void OAMCell::SetTopo(OAMCellTopology place, size_t bank_num)
	{
		topo = place;
		bank = bank_num;
	}

	void OAMBufferBit::sim(OAMLane* lane, size_t row, size_t bit_num, TriState OB_OAM, TriState n_WE)
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_R4 = ppu->wire.n_R4;
		TriState n_DBE = ppu->wire.n_DBE;
		TriState BLNK = ppu->fsm.BLNK;
		TriState I_OAM2 = ppu->fsm.IOAM2;

		auto n_R4_Enable = NOT(NOR(n_R4, n_DBE));

		TriState FromOAM = TriState::Z;
		if (lane != nullptr)
		{
			lane->sim(row, bit_num, FromOAM);
		}

		Input_FF.set(MUX(PCLK, FromOAM, TriState::Zero));
		OB_FF.set(NOT(MUX(PCLK, NOR(I_OAM2, Input_FF.get()), NOT(OB_FF.get()))));
		auto OBOut = OB_FF.get();
		ppu->wire.OB[bit_num] = OBOut;
		R4_out_latch.set(OBOut, PCLK);
		ppu->SetDBBit(bit_num, MUX(NOT(n_R4_Enable), TriState::Z, R4_out_latch.get()));
		out_latch.set(MUX(BLNK, MUX(OB_OAM, TriState::Z, OBOut), ppu->GetDBBit(bit_num)), TriState::One);

		if (lane != nullptr)
		{
			TriState val_out = MUX(NOT(n_WE), TriState::Z, out_latch.get());
			lane->sim(row, bit_num, val_out);
		}
	}

	TriState OAMBufferBit::get()
	{
		return OB_FF.get();
	}

	void OAMBufferBit::set(TriState val)
	{
		return OB_FF.set(val);
	}

	OAMLane::OAMLane(PPU* parent, bool SkipAttrBits)
	{
		ppu = parent;
		skip_attr_bits = SkipAttrBits;

		for (size_t n = 0; n < cells_per_lane; n++)
		{
			cells[n] = new OAMCell(ppu);

			size_t bit = n / 8;
			size_t bank = bit < 4 ? 0 : 1;

			OAMCellTopology topo = OAMCellTopology::Middle;

			// TBD: Topology

			cells[n]->SetTopo(topo, bank);
		}
	}

	OAMLane::~OAMLane()
	{
		for (size_t n = 0; n < cells_per_lane; n++)
		{
			delete cells[n];
		}
	}

	/// <summary>
	/// Repeats the logic of the real cell.
	/// If the input is z - place the cell value on it (Read), otherwise - write a new value to the cell (Write).
	/// </summary>
	void OAMLane::sim(size_t Row, size_t bit_num, TriState& inOut)
	{
		OAMCell* cell = cells[8 * Row + bit_num];

		// Skip unused bits 2-4 for columns 2/6, which correspond to the attribute byte.

		bool skip_bit = skip_attr_bits && (bit_num == 2 || bit_num == 3 || bit_num == 4);

		if (!skip_bit)
		{
			if (inOut == TriState::Z)
			{
				inOut = cell->get();
			}
			else
			{
				cell->set(inOut);
			}
		}
	}

	/// <summary>
	/// Remaps the logical ROW value to the physical row position.
	/// Physical topology: 0 - left, 31 - right.
	/// The need for this method arose because the NTSC PPU OAM address is in inverse logic (/OAM0-7) and the PAL PPU is in direct logic (OAM0-7).
	/// Therefore the physical arrangement of the rows on the chip is different.
	/// </summary>
	/// <param name="n"></param>
	/// <returns></returns>
	size_t OAM::RowMap(size_t n)
	{
		// TBD.

		// Might come in handy when we add topology support. But until the cell degradation timings are calculated, it makes no sense.

		return n;
	}

	TriState OAM::get_OB(size_t bit_num)
	{
		return ob[bit_num]->get();
	}

	void OAM::set_OB(size_t bit_num, TriState val)
	{
		ob[bit_num]->set(val);
	}

	void OAM::SetOamDecayBehavior(OAMDecayBehavior behavior)
	{
		decay_behav = behavior;
	}

	OAMDecayBehavior OAM::GetOamDecayBehavior()
	{
		return decay_behav;
	}

	/// <summary>
	/// The different version for RGB PPU (the one studied) is that OAM is Write-Only.
	/// </summary>
	void OAMBufferBit_RGB::sim(OAMLane* lane, size_t row, size_t bit_num, TriState OB_OAM, TriState n_WE)
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState BLNK = ppu->fsm.BLNK;
		TriState I_OAM2 = ppu->fsm.IOAM2;

		TriState FromOAM = TriState::Z;
		if (lane != nullptr)
		{
			lane->sim(row, bit_num, FromOAM);
		}

		Input_FF.set(MUX(PCLK, FromOAM, TriState::Zero));
		OB_FF.set(NOT(MUX(PCLK, NOR(I_OAM2, Input_FF.get()), NOT(OB_FF.get()))));
		auto OBOut = OB_FF.get();
		ppu->wire.OB[bit_num] = OBOut;
		out_latch.set(MUX(BLNK, MUX(OB_OAM, TriState::Z, OBOut), ppu->GetDBBit(bit_num)), TriState::One);

		if (lane != nullptr)
		{
			TriState val_out = MUX(NOT(n_WE), TriState::Z, out_latch.get());
			lane->sim(row, bit_num, val_out);
		}
	}

	// Picture Address Register

	PAR::PAR(PPU* parent)
	{
		ppu = parent;
	}

	PAR::~PAR()
	{
	}

	void PAR::sim()
	{
		sim_Control();
		sim_VInv();
		sim_ParBitsInv();
		sim_ParBit4();
		sim_ParBits();

		TriState n_H1_D = ppu->wire.nH1_Dash;
		ppu->wire.PAT_ADR[3] = NOT(n_H1_D);
		ppu->wire.PAT_ADR[13] = TriState::Zero;
	}

	void PAR::sim_Control()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState H0_DD = ppu->wire.H0_Dash2;
		TriState nF_NT = ppu->fsm.nFNT;
		TriState BGSEL = ppu->wire.BGSEL;
		TriState OBSEL = ppu->wire.OBSEL;
		TriState O8_16 = ppu->wire.O8_16;
		TriState OBJ_READ = ppu->fsm.OBJ_READ;

		fnt_latch.set(NOT(NOR(NOT(H0_DD), nF_NT)), n_PCLK);
		O = NOR(fnt_latch.get(), n_PCLK);
		ob0_latch.set(ppu->wire.OB[0], O);
		pad12_latch.set(MUX(OBJ_READ, BGSEL, MUX(O8_16, OBSEL, ob0_latch.nget())), n_PCLK);

		ppu->wire.PAT_ADR[12] = pad12_latch.nget();
	}

	void PAR::sim_VInv()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_OBJ_RD_ATTR = ppu->wire.n_OBJ_RD_ATTR;

		VINV_FF.set(MUX(n_PCLK, MUX(NOR(n_PCLK, n_OBJ_RD_ATTR), TriState::Z, ppu->wire.OB[7]), NOT(NOT(VINV_FF.get()))));
		VINV = NOT(NOT(VINV_FF.get()));
	}

	void PAR::sim_ParBitsInv()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState OBJ_READ = ppu->fsm.OBJ_READ;

		for (size_t n = 0; n < 4; n++)
		{
			inv_bits[n].sim(n_PCLK, O, VINV, ppu->wire.OV[n], inv_bits_out[n]);
		}

		pad0_latch.set(MUX(OBJ_READ, ppu->wire.n_FVO[0], inv_bits_out[0]), n_PCLK);
		pad1_latch.set(MUX(OBJ_READ, ppu->wire.n_FVO[1], inv_bits_out[1]), n_PCLK);
		pad2_latch.set(MUX(OBJ_READ, ppu->wire.n_FVO[2], inv_bits_out[2]), n_PCLK);

		ppu->wire.PAT_ADR[0] = pad0_latch.nget();
		ppu->wire.PAT_ADR[1] = pad1_latch.nget();
		ppu->wire.PAT_ADR[2] = pad2_latch.nget();
	}

	void PAR::sim_ParBit4()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState val_OB = ppu->wire.OB[0];
		TriState val_PD = ppu->GetPDBit(0);
		TriState OBJ_READ = ppu->fsm.OBJ_READ;
		TriState O8_16 = ppu->wire.O8_16;
		TriState val_OBPrev = inv_bits_out[3];

		pdin_latch.set(NOT(val_PD), n_PCLK);
		pdout_latch.set(pdin_latch.nget(), O);
		ob_latch.set(val_OB, O);
		pad4_latch.set(MUX(OBJ_READ, pdout_latch.nget(), MUX(O8_16, ob_latch.nget(), val_OBPrev)), n_PCLK);
		ppu->wire.PAT_ADR[4] = pad4_latch.nget();
	}

	void PAR::sim_ParBits()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState OBJ_READ = ppu->fsm.OBJ_READ;

		for (size_t n = 0; n < 7; n++)
		{
			bits[n].sim(n_PCLK, O, ppu->wire.OB[n + 1], ppu->GetPDBit(n + 1), OBJ_READ, ppu->wire.PAT_ADR[5 + n]);
		}
	}

	void ParBitInv::sim(TriState n_PCLK, TriState O, TriState INV, TriState val_in,
		TriState& val_out)
	{
		in_latch.set(val_in, n_PCLK);
		out_latch.set(in_latch.nget(), O);
		val_out = NOT(MUX(INV, out_latch.nget(), NOT(out_latch.nget())));
	}

	void ParBit::sim(TriState n_PCLK, TriState O, TriState val_OB, TriState val_PD, TriState OBJ_READ,
		TriState& PADx)
	{
		pdin_latch.set(NOT(val_PD), n_PCLK);
		pdout_latch.set(pdin_latch.nget(), O);
		ob_latch.set(val_OB, O);
		padx_latch.set(MUX(OBJ_READ, pdout_latch.nget(), ob_latch.nget()), n_PCLK);
		PADx = padx_latch.nget();
	}

	// Tile Counters (nesdev `v`)

	TileCnt::TileCnt(PPU* parent)
	{
		ppu = parent;
	}

	TileCnt::~TileCnt()
	{
	}

	void TileCnt::sim()
	{
		sim_CountersControl();
		sim_CountersCarry();
		sim_FVCounter();
		sim_NTCounters();
		sim_TVCounter();
		sim_THCounter();
	}

	void TileCnt::sim_CountersControl()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState PCLK = ppu->wire.PCLK;
		TriState W6_2_Enable = NOR(ppu->wire.n_W6_2, ppu->wire.n_DBE);
		TriState SC_CNT = ppu->fsm.SCCNT;
		TriState RESCL = ppu->fsm.RESCL;
		TriState E_EV = ppu->fsm.EEV;
		TriState TSTEP = ppu->wire.TSTEP;
		TriState F_TB = ppu->fsm.FTB;
		TriState H0_DD = ppu->wire.H0_Dash2;

		auto Temp93 = NOT(w62_latch.nget());
		W62_FF1.set(NOR(AND(n_PCLK, Temp93), NOR(W62_FF1.get(), W6_2_Enable)));
		W62_FF2.set(MUX(PCLK, NOR(W62_FF1.nget(), W6_2_Enable), NOT(NOT(W62_FF2.get()))));
		w62_latch.set(NOT(NOT(W62_FF2.get())), PCLK);
		Temp93 = NOT(w62_latch.nget());

		sccnt_latch.set(SC_CNT, PCLK);
		TVLOAD = NOR(NOR(AND(NOT(sccnt_latch.nget()), RESCL), Temp93), NOT(n_PCLK));

		eev_latch1.set(E_EV, n_PCLK);
		eev_latch2.set(eev_latch1.nget(), PCLK);
		THLOAD = NOR(NOR(Temp93, eev_latch2.nget()), PCLK);

		THSTEP = NOR(NOR(TSTEP, AND(F_TB, H0_DD)), PCLK);
		TVSTEP = NOR(NOR(E_EV, TSTEP), PCLK);
	}

	void TileCnt::sim_CountersCarry()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState PCLK = ppu->wire.PCLK;
		TriState BLNK = ppu->fsm.BLNK;
		TriState I1_32 = ppu->wire.I1_32;
		TriState temp[7]{};

		// THZ/THZB

		temp[0] = NOT(BLNK);
		temp[1] = ppu->wire.n_THO[0];
		temp[2] = ppu->wire.n_THO[1];
		temp[3] = ppu->wire.n_THO[2];
		temp[4] = ppu->wire.n_THO[3];
		temp[5] = ppu->wire.n_THO[4];
		THZB = NOR6(temp);

		temp[0] = BLNK;
		THZ = NOR6(temp);

		// FVIN

		auto n_FVIN = NOR(NOT(BLNK), AND(NTVO, NOT(NOT(BLNK))));
		FVIN = NOT(n_FVIN);

		// TVIN/THIN

		TriState fvz[5]{};
		fvz[0] = n_FVIN;
		fvz[1] = BLNK;
		fvz[2] = ppu->wire.n_FVO[0];
		fvz[3] = ppu->wire.n_FVO[1];
		fvz[4] = ppu->wire.n_FVO[2];
		FVZ = NOR5(fvz);

		TVIN = NOT(NOR3(FVZ, THZB, AND(BLNK, I1_32)));
		THIN = NAND(BLNK, I1_32);

		// TVZ/TVZB

		temp[0] = NOT(BLNK);
		temp[1] = NOT(TVIN);
		temp[2] = ppu->wire.n_TVO[0];
		temp[3] = ppu->wire.n_TVO[1];
		temp[4] = ppu->wire.n_TVO[2];
		temp[5] = ppu->wire.n_TVO[3];
		temp[6] = ppu->wire.n_TVO[4];
		TVZB = NOR7(temp);

		temp[0] = BLNK;
		temp[3] = ppu->wire.TVO[1];			// !!!
		TVZ = NOR7(temp);

		// NTHIN/NTVIN

		NTHIN = NOT(NOR(TVZB, THZ));
		NTVIN = NOT(NOR(AND(NTHO, NOT(NOT(BLNK))), TVZ));

		// 0/TV

		tvz_latch1.set(TVZ, PCLK);
		tvz_latch2.set(tvz_latch1.nget(), n_PCLK);
		tvstep_latch.set(NOT(TVSTEP), n_PCLK);
		Z_TV = NOR(tvz_latch2.get(), tvstep_latch.get());
	}

	void TileCnt::sim_FVCounter()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState carry = FVIN;

		for (size_t n = 0; n < 3; n++)
		{
			carry = FVCounter[n].sim(PCLK, TVLOAD, TVSTEP, ppu->wire.FV[n], carry, ppu->wire.FVO[n], ppu->wire.n_FVO[n]);
		}
	}

	void TileCnt::sim_NTCounters()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState unused;

		NTHO = NTHCounter.sim(PCLK, THLOAD, THSTEP, ppu->wire.NTH, NTHIN, ppu->wire.NTHOut, unused);
		NTVO = NTVCounter.sim(PCLK, TVLOAD, TVSTEP, ppu->wire.NTV, NTVIN, ppu->wire.NTVOut, unused);
	}

	void TileCnt::sim_TVCounter()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState carry = TVIN;

		for (size_t n = 0; n < 5; n++)
		{
			carry = TVCounter[n].sim_res(PCLK, TVLOAD, TVSTEP, ppu->wire.TV[n], carry, Z_TV, ppu->wire.TVO[n], ppu->wire.n_TVO[n]);
		}
	}

	void TileCnt::sim_THCounter()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState carry = THIN;

		for (size_t n = 0; n < 5; n++)
		{
			carry = THCounter[n].sim(PCLK, THLOAD, THSTEP, ppu->wire.TH[n], carry, ppu->wire.THO[n], ppu->wire.n_THO[n]);
		}
	}

	TriState TileCounterBit::sim(TriState Clock, TriState Load, TriState Step,
		TriState val_in, TriState carry_in,
		TriState& val_out, TriState& n_val_out)
	{
		auto val = MUX(Step, MUX(Load, MUX(Clock, TriState::Z, ff.get()), val_in), step_latch.nget());
		ff.set(val);
		step_latch.set(MUX(carry_in, ff.nget(), ff.get()), Clock);
		val_out = ff.get();
		n_val_out = ff.nget();
		TriState carry_out = NOR(n_val_out, NOT(carry_in));
		return carry_out;
	}

	TriState TileCounterBit::sim_res(TriState Clock, TriState Load, TriState Step,
		TriState val_in, TriState carry_in, TriState Reset,		// Reset: clears not only the contents of the counter's input FF in keep state, but also pulldowns its output value (but NOT complement output)
		TriState& val_out, TriState& n_val_out)
	{
		auto val = MUX(Step, MUX(Load, MUX(Clock, TriState::Z, AND(ff.get(), NOT(Reset))), val_in), step_latch.nget());
		ff.set(val);
		step_latch.set(MUX(carry_in, ff.nget(), ff.get()), Clock);
		val_out = AND(ff.get(), NOT(Reset));
		n_val_out = ff.nget();
		TriState carry_out = NOR(n_val_out, NOT(carry_in));
		return carry_out;
	}

	// PPU Address Mux

	PAMUX::PAMUX(PPU* parent)
	{
		ppu = parent;
	}

	PAMUX::~PAMUX()
	{
	}

	void PAMUX::sim()
	{
		sim_Control();
	}

	void PAMUX::sim_Control()
	{
		TriState nH2_D = ppu->wire.nH2_Dash;
		TriState BLNK = ppu->fsm.BLNK;
		TriState F_AT = ppu->fsm.FAT;
		TriState DB_PAR = ppu->wire.DB_PAR;

		PARR = NOR(nH2_D, BLNK);
		PAH = NOR(PARR, F_AT);
		PAL = NOR(NOT(PAH), DB_PAR);
	}

	void PAMUX::sim_MuxInputs()
	{
		TriState BLNK = ppu->fsm.BLNK;

		AT_ADR[0] = ppu->wire.THO[2];
		AT_ADR[1] = ppu->wire.THO[3];
		AT_ADR[2] = ppu->wire.THO[4];
		AT_ADR[3] = ppu->wire.TVO[2];
		AT_ADR[4] = ppu->wire.TVO[3];
		AT_ADR[5] = ppu->wire.TVO[4];
		AT_ADR[6] = TriState::One;
		AT_ADR[7] = TriState::One;
		AT_ADR[8] = TriState::One;
		AT_ADR[9] = TriState::One;

		NT_ADR[0] = ppu->wire.THO[0];
		NT_ADR[1] = ppu->wire.THO[1];
		NT_ADR[2] = ppu->wire.THO[2];
		NT_ADR[3] = ppu->wire.THO[3];
		NT_ADR[4] = ppu->wire.THO[4];
		NT_ADR[5] = ppu->wire.TVO[0];
		NT_ADR[6] = ppu->wire.TVO[1];
		NT_ADR[7] = ppu->wire.TVO[2];
		NT_ADR[8] = ppu->wire.TVO[3];
		NT_ADR[9] = ppu->wire.TVO[4];

		AT_ADR[10] = NT_ADR[10] = ppu->wire.NTHOut;
		AT_ADR[11] = NT_ADR[11] = ppu->wire.NTVOut;
		AT_ADR[12] = NT_ADR[12] = NOR(ppu->wire.n_FVO[0], NOT(BLNK));
		AT_ADR[13] = NT_ADR[13] = NOT(NOR(ppu->wire.FVO[1], NOT(BLNK)));

		for (size_t n = 0; n < 14; n++)
		{
			PAT_ADR[n] = ppu->wire.PAT_ADR[n];
		}
	}

	void PAMUX::sim_MuxOutputs()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState DB_PAR = ppu->wire.DB_PAR;
		TriState F_AT = ppu->fsm.FAT;

		for (size_t n = 0; n < 8; n++)
		{
			par_lo[n].sim(PCLK, PARR, DB_PAR, PAL, F_AT, AT_ADR[n], NT_ADR[n], PAT_ADR[n], ppu->GetDBBit(n), ppu->wire.n_PA_Bot[n]);
		}

		for (size_t n = 0; n < 6; n++)
		{
			par_hi[n].sim(PCLK, PARR, PAH, F_AT, AT_ADR[8 + n], NT_ADR[8 + n], PAT_ADR[8 + n], ppu->wire.n_PA_Top[n]);
		}
	}

	void PAMUX_LowBit::sim(TriState PCLK, TriState PARR, TriState DB_PAR, TriState PAL, TriState F_AT,
		TriState AT_ADR, TriState NT_ADR, TriState PAT_ADR, TriState DB_in,
		TriState& n_PAx)
	{
		auto val = MUX(DB_PAR,
			MUX(PARR,
				MUX(PAL, MUX(F_AT, TriState::Z, AT_ADR), NT_ADR), PAT_ADR), DB_in);
		in_latch.set(val, TriState::One);
		out_latch.set(in_latch.nget(), PCLK);
		n_PAx = out_latch.get();
	}

	void PAMUX_HighBit::sim(TriState PCLK, TriState PARR, TriState PAH, TriState F_AT,
		TriState AT_ADR, TriState NT_ADR, TriState PAT_ADR,
		TriState& n_PAx)
	{
		auto val = MUX(PARR,
			MUX(PAH, MUX(F_AT, TriState::Z, AT_ADR), NT_ADR), PAT_ADR);
		in_latch.set(val, TriState::One);
		out_latch.set(in_latch.nget(), PCLK);
		n_PAx = out_latch.get();
	}

	// Control Registers

	ControlRegs::ControlRegs(PPU* parent)
	{
		ppu = parent;
	}

	ControlRegs::~ControlRegs()
	{
	}

	void ControlRegs::sim()
	{
		sim_RegularRegOps();
		sim_W56RegOps();
		sim_FirstSecond_SCCX_Write();

		sim_RegFFs();

		switch (ppu->rev)
		{
		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
			sim_PalBLACK();
			break;
		}
	}

	void ControlRegs::sim_RWDecoder()
	{
		TriState RnW = ppu->wire.RnW;
		TriState n_DBE = ppu->wire.n_DBE;

		ppu->wire.n_RD = NOT(NOR(NOT(RnW), n_DBE));
		ppu->wire.n_WR = NOT(NOR(RnW, n_DBE));
	}

	void ControlRegs::sim_RegularRegOps()
	{
		TriState RS0 = ppu->wire.RS[0];
		TriState RS1 = ppu->wire.RS[1];
		TriState RS2 = ppu->wire.RS[2];
		TriState RnW = ppu->wire.RnW;
		TriState in2[4]{};

		// Others

		in2[0] = NOT(RS0);
		in2[1] = NOT(RS1);
		in2[2] = NOT(RS2);
		in2[3] = NOT(RnW);
		ppu->wire.n_R7 = NOT(NOR4(in2));

		in2[0] = NOT(RS0);
		in2[1] = NOT(RS1);
		in2[2] = NOT(RS2);
		in2[3] = RnW;
		ppu->wire.n_W7 = NOT(NOR4(in2));

		in2[0] = RS0;
		in2[1] = RS1;
		in2[2] = NOT(RS2);
		in2[3] = RnW;
		ppu->wire.n_W4 = NOT(NOR4(in2));

		in2[0] = NOT(RS0);
		in2[1] = NOT(RS1);
		in2[2] = RS2;
		in2[3] = RnW;
		ppu->wire.n_W3 = NOT(NOR4(in2));

		in2[0] = RS0;
		in2[1] = NOT(RS1);
		in2[2] = RS2;
		in2[3] = NOT(RnW);
		ppu->wire.n_R2 = NOT(NOR4(in2));

		in2[0] = NOT(RS0);
		in2[1] = RS1;
		in2[2] = RS2;
		in2[3] = RnW;
		ppu->wire.n_W1 = NOT(NOR4(in2));

		in2[0] = RS0;
		in2[1] = RS1;
		in2[2] = RS2;
		in2[3] = RnW;
		ppu->wire.n_W0 = NOT(NOR4(in2));

		switch (ppu->rev)
		{
			// TBD: Add the remaining RGB PPUs

		case Revision::RP2C04_0003:
		{
			// n_R4 is not used.
		}
		break;

		default:
		{
			in2[0] = RS0;
			in2[1] = RS1;
			in2[2] = NOT(RS2);
			in2[3] = NOT(RnW);
			ppu->wire.n_R4 = NOT(NOR4(in2));
		}
		break;
		}
	}

	void ControlRegs::sim_W56RegOps()
	{
		TriState RS0 = ppu->wire.RS[0];
		TriState RS1 = ppu->wire.RS[1];
		TriState RS2 = ppu->wire.RS[2];
		TriState RnW = ppu->wire.RnW;
		TriState in[5]{};
		TriState in2[4]{};

		// SCCX

		in[0] = RS0;
		in[1] = NOT(RS1);
		in[2] = NOT(RS2);
		in[3] = get_Scnd();
		in[4] = RnW;
		ppu->wire.n_W6_1 = NOT(NOR5(in));

		in[0] = RS0;
		in[1] = NOT(RS1);
		in[2] = NOT(RS2);
		in[3] = get_Frst();
		in[4] = RnW;
		ppu->wire.n_W6_2 = NOT(NOR5(in));

		in[0] = NOT(RS0);
		in[1] = RS1;
		in[2] = NOT(RS2);
		in[3] = get_Scnd();
		in[4] = RnW;
		ppu->wire.n_W5_1 = NOT(NOR5(in));

		in[0] = NOT(RS0);
		in[1] = RS1;
		in[2] = NOT(RS2);
		in[3] = get_Frst();
		in[4] = RnW;
		ppu->wire.n_W5_2 = NOT(NOR5(in));

		in2[0] = NOT(ppu->wire.n_W5_1);
		in2[1] = NOT(ppu->wire.n_W5_2);
		in2[2] = NOT(ppu->wire.n_W6_1);
		in2[3] = NOT(ppu->wire.n_W6_2);
		n_W56 = NOR4(in2);
	}

	void ControlRegs::sim_FirstSecond_SCCX_Write()
	{
		TriState RC = ppu->wire.RC;
		TriState n_DBE = ppu->wire.n_DBE;
		TriState n_R2 = ppu->wire.n_R2;

		TriState R2_Enable = NOR(n_R2, n_DBE);
		TriState W56_Enable = NOR(n_W56, n_DBE);

		SCCX_FF1.set(NOR3(RC, R2_Enable, MUX(W56_Enable, NOT(SCCX_FF2.get()), NOT(SCCX_FF1.get()))));
		SCCX_FF2.set(NOR3(RC, R2_Enable, MUX(W56_Enable, NOT(SCCX_FF2.get()), SCCX_FF1.get())));
	}

	void ControlRegs::sim_RegFFs()
	{
		TriState RC = ppu->wire.RC;
		TriState n_W0 = ppu->wire.n_W0;
		TriState n_W1 = ppu->wire.n_W1;
		TriState n_DBE = ppu->wire.n_DBE;
		TriState W0_Enable = NOR(n_W0, n_DBE);
		TriState W1_Enable = NOR(n_W1, n_DBE);

		// All code below is derived from CTRL0/CTRL1 FFs and in the simulator is stored on wires with virtually infinite capacity (ppu->wire)

		for (size_t n = 0; n < 8; n++)
		{
			if (n >= 2)
			{
				// Bits 0 and 1 in the PAR Gen.

				PPU_CTRL0[n].set(NOR(RC, NOT(MUX(W0_Enable, PPU_CTRL0[n].get(), ppu->GetDBBit(n)))));
			}
			PPU_CTRL1[n].set(NOR(RC, NOT(MUX(W1_Enable, PPU_CTRL1[n].get(), ppu->GetDBBit(n)))));
		}

		// CTRL0

		i132_latch.set(PPU_CTRL0[2].get(), NOT(W0_Enable));
		ppu->wire.I1_32 = NOT(i132_latch.nget());

		obsel_latch.set(PPU_CTRL0[3].get(), NOT(W0_Enable));
		ppu->wire.OBSEL = obsel_latch.nget();

		bgsel_latch.set(PPU_CTRL0[4].get(), NOT(W0_Enable));
		ppu->wire.BGSEL = bgsel_latch.nget();

		o816_latch.set(PPU_CTRL0[5].get(), NOT(W0_Enable));
		ppu->wire.O8_16 = NOT(o816_latch.nget());

		ppu->wire.n_SLAVE = PPU_CTRL0[6].get();

		switch (ppu->rev)
		{
			// The PAL PPU (and derivative) uses a hidden latch for the VBL signal, which is stored between the open transistor and the inverter in the VBlank INT circuit.

		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
			vbl_latch.set(PPU_CTRL0[7].get(), NOT(W0_Enable));
			ppu->wire.VBL = vbl_latch.get();
			break;

		default:
			ppu->wire.VBL = PPU_CTRL0[7].get();
			break;
		}

		// CTRL1

		ppu->wire.BnW = PPU_CTRL1[0].get();

		bgclip_latch.set(PPU_CTRL1[1].get(), NOT(W1_Enable));
		ppu->wire.n_BGCLIP = ClippingAlwaysDisabled ? TriState::One : NOT(bgclip_latch.nget());

		obclip_latch.set(PPU_CTRL1[2].get(), NOT(W1_Enable));
		ppu->wire.n_OBCLIP = ClippingAlwaysDisabled ? TriState::One : NOT(obclip_latch.nget());

		bge_latch.set(PPU_CTRL1[3].get(), NOT(W1_Enable));
		obe_latch.set(PPU_CTRL1[4].get(), NOT(W1_Enable));
		ppu->wire.BGE = RenderAlwaysEnabled ? TriState::One : bge_latch.get();
		ppu->wire.OBE = RenderAlwaysEnabled ? TriState::One : obe_latch.get();
		ppu->wire.BLACK = NOR(ppu->wire.BGE, ppu->wire.OBE);

		tr_latch.set(PPU_CTRL1[5].get(), NOT(W1_Enable));
		ppu->wire.n_TR = tr_latch.nget();

		tg_latch.set(PPU_CTRL1[6].get(), NOT(W1_Enable));
		ppu->wire.n_TG = tg_latch.nget();

		ppu->wire.n_TB = NOT(PPU_CTRL1[7].get());
	}

	TriState ControlRegs::get_Frst()
	{
		return SCCX_FF1.nget();
	}

	TriState ControlRegs::get_Scnd()
	{
		return SCCX_FF1.get();
	}

	/// <summary>
	/// The CLPB/CLPO signal acquisition simulation should be done after the FSM.
	/// </summary>
	void ControlRegs::sim_CLP()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_VIS = ppu->fsm.nVIS;
		TriState CLIP_B = ppu->fsm.CLIP_B;
		TriState CLIP_O = ppu->fsm.CLIP_O;
		TriState BGE = ppu->wire.BGE;
		TriState OBE = ppu->wire.OBE;

		nvis_latch.set(n_VIS, n_PCLK);
		clipb_latch.set(CLIP_B, n_PCLK);
		clipo_latch.set(NOR3(nvis_latch.get(), CLIP_O, NOT(OBE)), n_PCLK);

		ppu->wire.n_CLPB = NOR3(nvis_latch.get(), clipb_latch.get(), NOT(BGE));
		ppu->wire.CLPO = clipo_latch.nget();
	}

	/// <summary>
	/// The `/SLAVE` signal is used for EXT input terminals.
	/// </summary>
	/// <returns></returns>
	TriState ControlRegs::get_nSLAVE()
	{
		return PPU_CTRL0[6].get();
	}

	/// <summary>
	/// Special BLACK signal processing for PAL-Like PPUs.
	/// </summary>
	void ControlRegs::sim_PalBLACK()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;

		BLACK_FF1.set(MUX(PCLK, NOT(NOT(BLACK_FF1.get())), ppu->wire.BLACK));
		BLACK_FF2.set(MUX(n_PCLK, NOT(NOT(BLACK_FF2.get())), NOT(NOT(BLACK_FF1.get()))));
		black_latch1.set(NOT(NOT(BLACK_FF2.get())), PCLK);
		black_latch2.set(black_latch1.nget(), n_PCLK);
		ppu->wire.BLACK = black_latch2.nget();
	}

	// Scrolling Registers

	ScrollRegs::ScrollRegs(PPU* parent)
	{
		ppu = parent;
	}

	ScrollRegs::~ScrollRegs()
	{
	}

	void ScrollRegs::sim()
	{
		TriState n_DBE = ppu->wire.n_DBE;
		TriState RC = ppu->wire.RC;
		TriState W0 = NOR(ppu->wire.n_W0, n_DBE);
		TriState W5_1 = NOR(ppu->wire.n_W5_1, n_DBE);
		TriState W5_2 = NOR(ppu->wire.n_W5_2, n_DBE);
		TriState W6_1 = NOR(ppu->wire.n_W6_1, n_DBE);
		TriState W6_2 = NOR(ppu->wire.n_W6_2, n_DBE);

		// FineH

		for (size_t n = 0; n < 3; n++)
		{
			FineH[n].sim(MUX(W5_1, TriState::Z, ppu->GetDBBit(n)), n_DBE, RC, ppu->wire.FH[n]);
		}

		// FineV

		for (size_t n = 0; n < 3; n++)
		{
			FineV[n].sim(MUX(W5_2, MUX(W6_1, TriState::Z,
				n == 2 ? TriState::Zero : ppu->GetDBBit(4 + n)),
				ppu->GetDBBit(n)), n_DBE, RC, ppu->wire.FV[n]);
		}

		// NTV

		NTV.sim(MUX(W0, MUX(W6_1, TriState::Z, ppu->GetDBBit(3)), ppu->GetDBBit(1)), n_DBE, RC, ppu->wire.NTV);

		// NTH

		NTH.sim(MUX(W0, MUX(W6_1, TriState::Z, ppu->GetDBBit(2)), ppu->GetDBBit(0)), n_DBE, RC, ppu->wire.NTH);

		// TileV

		for (size_t n = 0; n < 5; n++)
		{
			TriState val_prev{};

			if (n >= 3) {
				val_prev = MUX(W6_1, TriState::Z, ppu->GetDBBit(n - 3));
			}
			else {
				val_prev = MUX(W6_2, TriState::Z, ppu->GetDBBit(5 + n));
			}
			TriState val_in = MUX(W5_2, val_prev, ppu->GetDBBit(3 + n));

			TileV[n].sim(val_in, n_DBE, RC, ppu->wire.TV[n]);
		}

		// TileH

		for (size_t n = 0; n < 5; n++)
		{
			TileH[n].sim(MUX(W5_1, MUX(W6_2, TriState::Z, ppu->GetDBBit(n)), ppu->GetDBBit(3 + n)), n_DBE, RC, ppu->wire.TH[n]);
		}
	}

	void SCC_FF::sim(TriState val_in, TriState n_DBE, TriState RC, TriState& val_out)
	{
		val_in = val_in == TriState::Z ? ff.get() : val_in;
		ff.set(NOR(NOT(MUX(n_DBE, val_in, ff.get())), RC));
		val_out = ff.get();
	}

	TriState SCC_FF::get()
	{
		return ff.get();
	}

	void SCC_FF::set(TriState val)
	{
		ff.set(val);
	}

	// Data Reader (Still Picture Generator)

	DataReader::DataReader(PPU* parent)
	{
		ppu = parent;

		par = new PAR(ppu);
		tilecnt = new TileCnt(ppu);
		pamux = new PAMUX(ppu);
		sccx = new ScrollRegs(ppu);
		bgcol = new BGCol(ppu);
	}

	DataReader::~DataReader()
	{
		delete par;
		delete tilecnt;
		delete pamux;
		delete sccx;
		delete bgcol;
	}

	void DataReader::sim()
	{
		sccx->sim();
		pamux->sim();
		tilecnt->sim();
		par->sim();
		pamux->sim_MuxInputs();
		pamux->sim_MuxOutputs();
		bgcol->sim();
	}

	// Sprite Comparison

	ObjEval::ObjEval(PPU* parent)
	{
		ppu = parent;
	}

	ObjEval::~ObjEval()
	{
	}

	void ObjEval::sim()
	{
		sim_StepJohnson();
		sim_Comparator();
		sim_ComparisonFSM();

		sim_MainCounterControl();
		sim_MainCounter();

		sim_TempCounterControlBeforeCounter();
		sim_TempCounter();
		sim_TempCounterControlAfterCounter();

		sim_SpriteOVF();

		sim_OAMAddress();
	}

	void ObjEval::sim_StepJohnson()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState H0_DD = ppu->wire.H0_Dash2;

		COPY_STEP = NOR(PCLK, NOT(H0_DD));

		// The shift register must be simulated backwards.

		i2_latch[5].set(i2_latch[4].nget(), PCLK);
		i2_latch[4].set(i2_latch[3].nget(), COPY_STEP);
		i2_latch[3].set(i2_latch[2].nget(), PCLK);
		i2_latch[2].set(i2_latch[1].nget(), COPY_STEP);
		i2_latch[1].set(i2_latch[0].nget(), PCLK);

		COPY_OVF = NOT(NOR3(i2_latch[5].nget(), i2_latch[3].nget(), i2_latch[1].nget()));
	}

	void ObjEval::sim_Comparator()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState O8_16 = ppu->wire.O8_16;
		TriState carry_in = TriState::Zero;

		for (size_t n = 0; n < 8; n++)
		{
			OB_latch[n].set(ppu->oam->get_OB(n), PCLK);
			OB_Bits[n] = OB_latch[n].get();
		}

		for (size_t n = 0; n < 4; n++)
		{
			carry_in = cmpr[n].sim(
				OB_Bits[2 * n], ppu->v->getBit(2 * n),
				OB_Bits[2 * n + 1], ppu->v->getBit(2 * n + 1),
				carry_in, ppu->wire.OV[2 * n], ppu->wire.OV[2 * n + 1]);
		}

		ovz_latch.set(ppu->oam->get_OB(7), PCLK);

		TriState temp[7]{};
		temp[0] = ppu->wire.OV[4];
		temp[1] = ppu->wire.OV[5];
		temp[2] = ppu->wire.OV[6];
		temp[3] = ppu->wire.OV[7];
		temp[4] = AND(ppu->wire.OV[3], NOT(O8_16));
		temp[5] = NOR(ovz_latch.nget(), ppu->v->getBit(7));
		temp[6] = COPY_OVF;
		OVZ = NOR7(temp);
	}

	void ObjEval::sim_ComparisonFSM()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState PCLK = ppu->wire.PCLK;
		TriState nF_NT = ppu->fsm.nFNT;
		TriState H0_DD = ppu->wire.H0_Dash2;
		TriState I_OAM2 = ppu->fsm.IOAM2;
		TriState n_VIS = ppu->fsm.nVIS;
		TriState SPR_OV = get_SPR_OV();
		TriState S_EV = ppu->fsm.SEV;
		TriState OBJ_READ = ppu->fsm.OBJ_READ;
		TriState NotUsed{};

		// PD/FIFO

		TriState n_PCLK2 = NOT(ppu->wire.PCLK);
		fnt_latch.set(NOT(NOR(nF_NT, NOT(H0_DD))), n_PCLK2);
		novz_latch.set(NOT(OVZ), n_PCLK2);
		eval_FF3.sim(n_PCLK2, fnt_latch.get(), novz_latch.nget(), ppu->wire.PD_FIFO, NotUsed);

		// Set the command to copy the sprite if it is found.

		TriState temp[4]{};
		temp[0] = I_OAM2;
		temp[1] = n_VIS;
		temp[2] = SPR_OV;
		temp[3] = NOT(OVZ);
		DO_COPY = NOR4(temp);

		// Reload Johnson counter

		i2_latch[0].set(DO_COPY, COPY_STEP);

		// Set Mode4

		OMFG = NOR(COPY_OVF, DO_COPY);

		// Handle finding sprite 0 on the current line for the STRIKE circuit (Spr0 Hit).

		TriState nFF2_Out{};
		eval_FF2.sim(PCLK, NOT(S_EV), DO_COPY, NotUsed, nFF2_Out);
		eval_FF1.sim(PCLK, NOT(OBJ_READ), nFF2_Out, ppu->wire.n_SPR0_EV, NotUsed);
	}

	void ObjEval::sim_MainCounterControl()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState PCLK = ppu->wire.PCLK;
		TriState I_OAM2 = ppu->fsm.IOAM2;
		TriState n_VIS = ppu->fsm.nVIS;
		TriState H0_DD = ppu->wire.H0_Dash2;
		TriState OFETCH = ppu->wire.OFETCH;
		TriState n_W3 = ppu->wire.n_W3;
		TriState n_DBE = ppu->wire.n_DBE;
		TriState ZOMG{};

		switch (ppu->rev)
		{
			// For the PAL PPU, the $2003 write delay is screwed on. This is most likely how they fight OAM Decay.

		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
		{
			auto W3 = NOR(n_W3, n_DBE);
			W3_FF1.set(NOR(NOR(W3, W3_FF1.get()), w3_latch3.nget()));
			auto w3_ff1_out = NOR(W3_FF1.nget(), W3);
			W3_FF2.set(NOT(NOT(MUX(PCLK, W3_FF2.get(), w3_ff1_out))));
			w3_latch1.set(W3_FF2.nget(), n_PCLK);
			w3_latch2.set(w3_latch1.nget(), PCLK);
			w3_latch3.set(w3_latch2.nget(), n_PCLK);
			w3_latch4.set(w3_latch3.nget(), PCLK);
			W3_Enable = NOR(w3_latch4.get(), w3_latch2.nget());

			// ZOMG comes from the circuit located in the same place as the EVEN/ODD circuit for the NTSC PPU (to the right of the V Decoder).

			ZOMG = ppu->wire.EvenOddOut;
			break;
		}

		default:
			W3_Enable = NOR(n_W3, n_DBE);
			// In order not to change the logic below the pseudo-ZOMG is made equal to 0 and NOR becomes NOT.
			ZOMG = TriState::Zero;
			break;
		}

		init_latch.set(NAND(NOR(I_OAM2, n_VIS), H0_DD), n_PCLK);
		ofetch_latch.set(OFETCH, n_PCLK);
		OMSTEP = NAND(OR(init_latch.get(), n_PCLK), NOR(NOR(ofetch_latch.nget(), n_PCLK), ZOMG));
		OMOUT = NOR(OMSTEP, W3_Enable);
	}

	void ObjEval::sim_MainCounter()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState BLNK = ppu->fsm.BLNK;
		TriState OMFG = this->OMFG;
		TriState OBJ_READ = ppu->fsm.OBJ_READ;
		TriState OMOUT = this->OMOUT;
		TriState OMSTEP = this->OMSTEP;
		TriState n_out[8]{};
		auto Mode4 = NOR(BLNK, NOT(OMFG));
		TriState carry_in;
		TriState carry_out;

		carry_in = TriState::One;
		carry_out = MainCounter[0].sim(OMOUT, W3_Enable, OMSTEP, Mode4, OBJ_READ, ppu->GetDBBit(0), carry_in, OAM_x[0], n_out[0]);

		carry_in = carry_out;
		MainCounter[1].sim(OMOUT, W3_Enable, OMSTEP, Mode4, OBJ_READ, ppu->GetDBBit(1), carry_in, OAM_x[1], n_out[1]);

		auto out01 = NOT(NOR(n_out[0], n_out[1]));
		auto out01m = AND(out01, NOT(Mode4));

		carry_in = NAND(NOT(Mode4), out01);
		MainCounter[2].sim(OMOUT, W3_Enable, OMSTEP, TriState::Zero, OBJ_READ, ppu->GetDBBit(2), carry_in, OAM_x[2], n_out[2]);

		carry_in = NOR(out01m, n_out[2]);
		MainCounter[3].sim(OMOUT, W3_Enable, OMSTEP, TriState::Zero, OBJ_READ, ppu->GetDBBit(3), carry_in, OAM_x[3], n_out[3]);

		carry_in = NOR3(out01m, n_out[2], n_out[3]);
		MainCounter[4].sim(OMOUT, W3_Enable, OMSTEP, TriState::Zero, OBJ_READ, ppu->GetDBBit(4), carry_in, OAM_x[4], n_out[4]);

		TriState temp[6]{};

		temp[0] = out01m;
		temp[1] = n_out[2];
		temp[2] = n_out[3];
		temp[3] = n_out[4];
		carry_in = NOR4(temp);
		MainCounter[5].sim(OMOUT, W3_Enable, OMSTEP, TriState::Zero, OBJ_READ, ppu->GetDBBit(5), carry_in, OAM_x[5], n_out[5]);

		temp[4] = n_out[5];
		carry_in = NOR5(temp);
		MainCounter[6].sim(OMOUT, W3_Enable, OMSTEP, TriState::Zero, OBJ_READ, ppu->GetDBBit(6), carry_in, OAM_x[6], n_out[6]);

		temp[5] = n_out[6];
		carry_in = NOR6(temp);
		OMV = MainCounter[7].sim(OMOUT, W3_Enable, OMSTEP, TriState::Zero, OBJ_READ, ppu->GetDBBit(7), carry_in, OAM_x[7], n_out[7]);

		omv_latch.set(OMV, n_PCLK);
	}

	void ObjEval::sim_TempCounterControlBeforeCounter()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_EVAL = ppu->fsm.n_EVAL;
		TriState OMFG = this->OMFG;
		TriState I_OAM2 = ppu->fsm.IOAM2;
		TriState H0_D = ppu->wire.H0_Dash;
		TriState n_H2_D = ppu->wire.nH2_Dash;
		TriState OBJ_READ = ppu->fsm.OBJ_READ;

		eval_latch.set(n_EVAL, n_PCLK);
		ORES = NOR(n_PCLK, eval_latch.get());
		nomfg_latch.set(NOT(OMFG), n_PCLK);
		ioam2_latch.set(I_OAM2, n_PCLK);
		auto DontStep = NOR(NOR(NOR(nomfg_latch.get(), ioam2_latch.get()), H0_D), AND(n_H2_D, OBJ_READ));
		temp_latch1.set(NAND(OAMCTR2_FF.nget(), n_EVAL), n_PCLK);
		OSTEP = NOR3(temp_latch1.get(), n_PCLK, DontStep);
	}

	void ObjEval::sim_TempCounter()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState carry = TriState::One;
		TriState ORES = this->ORES;
		TriState OSTEP = this->OSTEP;
		TriState NotUsed;

		for (size_t n = 0; n < 5; n++)
		{
			carry = TempCounter[n].sim(n_PCLK, ORES, OSTEP, TriState::Zero, TriState::Zero, TriState::Zero,
				carry, OAM_Temp[n], NotUsed);
		}

		TMV = carry;			// carry_out from the most significant bit
	}

	void ObjEval::sim_TempCounterControlAfterCounter()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;

		tmv_latch.set(TMV, n_PCLK);
		OAMCTR2_FF.set(NOR(ORES, NOR(AND(tmv_latch.get(), OSTEP), OAMCTR2_FF.get())));
		ppu->wire.OAMCTR2 = OAMCTR2_FF.get();
	}

	/// <summary>
	/// Sprite Overflow logic should be simulated after the counters work. 
	/// It captures the fact of their overflow (and terminates the current sprite process)
	/// </summary>
	void ObjEval::sim_SpriteOVF()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState H0_D = ppu->wire.H0_Dash;
		TriState RESCL = ppu->fsm.RESCL;
		TriState I_OAM2 = ppu->fsm.IOAM2;
		TriState n_R2 = ppu->wire.n_R2;
		TriState n_DBE = ppu->wire.n_DBE;
		auto R2_Enable = NOR(n_R2, n_DBE);

		omfg_latch.set(OMFG, n_PCLK);
		setov_latch.set(OAMCTR2_FF.nget(), n_PCLK);

		TriState temp[4]{};
		temp[0] = omfg_latch.get();
		temp[1] = setov_latch.get();
		temp[2] = H0_D;
		temp[3] = n_PCLK;
		auto TempOVF = NOR4(temp);

		auto MainOVF = AND(OMSTEP, omv_latch.get());
		SPR_OV_REG_FF.set(NOR(NOR(TempOVF, SPR_OV_REG_FF.get()), RESCL));
		SPR_OV_FF.set(NOR(NOR3(MainOVF, TempOVF, SPR_OV_FF.get()), I_OAM2));
		ppu->wire.SPR_OV = SPR_OV_FF.get();
		ppu->SetDBBit(5, MUX(R2_Enable, TriState::Z, SPR_OV_REG_FF.get()));
	}

	void ObjEval::sim_OAMAddress()
	{
		TriState n_VIS = ppu->fsm.nVIS;
		TriState H0_DD = ppu->wire.H0_Dash2;
		TriState BLNK = ppu->fsm.BLNK;
		TriState OAP{};

		switch (ppu->rev)
		{
		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
		{
			TriState n_PCLK = ppu->wire.n_PCLK;
			blnk_latch.set(BLNK, n_PCLK);
			OAP = NAND(OR(n_VIS, H0_DD), blnk_latch.nget());
			break;
		}

		default:
			OAP = NAND(OR(n_VIS, H0_DD), NOT(BLNK));
			break;
		}

		ppu->wire.OAM8 = NOT(OAP);

		for (size_t n = 0; n < 3; n++)
		{
			ppu->wire.n_OAM[n] = NOT(OAM_x[n]);
		}

		for (size_t n = 0; n < 5; n++)
		{
			ppu->wire.n_OAM[n + 3] = NOT(MUX(ppu->wire.OAM8, OAM_x[n + 3], OAM_Temp[n]));
		}
	}

	TriState OAMCounterBit::sim(
		TriState Clock,
		TriState Load,
		TriState Step,
		TriState BlockCount,
		TriState Reset,
		TriState val_in,
		TriState carry_in,
		TriState& val_out,
		TriState& n_val_out)
	{
		keep_ff.set(MUX(Step,
			MUX(Load, MUX(Clock, TriState::Z, NOR(Reset, keep_ff.nget())), val_in),
			NOR(cnt_latch.get(), BlockCount)));
		cnt_latch.set(MUX(carry_in, keep_ff.nget(), NOR(keep_ff.nget(), Reset)), Clock);
		TriState carry_out = NOR(keep_ff.nget(), NOT(carry_in));
		val_out = NOT(keep_ff.nget());
		n_val_out = keep_ff.nget();
		return carry_out;
	}

	TriState OAMCounterBit::get()
	{
		return keep_ff.get();
	}

	void OAMCounterBit::set(TriState value)
	{
		keep_ff.set(value);
	}

	TriState OAMCmprBit::sim(
		TriState OB_Even,
		TriState V_Even,
		TriState OB_Odd,
		TriState V_Odd,
		TriState carry_in,
		TriState& OV_Even,
		TriState& OV_Odd)
	{
		auto e0 = NAND(NOT(OB_Even), V_Even);
		auto e1 = NOR(NOT(OB_Even), V_Even);
		auto o0 = NOT(NAND(NOT(OB_Odd), V_Odd));
		auto o1 = NOR(NOT(OB_Odd), V_Odd);

		auto z1 = NOR(NOT(e0), e1);
		auto z2 = NOR(NOT(e0), NOR(carry_in, e1));
		auto z3 = NOR(o0, o1);

		OV_Even = NOT(NOR(NOR(carry_in, z1), AND(carry_in, z1)));
		OV_Odd = NOT(NOR(NOR(z2, z3), AND(z2, z3)));
		TriState carry_out = NOR3(NOR(e0, o1), o0, NOR3(e1, o1, carry_in));
		return carry_out;
	}

	void OAMPosedgeDFFE::sim(
		TriState CLK,
		TriState n_EN,
		TriState val_in,
		TriState& Q,
		TriState& n_Q)
	{
		if (CLK == TriState::One)
		{
			// Keep
			ff.set(NOT(NOT(ff.get())));
		}
		else if (CLK == TriState::Zero)
		{
			if (NOR(CLK, n_EN) == TriState::One)
			{
				ff.set(NOT(NOT(val_in)));
			}
		}

		Q = ff.get();
		n_Q = NOT(Q);
	}

	TriState ObjEval::get_SPR_OV()
	{
		return SPR_OV_FF.get();
	}

	// Video Signal Generator

	VideoOut::VideoOut(PPU* parent)
	{
		ppu = parent;

		switch (ppu->rev)
		{
		case Revision::RP2C02G:
		case Revision::RP2C02H:			// TBD
			SyncLevel[0] = 0.f;
			SyncLevel[1] = 0.525f;
			BurstLevel[0] = 0.300f;
			BurstLevel[1] = 0.841f;
			LumaLevel[0][0] = 0.366f;
			LumaLevel[0][1] = 1.091f;
			LumaLevel[1][0] = 0.525f;
			LumaLevel[1][1] = 1.500f;
			LumaLevel[2][0] = 0.966f;
			LumaLevel[2][1] = 1.941f;
			LumaLevel[3][0] = 1.558f;
			LumaLevel[3][1] = 1.941f;
			EmphasizedLumaLevel[0][0] = 0.266f;
			EmphasizedLumaLevel[0][1] = 0.825f;
			EmphasizedLumaLevel[1][0] = 0.391f;
			EmphasizedLumaLevel[1][1] = 1.133f;
			EmphasizedLumaLevel[2][0] = 0.733f;
			EmphasizedLumaLevel[2][1] = 1.466f;
			EmphasizedLumaLevel[3][0] = 1.166f;
			EmphasizedLumaLevel[3][1] = 1.466f;
			composite = true;
			break;

		case Revision::RP2C07_0:
			SyncLevel[0] = 0.f;
			SyncLevel[1] = 0.566f;
			BurstLevel[0] = 0.300f;
			BurstLevel[1] = 0.900f;
			LumaLevel[0][0] = 0.391f;
			LumaLevel[0][1] = 1.175f;
			LumaLevel[1][0] = 0.566f;
			LumaLevel[1][1] = 1.566f;
			LumaLevel[2][0] = 1.041f;
			LumaLevel[2][1] = 2.033f;
			LumaLevel[3][0] = 1.633f;
			LumaLevel[3][1] = 2.033f;
			EmphasizedLumaLevel[0][0] = 0.300f;
			EmphasizedLumaLevel[0][1] = 0.900f;
			EmphasizedLumaLevel[1][0] = 0.433f;
			EmphasizedLumaLevel[1][1] = 1.208f;
			EmphasizedLumaLevel[2][0] = 0.791f;
			EmphasizedLumaLevel[2][1] = 1.566f;
			EmphasizedLumaLevel[3][0] = 1.266f;
			EmphasizedLumaLevel[3][1] = 1.566f;
			composite = true;
			break;

			// The DAC is significantly redesigned, so the 6538 signal levels are different (a little brighter/more saturated).

		case Revision::UMC_UA6538:
			SyncLevel[0] = 0.f;
			SyncLevel[1] = 0.458f;
			BurstLevel[0] = 0.300f;
			BurstLevel[1] = 0.700f;
			LumaLevel[0][0] = 0.341f;
			LumaLevel[0][1] = 0.916f;
			LumaLevel[1][0] = 0.466f;
			LumaLevel[1][1] = 1.400f;
			LumaLevel[2][0] = 0.816f;
			LumaLevel[2][1] = 1.916f;
			LumaLevel[3][0] = 1.441f;
			LumaLevel[3][1] = 1.916f;
			EmphasizedLumaLevel[0][0] = 0.258f;
			EmphasizedLumaLevel[0][1] = 0.700f;
			EmphasizedLumaLevel[1][0] = 0.350f;
			EmphasizedLumaLevel[1][1] = 1.066f;
			EmphasizedLumaLevel[2][0] = 0.625f;
			EmphasizedLumaLevel[2][1] = 1.475f;
			EmphasizedLumaLevel[3][0] = 1.100f;
			EmphasizedLumaLevel[3][1] = 1.475f;
			composite = true;
			break;

		case Revision::RP2C04_0003:
			composite = false;
			SetupColorMatrix();
			break;

			// TBD: others (RGB, clones)
		}

		// For PAL-like composite PPUs, the Chroma decoder simulation is done using PLA, since the decoder there is big enough (so you don't have to bother with it).

		switch (ppu->rev)
		{
		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
			SetupChromaDecoderPAL();
			break;
		}
	}

	VideoOut::~VideoOut()
	{
		if (chroma_decoder != nullptr)
		{
			delete chroma_decoder;
		}

		if (color_matrix != nullptr)
		{
			delete color_matrix;
		}
	}

	void VideoOut::sim(VideoOutSignal& vout)
	{
		sim_nPICTURE();
		sim_InputLatches();

		if (raw)
		{
			// Raw color can be produced immediately after simulating the input latches.

			sim_RAWOutput(vout);
			return;
		}

		if (composite)
		{
			sim_PhaseShifter(ppu->wire.n_CLK, ppu->wire.CLK, ppu->wire.RES);
			sim_ChromaDecoder();
			sim_OutputLatches();
			sim_LumaDecoder(ppu->wire.n_LL);
			sim_Emphasis(ppu->wire.n_TR, ppu->wire.n_TG, ppu->wire.n_TB);
			sim_CompositeDAC(vout);
		}
		else
		{
			sim_ColorMatrix();
			sim_Select12To3();
			sim_OutputLatches();
			sim_RGB_DAC(vout);
		}
	}

	/// <summary>
	/// Memorize the signal values on all the auxiliary latches.
	/// </summary>
	void VideoOut::sim_InputLatches()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState SYNC = ppu->fsm.SYNC;

		for (size_t n = 0; n < 4; n++)
		{
			cc_latch1[n].set(ppu->wire.n_CC[n], PCLK);
			cc_latch2[n].set(cc_latch1[n].nget(), n_PCLK);
		}

		if (composite)
		{
			TriState BURST = ppu->fsm.BURST;

			cc_burst_latch.set(BURST, n_PCLK);
			sync_latch.set(NOT(SYNC), n_PCLK);
			cb_latch.set(BURST, n_PCLK);
		}
		else
		{
			rgb_sync_latch[0].set(SYNC, n_PCLK);
			rgb_sync_latch[1].set(rgb_sync_latch[0].nget(), PCLK);
			rgb_sync_latch[2].set(rgb_sync_latch[1].nget(), n_PCLK);
		}
	}

	void VideoOut::sim_PhaseShifter(TriState n_CLK, TriState CLK, TriState RES)
	{
		TriState unused{};
		TriState sr0_sout{};

		// SR Bit5

		auto sr3_sout = NOT(sr[3].getn_ShiftOut());
		sr[5].sim(sr3_sout, n_CLK, CLK, RES, sr0_sout, unused, PZ[0]);
		PZ[1] = NOR(sr0_sout, NOR(sr[4].getn_ShiftOut(), sr3_sout));

		// SR Bit4

		sr[4].sim(PZ[1], n_CLK, CLK, RES, unused, PZ[3], PZ[2]);

		// SR Bit3

		sr[3].sim(PZ[3], n_CLK, CLK, RES, unused, PZ[6], PZ[5]);

		// SR Bit2

		sr[2].sim(PZ[6], n_CLK, CLK, RES, unused, PZ[8], PZ[7]);

		// SR Bit1

		sr[1].sim(PZ[8], n_CLK, CLK, RES, unused, PZ[10], PZ[9]);

		// SR Bit0

		sr[0].sim(PZ[10], n_CLK, CLK, RES, unused, PZ[12], PZ[11]);

		switch (ppu->rev)
		{
			// TBD: Check and add the remaining "composite" PPUs

		case Revision::RP2C02G:
		case Revision::RP2C02H:
		{
			n_PR = PZ[0];
			n_PG = PZ[9];
			n_PB = PZ[5];
		}
		break;

		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
		{
			TriState n_PCLK = ppu->wire.n_PCLK;
			if (ppu->v != nullptr)
			{
				v0_latch.set(ppu->v->getBit(0), n_PCLK);
			}
			else
			{
				// To convert RAW -> Composite, a virtual PPU is created in which there is nothing else but a video generator.

				v0_latch.set(TriState::Zero, n_PCLK);
			}
			TriState n_V0D = v0_latch.nget();

			n_PR = NOT(MUX(n_V0D, PZ[7], PZ[2]));
			n_PG = NOT(MUX(n_V0D, PZ[2], PZ[7]));
			n_PB = PZ[5];
		}
		break;
		}
	}

	void VideoOut::sim_ChromaDecoder()
	{
		if (chroma_decoder != nullptr)
		{
			sim_ChromaDecoder_PAL();
		}
		else
		{
			sim_ChromaDecoder_NTSC();
		}
	}

	void VideoOut::sim_ChromaDecoder_PAL()
	{
		PALChromaInputs chroma_in{};

		chroma_in.n_V0 = v0_latch.nget();
		chroma_in.V0 = ~chroma_in.n_V0;
		chroma_in.CC0 = cc_latch2[0].nget();
		chroma_in.n_CC0 = ~chroma_in.CC0;
		chroma_in.CC1 = NOR(cc_latch2[1].get(), cc_burst_latch.get());
		chroma_in.n_CC1 = ~chroma_in.CC1;
		chroma_in.CC2 = cc_latch2[2].nget();
		chroma_in.n_CC2 = ~chroma_in.CC2;
		chroma_in.CC3 = NOR(cc_latch2[3].get(), cc_burst_latch.get());
		chroma_in.n_CC3 = ~chroma_in.CC3;

		PBLACK = NOR3(NOR(cc_latch2[1].get(), cc_burst_latch.get()), cc_latch2[2].nget(), NOR(cc_latch2[3].get(), cc_burst_latch.get()));

		TriState* chroma_out;

		chroma_decoder->sim(chroma_in.packed_bits, &chroma_out);

		TriState pz[25]{};

		pz[0] = AND(chroma_out[0], NOT(PZ[0]));
		pz[1] = AND(chroma_out[1], NOT(PZ[0]));
		pz[2] = AND(chroma_out[2], NOT(PZ[1]));
		pz[3] = AND(chroma_out[3], NOT(PZ[1]));

		pz[4] = AND(chroma_out[4], NOT(PZ[2]));
		pz[5] = AND(chroma_out[5], NOT(PZ[2]));
		pz[6] = AND(chroma_out[6], NOT(PZ[3]));
		pz[7] = AND(chroma_out[7], NOT(PZ[3]));

		pz[8] = AND(chroma_out[8], NOT(PZ[5]));
		pz[9] = AND(chroma_out[9], NOT(PZ[5]));
		pz[10] = chroma_out[10];
		pz[11] = AND(chroma_out[11], NOT(PZ[6]));

		pz[12] = AND(chroma_out[12], NOT(PZ[6]));
		pz[13] = AND(chroma_out[13], NOT(PZ[7]));
		pz[14] = AND(chroma_out[14], NOT(PZ[7]));
		pz[15] = AND(chroma_out[15], NOT(PZ[8]));

		pz[16] = AND(chroma_out[16], NOT(PZ[8]));
		pz[17] = AND(chroma_out[17], NOT(PZ[9]));
		pz[18] = AND(chroma_out[18], NOT(PZ[9]));
		pz[19] = AND(chroma_out[19], NOT(PZ[10]));

		pz[20] = AND(chroma_out[20], NOT(PZ[10]));
		pz[21] = AND(chroma_out[21], NOT(PZ[11]));
		pz[22] = AND(chroma_out[22], NOT(PZ[11]));
		pz[23] = AND(chroma_out[23], NOT(PZ[12]));
		pz[24] = AND(chroma_out[24], NOT(PZ[12]));

		n_PZ = NOR25(pz);
	}

	void VideoOut::sim_ChromaDecoder_NTSC()
	{
		TriState dmxin[4]{};
		TriState dmxout[16]{};
		dmxin[0] = cc_latch2[0].nget();
		dmxin[1] = cc_latch2[1].nget();
		dmxin[2] = cc_latch2[2].nget();
		dmxin[3] = NOR(cc_latch2[3].get(), cc_burst_latch.get());
		DMX4(dmxin, dmxout);

		PBLACK = NOR3(dmxin[1], dmxin[2], dmxin[3]);

		P[0] = dmxout[3];
		P[1] = dmxout[10];
		P[2] = dmxout[5];
		P[3] = dmxout[12];
		P[4] = dmxout[15];
		P[5] = dmxout[7];
		P[6] = dmxout[14];
		P[7] = dmxout[9];
		P[8] = dmxout[4];
		P[9] = dmxout[11];
		P[10] = dmxout[6];
		P[11] = dmxout[13];
		P[12] = dmxout[8];

		TriState pz[13]{};

		for (size_t n = 0; n < 13; n++)
		{
			if (n != 4)
			{
				pz[n] = AND(P[n], NOT(PZ[n]));
			}
			else
			{
				pz[n] = P[n];
			}
		}

		n_PZ = NOR13(pz);
	}

	/// <summary>
	/// Call after Chroma Decoder / SEL 12x3.
	/// </summary>
	void VideoOut::sim_OutputLatches()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_PICTURE = VidOut_n_PICTURE;

		if (composite)
		{
			TriState BURST = ppu->fsm.BURST;
			TriState SYNC = ppu->fsm.SYNC;

			// If /POUT = 1 - then the visible part of the signal is not output at all

			pic_out_latch.set(NOR(PBLACK, n_PICTURE), n_PCLK);
			n_POUT = pic_out_latch.nget();

			// For DAC

			black_latch.set(NOT(NOR3(NOR(PBLACK, n_PICTURE), SYNC, BURST)), n_PCLK);
		}
		else
		{
			TriState red_in[3]{};
			TriState green_in[3]{};
			TriState blue_in[3]{};

			TriState red_out[8]{};
			TriState green_out[8]{};
			TriState blue_out[8]{};

			red_sel.getOut(red_in);
			green_sel.getOut(green_in);
			blue_sel.getOut(blue_in);

			DMX3(red_in, red_out);
			DMX3(green_in, green_out);
			DMX3(blue_in, blue_out);

			for (size_t n = 0; n < 8; n++)
			{
				rgb_red_latch[n].set(MUX(NOT(n_PICTURE), TriState::Zero, red_out[n]), n_PCLK);
				rgb_green_latch[n].set(MUX(NOT(n_PICTURE), TriState::Zero, green_out[n]), n_PCLK);
				rgb_blue_latch[n].set(MUX(NOT(n_PICTURE), TriState::Zero, blue_out[n]), n_PCLK);
			}
		}
	}

	void VideoOut::sim_LumaDecoder(TriState n_LL[4])
	{
		n_LU[3] = NOT(NOR3(n_POUT, n_LL[0], n_LL[1]));
		n_LU[2] = NOT(NOR3(n_POUT, NOT(n_LL[0]), n_LL[1]));
		n_LU[1] = NOT(NOR3(n_POUT, n_LL[0], NOT(n_LL[1])));
		n_LU[0] = NOT(NOR3(n_POUT, NOT(n_LL[0]), NOT(n_LL[1])));
	}

	void VideoOut::sim_Emphasis(TriState n_TR, TriState n_TG, TriState n_TB)
	{
		TriState n[3]{};
		n[0] = NOR3(n_POUT, n_PB, n_TB);
		n[1] = NOR3(n_POUT, n_PG, n_TG);
		n[2] = NOR3(n_POUT, n_PR, n_TR);
		TINT = NOT(NOR3(n[0], n[1], n[2]));
	}

	void VideoOut::sim_CompositeDAC(VideoOutSignal& vout)
	{
		TriState tmp = TriState::Zero;
		float v = LumaLevel[3][1];		// White level

		// Synch Low Level

		if (sync_latch.nget() == TriState::One)
		{
			v = std::min(v, SyncLevel[0]);
		}

		// Synch High Level

		if (black_latch.nget() == TriState::One)
		{
			v = std::min(v, SyncLevel[1]);
		}

		// Colorburst phase level

		v = PhaseSwing(v, cb_latch.nget(), BurstLevel[0], BurstLevel[1]);

		// Luminance phase levels

		if (TINT == TriState::One)
		{
			v = PhaseSwing(v, n_LU[0], EmphasizedLumaLevel[0][0], EmphasizedLumaLevel[0][1]);
			v = PhaseSwing(v, n_LU[1], EmphasizedLumaLevel[1][0], EmphasizedLumaLevel[1][1]);
			v = PhaseSwing(v, n_LU[2], EmphasizedLumaLevel[2][0], EmphasizedLumaLevel[2][1]);
			v = PhaseSwing(v, n_LU[3], EmphasizedLumaLevel[3][0], EmphasizedLumaLevel[3][1]);
		}
		else
		{
			v = PhaseSwing(v, n_LU[0], LumaLevel[0][0], LumaLevel[0][1]);
			v = PhaseSwing(v, n_LU[1], LumaLevel[1][0], LumaLevel[1][1]);
			v = PhaseSwing(v, n_LU[2], LumaLevel[2][0], LumaLevel[2][1]);
			v = PhaseSwing(v, n_LU[3], LumaLevel[3][0], LumaLevel[3][1]);
		}

		// In order not to torture the video decoder too much we will mix the noise only in the visible part of the line.

		if (noise_enable && VidOut_n_PICTURE == TriState::Zero)
		{
			v += GetNoise();
		}

		vout.composite = v;
	}

	float VideoOut::PhaseSwing(float v, TriState sel, float level_from, float level_to)
	{
		auto tmp = NOR(sel, n_PZ);
		if (tmp == TriState::One)
		{
			v = std::min(v, level_to);
		}

		tmp = NOR(sel, tmp);
		if (tmp == TriState::One)
		{
			v = std::min(v, level_from);
		}
		return v;
	}

	void VideoOut::sim_RAWOutput(VideoOutSignal& vout)
	{
		if (VidOut_n_PICTURE == TriState::Zero)
		{
			vout.RAW.CC0 = ToByte(cc_latch2[0].get());
			vout.RAW.CC1 = ToByte(cc_latch2[1].get());
			vout.RAW.CC2 = ToByte(cc_latch2[2].get());
			vout.RAW.CC3 = ToByte(cc_latch2[3].get());
			vout.RAW.LL0 = ToByte(NOT(ppu->wire.n_LL[0]));
			vout.RAW.LL1 = ToByte(NOT(ppu->wire.n_LL[1]));
			vout.RAW.TR = ToByte(NOT(ppu->wire.n_TR));
			vout.RAW.TG = ToByte(NOT(ppu->wire.n_TG));
			vout.RAW.TB = ToByte(NOT(ppu->wire.n_TB));
		}
		else
		{
			vout.RAW.raw = 0;
		}

		if (composite)
		{
			vout.RAW.Sync = ToByte(sync_latch.nget());
		}
		else
		{
			vout.RAW.Sync = ToByte(NOT(rgb_sync_latch[2].nget()));
		}
	}

	void VideoOutSRBit::sim(TriState n_shift_in, TriState n_CLK, TriState CLK, TriState RES,
		TriState& shift_out, TriState& n_shift_out, TriState& val)
	{
		in_latch.set(n_shift_in, CLK);
		val = get_Out(RES);
		out_latch.set(val, n_CLK);
		shift_out = out_latch.get();
		n_shift_out = out_latch.nget();
	}

	TriState VideoOutSRBit::get_Out(TriState RES)
	{
		return NOR(in_latch.get(), RES);
	}

	TriState VideoOutSRBit::get_ShiftOut()
	{
		return out_latch.get();
	}

	TriState VideoOutSRBit::getn_ShiftOut()
	{
		return out_latch.nget();
	}

	/// <summary>
	/// The PAL PPU contains an additional DLatch chain for /PICTURE signal propagation.
	/// </summary>
	/// <returns></returns>
	void VideoOut::sim_nPICTURE()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState n_PICTURE = ppu->fsm.n_PICTURE;

		switch (ppu->rev)
		{
		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
			npicture_latch1.set(NOT(n_PICTURE), n_PCLK);
			npicture_latch2.set(npicture_latch1.nget(), PCLK);
			n_PICTURE = npicture_latch2.get();
			break;
		}

		VidOut_n_PICTURE = n_PICTURE;
	}

	void VideoOut::GetSignalFeatures(VideoSignalFeatures& features)
	{
		switch (ppu->rev)
		{
		case Revision::RP2C02G:
		case Revision::RP2C02H:			// TBD
			features.SamplesPerPCLK = 8;
			features.PixelsPerScan = 341;
			features.ScansPerField = 262;
			features.BackPorchSize = 40;
			features.BlackLevel = 0.525f;
			features.WhiteLevel = 1.941f;
			features.SyncLevel = 0.f;
			features.PhaseAlteration = false;
			break;

		case Revision::RP2C07_0:
			features.SamplesPerPCLK = 10;
			features.PixelsPerScan = 341;
			features.ScansPerField = 312;
			features.BackPorchSize = 42;
			features.BlackLevel = 0.566f;
			features.WhiteLevel = 2.033f;
			features.SyncLevel = 0.f;
			features.PhaseAlteration = true;
			break;

			// The DAC is significantly redesigned, so the 6538 signal levels are different (a little brighter/more saturated).

		case Revision::UMC_UA6538:
			features.SamplesPerPCLK = 10;
			features.PixelsPerScan = 341;
			features.ScansPerField = 312;
			features.BackPorchSize = 42;
			features.BlackLevel = 0.458f;
			features.WhiteLevel = 1.916f;
			features.SyncLevel = 0.f;
			features.PhaseAlteration = true;
			break;

		case Revision::RP2C04_0003:
			features.SamplesPerPCLK = 8;
			features.PixelsPerScan = 341;
			features.ScansPerField = 262;
			features.BackPorchSize = 40;
			break;
		}

		features.Composite = IsComposite();
	}

	void VideoOut::SetRAWOutput(bool enable)
	{
		raw = enable;
	}

	void VideoOut::ConvertRAWToRGB_Composite(VideoOutSignal& rawIn, VideoOutSignal& rgbOut)
	{
		VideoSignalFeatures features{};
		GetSignalFeatures(features);

		// Using a temporary PPU of the same revision, to simulate the video generator

		PPU vppu(ppu->rev, true);

		size_t warmup_halfcycles = 6;
		size_t num_phases = 12;
		VideoOutSignal unused{};

		// Latch all important signals

		vppu.wire.n_CC[0] = NOT(FromByte(rawIn.RAW.CC0));
		vppu.wire.n_CC[1] = NOT(FromByte(rawIn.RAW.CC1));
		vppu.wire.n_CC[2] = NOT(FromByte(rawIn.RAW.CC2));
		vppu.wire.n_CC[3] = NOT(FromByte(rawIn.RAW.CC3));
		vppu.wire.n_LL[0] = NOT(FromByte(rawIn.RAW.LL0));
		vppu.wire.n_LL[1] = NOT(FromByte(rawIn.RAW.LL1));
		vppu.wire.n_TR = NOT(FromByte(rawIn.RAW.TR));
		vppu.wire.n_TG = NOT(FromByte(rawIn.RAW.TG));
		vppu.wire.n_TB = NOT(FromByte(rawIn.RAW.TB));

		vppu.fsm.BURST = TriState::Zero;
		vppu.fsm.SYNC = FromByte(rawIn.RAW.Sync);
		vppu.fsm.n_PICTURE = TriState::Zero;

		vppu.wire.n_PCLK = TriState::Zero;
		vppu.wire.PCLK = TriState::One;
		vppu.vid_out->sim(unused);

		vppu.wire.n_PCLK = TriState::One;
		vppu.wire.PCLK = TriState::Zero;
		vppu.vid_out->sim(unused);

		// And again, for PAL PPU

		vppu.wire.n_PCLK = TriState::Zero;
		vppu.wire.PCLK = TriState::One;
		vppu.vid_out->sim(unused);

		vppu.wire.n_PCLK = TriState::One;
		vppu.wire.PCLK = TriState::Zero;
		vppu.vid_out->sim(unused);

		// Warm up the Phase Shifter, because once it starts, it gives out garbage.

		TriState CLK = TriState::Zero;

		for (size_t n = 0; n < warmup_halfcycles; n++)
		{
			vppu.wire.n_CLK = NOT(CLK);
			vppu.wire.CLK = CLK;

			vppu.vid_out->sim(unused);

			CLK = NOT(CLK);
		}

		// Get a video signal batch equal to the number of phases

		VideoOutSignal* batch = new VideoOutSignal[num_phases];

		for (size_t n = 0; n < num_phases; n++)
		{
			vppu.wire.n_CLK = NOT(CLK);
			vppu.wire.CLK = CLK;

			vppu.vid_out->sim(batch[n]);

			CLK = NOT(CLK);
		}

		// Process the batch

		const float pi = 3.14159265358979323846f;
		float normalize_factor = 1.2f / features.WhiteLevel;	// 120 IRE  -- correction for measurements by HardWareMan
		float Y = 0, I = 0, Q = 0;
		size_t cb_phase = 9;

		for (size_t n = 0; n < num_phases; n++)
		{
			float level = ((batch[n].composite - features.BlackLevel) * normalize_factor) / num_phases;
			Y += level;
			I += level * cos((cb_phase + n) * (2 * pi / num_phases));
			Q += level * sin((cb_phase + n) * (2 * pi / num_phases)) * +1.0f;
		}

		// Note to PAL researchers. Read math behind PAL, or just use these:
		//		cb_phase = 1
		//		Q += level * sin((cb_phase + n) * (2 * π / num_phases)) * -1.0f;    <----   Minus

		delete[] batch;

		// 6500K color temperature

		float R = Y + (0.956f * I) + (0.623f * Q);
		float G = Y - (0.272f * I) - (0.648f * Q);
		float B = Y - (1.105f * I) + (1.705f * Q);

		rgbOut.RGB.RED = (uint8_t)(Clamp(R, 0.f, 1.f) * 255);
		rgbOut.RGB.GREEN = (uint8_t)(Clamp(G, 0.f, 1.f) * 255);
		rgbOut.RGB.BLUE = (uint8_t)(Clamp(B, 0.f, 1.f) * 255);
	}

	float VideoOut::Clamp(float val, float min, float max)
	{
		return std::min(max, std::max(val, min));
	}

	void VideoOut::SetupChromaDecoderPAL()
	{
		chroma_decoder = new PLA(chroma_decoder_inputs, chroma_decoder_outputs, (char*)"PALChromaDecoder.bin");

		// Set matrix

		static size_t bitmask[] = {
			0b1001100110,
			0b0110100101,
			0b1010100101,
			0b0101100110,
			0b1001010110,
			0b0110011001,
			0b1010011010,
			0b0101011010,
			0b1001101001,
			0b0110101001,
			0b0010101010,
			0b1010100110,
			0b0101101010,
			0b1001011001,
			0b0110010110,
			0b1010010110,
			0b0101011001,
			0b1001101010,
			0b0110100110,
			0b1010101001,
			0b0101101001,
			0b1001011010,
			0b0110011010,
			0b1010011001,
			0b0101010110,
		};

		chroma_decoder->SetMatrix(bitmask);
	}

#pragma region "RGB PPU Stuff"

	void VideoOut::SetupColorMatrix()
	{
		char colorMatrixName[0x20] = { 0 };

		// Generate PLA dump name according to the PPU revision.

		sprintf(colorMatrixName, "ColorMatrix_%s.bin", ppu->RevisionToStr(ppu->rev));

		color_matrix = new PLA(color_matrix_inputs, color_matrix_outputs, colorMatrixName);

		// Set matrix

		size_t* bitmask = nullptr;

		switch (ppu->rev)
		{
		case Revision::RP2C04_0003:
		{
			static size_t RP2C04_0003_ColorMatrix[] = {		// Undefined behavior in GCC if you do not specify static
				0b1010100001110110,
				0b1111000101010100,
				0b0000110101000000,
				0b0101111001100110,
				0b1000000001111011,
				0b0110110110110100,
				0b0001100101010011,
				0b1111101000110111,
				0b1010000001111000,
				0b1111110100111100,
				0b0110100101101001,
				0b1111100100010110,

				0b1010110001010110,
				0b0101010111100101,
				0b0000100101110000,
				0b1110101111001101,
				0b0001110011100001,
				0b0101111010010100,
				0b0100111111011000,
				0b1000001010111111,
				0b1010010001011110,
				0b0101110101011100,
				0b1010101111010110,
				0b1010010010111011,

				0b1110100100100110,
				0b0111011110100100,
				0b1011101101111001,
				0b0110011011100010,
				0b0000100110000111,
				0b0111101110110100,
				0b1010101001100100,
				0b0110011010101110,
				0b1010100101000110,
				0b1111101111111100,
				0b1011101001100100,
				0b0010110010101110,
			};

			bitmask = RP2C04_0003_ColorMatrix;
		}
		break;
		}

		if (bitmask != nullptr)
		{
			color_matrix->SetMatrix(bitmask);
		}
	}

	void VideoOut::sim_ColorMatrix()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState col[4]{};
		size_t packed = 0;

		if (PCLK == TriState::One)
		{
			TriState unpacked[16]{};
			col[0] = cc_latch2[2].nget();
			col[1] = cc_latch2[3].nget();
			col[2] = ppu->wire.n_LL[0];
			col[3] = ppu->wire.n_LL[1];

			DMX4(col, unpacked);
			packed = Pack(unpacked) | ((size_t)Pack(&unpacked[8]) << 8);
		}

		color_matrix->sim(packed, &rgb_output);
	}

	void VideoOut::sim_Select12To3()
	{
		TriState n_PCLK = ppu->wire.n_PCLK;
		TriState PCLK = ppu->wire.PCLK;
		TriState n_TR = ppu->wire.n_TR;
		TriState n_TG = ppu->wire.n_TG;
		TriState n_TB = ppu->wire.n_TB;
		TriState lum_in[2]{};

		lum_in[0] = NOR(n_PCLK, NOT(cc_latch2[0].nget()));
		lum_in[1] = NOR(n_PCLK, NOT(cc_latch2[1].nget()));

		red_sel.sim(PCLK, n_TR, &rgb_output[12 * 0], lum_in);
		green_sel.sim(PCLK, n_TG, &rgb_output[12 * 1], lum_in);
		blue_sel.sim(PCLK, n_TB, &rgb_output[12 * 2], lum_in);
	}

	void VideoOut::sim_RGB_DAC(VideoOutSignal& vout)
	{
		vout.RGB.RED = 0;
		vout.RGB.GREEN = 0;
		vout.RGB.BLUE = 0;

		// By multiplying by 35 we are just a little bit short of 255 for a component of 7, but that will do.

		const size_t Current_summation_factor = 35;

		for (size_t n = 0; n < 8; n++)
		{
			if (rgb_red_latch[n].get() == TriState::One)
			{
				vout.RGB.RED = (uint8_t)(n * Current_summation_factor);
			}

			if (rgb_green_latch[n].get() == TriState::One)
			{
				vout.RGB.GREEN = (uint8_t)(n * Current_summation_factor);
			}

			if (rgb_blue_latch[n].get() == TriState::One)
			{
				vout.RGB.BLUE = (uint8_t)(n * Current_summation_factor);
			}
		}

		vout.RGB.nSYNC = rgb_sync_latch[2].nget();
	}

	void RGB_SEL12x3::sim(TriState PCLK, TriState n_Tx, TriState col_in[12], TriState lum_in[2])
	{
		TriState mux_in[4]{};
		TriState mux_out{};

		mux_in[0] = col_in[8];
		mux_in[1] = col_in[10];
		mux_in[2] = col_in[9];
		mux_in[3] = col_in[11];
		mux_out = MUX2(lum_in, mux_in);
		ff[0].set(NAND(NOT(MUX(PCLK, ff[0].get(), mux_out)), n_Tx));

		mux_in[0] = col_in[4];
		mux_in[1] = col_in[6];
		mux_in[2] = col_in[5];
		mux_in[3] = col_in[7];
		mux_out = MUX2(lum_in, mux_in);
		ff[1].set(NAND(NOT(MUX(PCLK, ff[1].get(), mux_out)), n_Tx));

		mux_in[0] = col_in[0];
		mux_in[1] = col_in[2];
		mux_in[2] = col_in[1];
		mux_in[3] = col_in[3];
		mux_out = MUX2(lum_in, mux_in);
		ff[2].set(NAND(NOT(MUX(PCLK, ff[2].get(), mux_out)), n_Tx));
	}

	void RGB_SEL12x3::getOut(TriState col_out[3])
	{
		for (size_t n = 0; n < 3; n++)
		{
			col_out[n] = ff[n].get();
		}
	}

	void VideoOut::ConvertRAWToRGB_Component(VideoOutSignal& rawIn, VideoOutSignal& rgbOut)
	{
		// Using a temporary PPU of the same revision, to simulate the video generator

		PPU vppu(ppu->rev, true);

		// Latch all important signals

		vppu.wire.n_CC[0] = NOT(FromByte(rawIn.RAW.CC0));
		vppu.wire.n_CC[1] = NOT(FromByte(rawIn.RAW.CC1));
		vppu.wire.n_CC[2] = NOT(FromByte(rawIn.RAW.CC2));
		vppu.wire.n_CC[3] = NOT(FromByte(rawIn.RAW.CC3));
		vppu.wire.n_LL[0] = NOT(FromByte(rawIn.RAW.LL0));
		vppu.wire.n_LL[1] = NOT(FromByte(rawIn.RAW.LL1));
		vppu.wire.n_TR = NOT(FromByte(rawIn.RAW.TR));
		vppu.wire.n_TG = NOT(FromByte(rawIn.RAW.TG));
		vppu.wire.n_TB = NOT(FromByte(rawIn.RAW.TB));

		vppu.fsm.SYNC = FromByte(rawIn.RAW.Sync);
		vppu.fsm.n_PICTURE = TriState::Zero;

		size_t numHalfs = 6;

		vppu.wire.n_PCLK = TriState::One;
		vppu.wire.PCLK = TriState::Zero;

		for (size_t n = 0; n < numHalfs; n++)
		{
			vppu.vid_out->sim(rgbOut);
			vppu.wire.n_PCLK = NOT(vppu.wire.n_PCLK);
			vppu.wire.PCLK = NOT(vppu.wire.PCLK);
		}
	}

#pragma endregion "RGB PPU Stuff"

	bool VideoOut::IsComposite()
	{
		return composite;
	}

	float VideoOut::GetNoise()
	{
		float a = -noise;
		float b = noise;
		float r = a + (float)rand() / ((float)RAND_MAX / (b - a));
		return r;
	}

	void VideoOut::SetCompositeNoise(float volts)
	{
		if (volts != 0.0f)
		{
			srand((unsigned)time(0));
			noise = volts;
			noise_enable = true;
		}
		else
		{
			noise_enable = false;
		}
	}

	void VideoOut::ConvertRAWToRGB(VideoOutSignal& rawIn, VideoOutSignal& rgbOut)
	{
		if (composite)
		{
			ConvertRAWToRGB_Composite(rawIn, rgbOut);
		}
		else
		{
			ConvertRAWToRGB_Component(rawIn, rgbOut);
		}
	}

	// VRAM Controller

	VRAM_Control::VRAM_Control(PPU* parent)
	{
		ppu = parent;

		for (size_t n = 0; n < 8; n++)
		{
			RB[n] = new RB_Bit(ppu);
		}
	}

	VRAM_Control::~VRAM_Control()
	{
		for (size_t n = 0; n < 8; n++)
		{
			delete RB[n];
		}
	}

	void VRAM_Control::sim()
	{
		sim_RD();		// PD/RB, RD
		sim_WR();		// DB/PAR, TSTEP, WR
		sim_ALE();		// /ALE
	}

	void VRAM_Control::sim_RD()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;

		auto R7_EN = NOR(ppu->wire.n_R7, ppu->wire.n_DBE);
		R7_FF.set(NOR(rd_latch3.nget(), NOR(R7_EN, R7_FF.get())));
		RD_FF.set(MUX(PCLK, NOT(NOT(RD_FF.get())), NOR(R7_EN, R7_FF.nget())));
		rd_latch1.set(NOT(RD_FF.get()), n_PCLK);
		rd_latch2.set(rd_latch1.nget(), PCLK);
		rd_latch3.set(rd_latch2.nget(), n_PCLK);
		rd_latch4.set(rd_latch3.nget(), PCLK);
		tmp_2 = NOR(rd_latch4.get(), rd_latch2.nget());

		// PD/RB, RD

		ppu->wire.PD_RB = NOR(rd_latch4.nget(), rd_latch2.get());
		h0_latch.set(ppu->wire.H0_Dash, PCLK);
		ppu->wire.RD = NOT(NOR(AND(h0_latch.get(), NOT(ppu->fsm.BLNK)), ppu->wire.PD_RB));
	}

	void VRAM_Control::sim_WR()
	{
		TriState PCLK = ppu->wire.PCLK;
		TriState n_PCLK = ppu->wire.n_PCLK;

		auto W7_EN = NOR(ppu->wire.n_W7, ppu->wire.n_DBE);
		W7_FF.set(NOR(wr_latch3.nget(), NOR(W7_EN, W7_FF.get())));
		WR_FF.set(MUX(PCLK, NOT(NOT(WR_FF.get())), NOR(W7_EN, W7_FF.nget())));
		wr_latch1.set(NOT(WR_FF.get()), n_PCLK);
		wr_latch2.set(wr_latch1.nget(), PCLK);
		wr_latch3.set(wr_latch2.nget(), n_PCLK);
		wr_latch4.set(wr_latch3.nget(), PCLK);
		tmp_1 = NOR(wr_latch4.get(), wr_latch2.nget());

		// DB/PAR, TSTEP, WR

		ppu->wire.DB_PAR = NOR(wr_latch3.get(), wr_latch1.nget());
		tstep_latch.set(ppu->wire.DB_PAR, PCLK);
		ppu->wire.TSTEP = NOT(NOR(tstep_latch.get(), ppu->wire.PD_RB));
		ppu->wire.WR = NOR(NOT(ppu->wire.DB_PAR), ppu->wire.TH_MUX);
	}

	/// <summary>
	/// Call after Data Reader, but BEFORE MUX/CRAM & Read Buffer. PPU Address is obtained only after PAR simulation.
	/// </summary>
	void VRAM_Control::sim_TH_MUX()
	{
		// TH/MUX

		blnk_latch.set(ppu->fsm.BLNK, ppu->wire.PCLK);

		TriState in[7]{};
		in[0] = blnk_latch.nget();
		in[1] = ppu->wire.n_PA_Top[0];	// /PA8
		in[2] = ppu->wire.n_PA_Top[1];
		in[3] = ppu->wire.n_PA_Top[2];
		in[4] = ppu->wire.n_PA_Top[3];
		in[5] = ppu->wire.n_PA_Top[4];
		in[6] = ppu->wire.n_PA_Top[5];	// /PA13
		ppu->wire.TH_MUX = NOR7(in);

		// XRB

		auto R7_EN = NOR(ppu->wire.n_R7, ppu->wire.n_DBE);
		ppu->wire.XRB = NOT(NOR(NOT(R7_EN), ppu->wire.TH_MUX));
	}

	void VRAM_Control::sim_ALE()
	{
		ppu->wire.n_ALE = NOR3(NOR3(ppu->wire.H0_Dash, ppu->fsm.BLNK, ppu->wire.n_PCLK), tmp_1, tmp_2);
	}

	/// <summary>
	/// The Read Buffer should simulate after the Data Reader, after the PD bus gets a value to write to the DB.
	/// </summary>
	void VRAM_Control::sim_ReadBuffer()
	{
		for (size_t n = 0; n < 8; n++)
		{
			RB[n]->sim(n);
		}
	}

	void RB_Bit::sim(size_t bit_num)
	{
		TriState XRB = ppu->wire.XRB;
		TriState PD_RB = ppu->wire.PD_RB;
		TriState RC = ppu->wire.RC;

		ff.set(NOR(NOT(MUX(PD_RB, ff.get(), ppu->GetPDBit(bit_num))), RC));
		auto val = MUX(NOT(XRB), TriState::Z, ff.get());
		ppu->SetDBBit(bit_num, val);
	}

	TriState RB_Bit::get()
	{
		return ff.get();
	}

	void RB_Bit::set(TriState value)
	{
		ff.set(value);
	}

	// Pixel Clock

	void PPU::sim_PCLK()
	{
		TriState CLK = wire.CLK;
		TriState n_CLK = wire.n_CLK;
		TriState prev_pclk = wire.PCLK;
		TriState new_pclk{};

		// TBD: Add support for other PPU revisions.

		// In the original circuits there is also a single phase splitter, based on FF, but we are not simulating it here.

		switch (rev)
		{
		case Revision::RP2C02G:
		case Revision::RP2C02H:
		case Revision::RP2C04_0003:
		{
			pclk_1.set(NOR(wire.RES, pclk_4.nget()), n_CLK);
			pclk_2.set(pclk_1.nget(), CLK);
			pclk_3.set(pclk_2.nget(), n_CLK);
			pclk_4.set(pclk_3.nget(), CLK);

			new_pclk = pclk_4.nget();
		}
		break;

		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
		{
			pclk_1.set(NOR(pclk_6.get(), wire.RES), CLK);
			pclk_2.set(pclk_1.nget(), n_CLK);
			pclk_3.set(pclk_2.nget(), CLK);
			pclk_4.set(pclk_3.nget(), n_CLK);
			pclk_5.set(pclk_4.nget(), CLK);
			pclk_6.set(NOR(pclk_3.nget(), pclk_5.nget()), n_CLK);

			new_pclk = NOR(pclk_5.nget(), NOT(pclk_4.nget()));
		}
		break;
		}

		wire.PCLK = new_pclk;
		wire.n_PCLK = NOT(wire.PCLK);

		// The software PCLK counter is triggered by the falling edge.
		// This is purely a software design for convenience, and has nothing to do with PPU hardware circuitry.

		if (IsNegedge(prev_pclk, new_pclk))
		{
			pclk_counter++;
		}
	}

	// Main module and auxiliary logic.

	/// <summary>
	/// Constructor. Creates all PPU modules.
	/// </summary>
	/// <param name="_rev">Revision of the PPU chip.</param>
	/// <param name="VideoGen">true: Create a special version of the PPU that contains only a video generator.</param>
	PPU::PPU(Revision _rev, bool VideoGen)
	{
		rev = _rev;

		if (!VideoGen)
		{
			regs = new ControlRegs(this);
			h = new HVCounter(this, 9);
			v = new HVCounter(this, 9);
			hv_dec = new HVDecoder(this);
			hv_fsm = new FSM(this);
			cram = new CRAM(this);
			mux = new Mux(this);
			eval = new ObjEval(this);
			oam = new OAM(this);
			fifo = new FIFO(this);
			data_reader = new DataReader(this);
			vram_ctrl = new VRAM_Control(this);
		}

		vid_out = new VideoOut(this);
	}

	PPU::~PPU()
	{
		if (regs)
			delete regs;
		if (h)
			delete h;
		if (v)
			delete v;
		if (hv_dec)
			delete hv_dec;
		if (hv_fsm)
			delete hv_fsm;
		if (cram)
			delete cram;
		if (vid_out)
			delete vid_out;
		if (mux)
			delete mux;
		if (eval)
			delete eval;
		if (oam)
			delete oam;
		if (fifo)
			delete fifo;
		if (data_reader)
			delete data_reader;
		if (vram_ctrl)
			delete vram_ctrl;
	}

	void PPU::sim(TriState inputs[], TriState outputs[], uint8_t* ext, uint8_t* data_bus, uint8_t* ad_bus, uint8_t* addrHi_bus, VideoOutSignal& vout)
	{
		// Input terminals and binding

		wire.RnW = inputs[(size_t)InputPad::RnW];
		wire.RS[0] = inputs[(size_t)InputPad::RS0];
		wire.RS[1] = inputs[(size_t)InputPad::RS1];
		wire.RS[2] = inputs[(size_t)InputPad::RS2];
		wire.n_DBE = inputs[(size_t)InputPad::n_DBE];

		regs->sim_RWDecoder();

		wire.RES = NOT(inputs[(size_t)InputPad::n_RES]);

		if (wire.RES == TriState::One)
		{
			ResetPCLKCounter();
		}

		wire.CLK = inputs[(size_t)InputPad::CLK];
		wire.n_CLK = NOT(wire.CLK);

		sim_PCLK();

		hv_fsm->sim_RESCL_early();

		Reset_FF.set(NOR(fsm.RESCL, NOR(wire.RES, Reset_FF.get())));
		wire.RC = NOT(Reset_FF.nget());

		sim_BusInput(ext, data_bus, ad_bus);

		// Regs

		regs->sim();

		if (Prev_PCLK != wire.PCLK)
		{
			// H/V Control logic

			TriState* HPLA;
			hv_dec->sim_HDecoder(hv_fsm->get_VB(), hv_fsm->get_BLNK(wire.BLACK), &HPLA);
			TriState* VPLA;
			hv_dec->sim_VDecoder(&VPLA);

			hv_fsm->sim(HPLA, VPLA);

			h->sim(TriState::One, wire.HC);
			TriState V_IN = HPLA[23];
			v->sim(V_IN, wire.VC);

			// The other parts

			fifo->sim_SpriteH();

			regs->sim_CLP();

			vram_ctrl->sim();

			oam->sim_OFETCH();

			eval->sim();

			oam->sim();

			data_reader->sim();

			fifo->sim();

			vram_ctrl->sim_TH_MUX();

			vram_ctrl->sim_ReadBuffer();

			mux->sim();

			cram->sim();

			Prev_PCLK = wire.PCLK;
		}

		vid_out->sim(vout);

		// Output terminals

		outputs[(size_t)OutputPad::n_INT] = fsm.INT ? TriState::Zero : TriState::Z;
		outputs[(size_t)OutputPad::ALE] = NOT(wire.n_ALE);
		outputs[(size_t)OutputPad::n_RD] = NOT(wire.RD);
		outputs[(size_t)OutputPad::n_WR] = NOT(wire.WR);

		sim_BusOutput(ext, data_bus, ad_bus, addrHi_bus);
	}

	void PPU::sim_BusInput(uint8_t* ext, uint8_t* data_bus, uint8_t* ad_bus)
	{
		switch (rev)
		{
		case Revision::RP2C02G:
		case Revision::RP2C02H:
		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
		{
			TriState nSLAVE = regs->get_nSLAVE();

			for (size_t n = 0; n < 4; n++)
			{
				TriState extIn = (((*ext) >> n) & 1) ? TriState::One : TriState::Zero;
				wire.EXT_In[n] = NOR(NOT(extIn), nSLAVE);
			}
		}
		break;

		// TBD: The other RGB PPU's appear to be similar, but there are no pictures yet, we do not speculate.

		case Revision::RP2C04_0003:
		{
			for (size_t n = 0; n < 4; n++)
			{
				wire.EXT_In[n] = TriState::Zero;
			}
		}
		break;
		}

		if (wire.n_WR == TriState::Zero)
		{
			DB = *data_bus;
		}

		PD = *ad_bus;
	}

	void PPU::sim_BusOutput(uint8_t* ext, uint8_t* data_bus, uint8_t* ad_bus, uint8_t* addrHi_bus)
	{
		TriState n_PCLK = wire.n_PCLK;

		switch (rev)
		{
		case Revision::RP2C02G:
		case Revision::RP2C02H:
		case Revision::RP2C07_0:
		case Revision::UMC_UA6538:
		{
			for (size_t n = 0; n < 4; n++)
			{
				extout_latch[n].set(wire.n_EXT_Out[n], n_PCLK);
			}

			if (wire.n_SLAVE == TriState::One)
			{
				*ext = 0;
				for (size_t n = 0; n < 4; n++)
				{
					TriState extOut = NOR(NOT(wire.n_SLAVE), extout_latch[n].get());
					(*ext) |= ((extOut == TriState::One) ? 1 : 0) << n;
				}
			}
		}
		break;

		case Revision::RP2C04_0003:
		{
			// There are no EXT terminals in the RGB PPUs.
		}
		break;
		}

		if (wire.n_RD == TriState::Zero)
		{
			*data_bus = DB;
		}

		if (wire.RD == TriState::Zero)
		{
			uint8_t PABot = 0;
			for (size_t n = 0; n < 8; n++)
			{
				PABot |= (wire.n_PA_Bot[n] == TriState::Zero ? 1 : 0) << n;
			}
			*ad_bus = PABot;
		}

		uint8_t PATop = 0;
		for (size_t n = 0; n < 6; n++)
		{
			PATop |= (wire.n_PA_Top[n] == TriState::Zero ? 1 : 0) << n;
		}
		*addrHi_bus = PATop;
	}

	size_t PPU::GetPCLKCounter()
	{
		return pclk_counter;
	}

	void PPU::ResetPCLKCounter()
	{
		pclk_counter = 0;
		Prev_PCLK = TriState::X;
	}

	const char* PPU::RevisionToStr(Revision rev)
	{
		switch (rev)
		{
		case Revision::RP2C02G: return "RP2C02G";
		case Revision::RP2C02H: return "RP2C02H";

		case Revision::RP2C03B: return "RP2C03B";
		case Revision::RP2C03C: return "RP2C03C";
		case Revision::RC2C03B: return "RC2C03B";
		case Revision::RC2C03C: return "RC2C03C";
		case Revision::RP2C04_0001: return "RP2C04-0001";
		case Revision::RP2C04_0002: return "RP2C04-0002";
		case Revision::RP2C04_0003: return "RP2C04-0003";
		case Revision::RP2C04_0004: return "RP2C04-0004";
		case Revision::RC2C05_01: return "RC2C05-01";
		case Revision::RC2C05_02: return "RC2C05-02";
		case Revision::RC2C05_03: return "RC2C05-03";
		case Revision::RC2C05_04: return "RC2C05-04";
		case Revision::RC2C05_99: return "RC2C05-99";

		case Revision::RP2C07_0: return "RP2C07-0";

		case Revision::UMC_UA6538: return "UMC UA6538";

		default:
			break;
		}
		return "Unknown";
	}

	TriState PPU::GetDBBit(size_t n)
	{
		TriState DBBit = (DB & (1 << n)) != 0 ? TriState::One : TriState::Zero;
		return DBBit;
	}

	void PPU::SetDBBit(size_t n, TriState bit_val)
	{
		if (bit_val != TriState::Z)
		{
			uint8_t out = DB & ~(1 << n);
			out |= (bit_val == BaseLogic::One ? 1 : 0) << n;
			DB = out;
		}
	}

	TriState PPU::GetPDBit(size_t n)
	{
		TriState PDBit = (PD & (1 << n)) != 0 ? TriState::One : TriState::Zero;
		return PDBit;
	}

	void PPU::SetPDBit(size_t n, TriState bit_val)
	{
		if (bit_val != TriState::Z)
		{
			uint8_t out = PD & ~(1 << n);
			out |= (bit_val == BaseLogic::One ? 1 : 0) << n;
			PD = out;
		}
	}

	size_t PPU::GetHCounter()
	{
		return h->get();
	}

	size_t PPU::GetVCounter()
	{
		return v->get();
	}

	void PPU::GetSignalFeatures(VideoSignalFeatures& features)
	{
		vid_out->GetSignalFeatures(features);
	}

	void PPU::SetRAWOutput(bool enable)
	{
		vid_out->SetRAWOutput(enable);
	}

	void PPU::ConvertRAWToRGB(VideoOutSignal& rawIn, VideoOutSignal& rgbOut)
	{
		vid_out->ConvertRAWToRGB(rawIn, rgbOut);
	}

	void PPU::SetOamDecayBehavior(OAMDecayBehavior behavior)
	{
		if (oam != nullptr)
		{
			oam->SetOamDecayBehavior(behavior);
		}
	}

	bool PPU::IsComposite()
	{
		return vid_out->IsComposite();
	}

	void PPU::SetCompositeNoise(float volts)
	{
		vid_out->SetCompositeNoise(volts);
	}
}