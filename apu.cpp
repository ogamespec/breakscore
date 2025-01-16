#include "pch.h"

using namespace BaseLogic;

namespace APUSim
{
	// Common elements of APU circuitry

	void RegisterBit::sim(TriState ACLK1, TriState Enable, TriState Value)
	{
		transp_latch.set(MUX(ACLK1, MUX(Enable, TriState::Z, Value), NOT(transp_latch.nget())), TriState::One);
	}

	TriState RegisterBit::get()
	{
		return NOT(transp_latch.nget());
	}

	TriState RegisterBit::nget()
	{
		return transp_latch.nget();
	}

	void RegisterBit::set(TriState val)
	{
		transp_latch.set(val, TriState::One);
	}

	void RegisterBitRes::sim(TriState ACLK1, TriState Enable, TriState Value, TriState Res)
	{
		TriState d =
			MUX(ACLK1,
				MUX(Enable, TriState::Z, Value),
				NOT(transp_latch.nget()));
		if (Res == TriState::One) {
			d = TriState::Zero;
		}
		transp_latch.set(d, TriState::One);
	}

	TriState RegisterBitRes::get()
	{
		return NOT(transp_latch.nget());
	}

	TriState RegisterBitRes::nget()
	{
		return transp_latch.nget();
	}

	void RegisterBitRes::set(TriState val)
	{
		transp_latch.set(val, TriState::One);
	}

	void RegisterBitRes2::sim(TriState ACLK1, TriState Enable, TriState Value, TriState Res1, TriState Res2)
	{
		TriState d =
			MUX(ACLK1,
				MUX(Enable, TriState::Z, Value),
				NOT(transp_latch.nget()));
		if (OR(Res1, Res2) == TriState::One) {
			d = TriState::Zero;
		}
		transp_latch.set(d, TriState::One);
	}

	TriState RegisterBitRes2::get()
	{
		return NOT(transp_latch.nget());
	}

	TriState RegisterBitRes2::nget()
	{
		return transp_latch.nget();
	}

	void RegisterBitRes2::set(TriState val)
	{
		transp_latch.set(val, TriState::One);
	}

	TriState CounterBit::sim(TriState Carry, TriState Clear, TriState Load, TriState Step, TriState ACLK1, TriState val)
	{
		TriState latch_in =
			MUX(Load,
				MUX(Clear,
					MUX(Step,
						MUX(ACLK1, TriState::Z, NOT(transp_latch.nget())),
						cg_latch.nget()),
					TriState::Zero),
				val);

		transp_latch.set(latch_in, TriState::One);
		cg_latch.set(MUX(Carry, transp_latch.nget(), NOT(transp_latch.nget())), ACLK1);

		TriState cout = NOR(NOT(Carry), transp_latch.nget());
		return cout;
	}

	TriState CounterBit::get()
	{
		return NOT(transp_latch.nget());
	}

	TriState CounterBit::nget()
	{
		return transp_latch.nget();
	}

	void CounterBit::set(BaseLogic::TriState val)
	{
		transp_latch.set(val, TriState::One);
	}

	TriState DownCounterBit::sim(TriState Carry, TriState Clear, TriState Load, TriState Step, TriState ACLK1, TriState val)
	{
		TriState latch_in =
			MUX(Load,
				MUX(Clear,
					MUX(Step,
						MUX(ACLK1, TriState::Z, NOT(transp_latch.nget())),
						cg_latch.nget()),
					TriState::Zero),
				val);

		transp_latch.set(latch_in, TriState::One);
		cg_latch.set(MUX(Carry, transp_latch.nget(), NOT(transp_latch.nget())), ACLK1);

		TriState cout = NOR(NOT(Carry), NOT(transp_latch.nget()));
		return cout;
	}

	TriState DownCounterBit::get()
	{
		return NOT(transp_latch.nget());
	}

	TriState DownCounterBit::nget()
	{
		return transp_latch.nget();
	}

	void DownCounterBit::set(BaseLogic::TriState val)
	{
		transp_latch.set(val, TriState::One);
	}

	TriState RevCounterBit::sim(TriState Carry, TriState Dec, TriState Clear, TriState Load, TriState Step, TriState ACLK1, TriState val)
	{
		TriState latch_in =
			MUX(Load,
				MUX(Clear,
					MUX(Step,
						MUX(ACLK1, TriState::Z, NOT(transp_latch.nget())),
						cg_latch.nget()),
					TriState::Zero),
				val);

		transp_latch.set(latch_in, TriState::One);
		cg_latch.set(MUX(Carry, transp_latch.nget(), NOT(transp_latch.nget())), ACLK1);

		TriState cout = NOR(NOT(Carry), MUX(Dec, transp_latch.nget(), NOT(transp_latch.nget())));
		return cout;
	}

	TriState RevCounterBit::get()
	{
		return NOT(transp_latch.nget());
	}

	TriState RevCounterBit::nget()
	{
		return transp_latch.nget();
	}

	void RevCounterBit::set(BaseLogic::TriState val)
	{
		transp_latch.set(val, TriState::One);
	}

	// Timing Generator

	CLKGen::CLKGen(APU* parent)
	{
		apu = parent;
	}

	CLKGen::~CLKGen()
	{
	}

	void CLKGen::sim()
	{
		sim_ACLK();
		sim_SoftCLK_Mode();
		sim_SoftCLK_PLA();
		sim_SoftCLK_Control();
		sim_SoftCLK_LFSR();
	}

	void CLKGen::sim_ACLK()
	{
		TriState PHI1 = apu->wire.PHI1;
		TriState PHI2 = apu->wire.PHI2;
		TriState RES = apu->wire.RES;
		TriState prev_aclk = apu->wire.nACLK2;

		TriState temp = NOR(RES, phi2_latch.nget());
		phi1_latch.set(temp, PHI1);
		phi2_latch.set(phi1_latch.nget(), PHI2);

		TriState new_aclk = NOT(NOR(NOT(PHI1), temp));

		apu->wire.nACLK2 = new_aclk;
		apu->wire.ACLK1 = NOR(NOT(PHI1), phi2_latch.nget());

		// The software ACLK counter is triggered by the falling edge.
		// This is purely a software design for convenience, and has nothing to do with APU hardware circuitry.

		if (IsNegedge(prev_aclk, new_aclk))
		{
			apu->aclk_counter++;
		}
	}

	void CLKGen::sim_SoftCLK_Mode()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W4017 = apu->wire.W4017;

		// Mode

		reg_mode.sim(ACLK1, W4017, apu->GetDBBit(7));
		n_mode = reg_mode.nget();
		md_latch.set(n_mode, ACLK1);
		mode = md_latch.nget();
	}

	void CLKGen::sim_SoftCLK_Control()
	{
		TriState nACLK2 = apu->wire.nACLK2;
		TriState ACLK1 = apu->wire.ACLK1;
		TriState PHI1 = apu->wire.PHI1;
		TriState RES = apu->wire.RES;
		TriState DMCINT = apu->wire.DMCINT;
		TriState n_R4015 = apu->wire.n_R4015;
		TriState W4017 = apu->wire.W4017;

		// Interrupt(s)

		reg_mask.sim(ACLK1, W4017, apu->GetDBBit(6));
		int_ff.set(NOR4(NOR(int_ff.get(), AND(n_mode, pla[3])), NOR(n_R4015, PHI1), reg_mask.get(), RES));
		int_status.set(int_ff.nget(), ACLK1);
		apu->SetDBBit(6, NOT(n_R4015) == TriState::One ? NOT(int_status.get()) : TriState::Z);
		apu->wire.INT = NOT(NOR(int_ff.get(), DMCINT));

		// LFSR reload

		z1.set(z_ff.get(), ACLK1);
		z2.set(n_mode, ACLK1);
		z_ff.set(NOR3(NOR(z_ff.get(), NOR(z1.get(), nACLK2)), W4017, RES));
		Z1 = z1.nget();
		Z2 = NOR(z1.get(), z2.get());

		// LFSR operation

		TriState ftemp = NOR3(Z1, pla[3], pla[4]);
		F1 = NOR(ftemp, nACLK2);
		F2 = NOR(NOT(ftemp), nACLK2);

		// LFO Outputs

		apu->wire.n_LFO1 = NOT(NOR(NOR6(pla[0], pla[1], pla[2], pla[3], pla[4], Z2), nACLK2));
		apu->wire.n_LFO2 = NOT(NOR(NOR4(pla[1], pla[3], pla[4], Z2), nACLK2));
	}

	void CLKGen::sim_SoftCLK_PLA()
	{
		BaseLogic::TriState s[15]{};
		BaseLogic::TriState ns[15]{};

		for (size_t n = 0; n < 15; n++)
		{
			s[n] = lfsr[n].get_sout();
			ns[n] = lfsr[n].get_nsout();
		}

		pla[0] = NOR15(ns[0], s[1], s[2], s[3], s[4], ns[5], ns[6], s[7], s[8], s[9], s[10], s[11], ns[12], s[13], s[14]);
		pla[1] = NOR15(ns[0], ns[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], ns[9], ns[10], s[11], ns[12], ns[13], s[14]);
		pla[2] = NOR15(ns[0], ns[1], s[2], s[3], ns[4], s[5], ns[6], ns[7], s[8], s[9], ns[10], ns[11], s[12], ns[13], s[14]);
		pla[3] = NOR16(ns[0], ns[1], ns[2], ns[3], ns[4], s[5], s[6], s[7], s[8], ns[9], s[10], ns[11], s[12], s[13], s[14], mode);	// ⚠️
		pla[4] = NOR15(ns[0], s[1], ns[2], s[3], s[4], s[5], s[6], ns[7], ns[8], s[9], s[10], s[11], ns[12], ns[13], ns[14]);
		pla[5] = NOR15(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], s[9], s[10], s[11], s[12], s[13], s[14]);
	}

	void CLKGen::sim_SoftCLK_LFSR()
	{
		TriState ACLK1 = apu->wire.ACLK1;

		// Feedback

		TriState C13 = lfsr[13].get_sout();
		TriState C14 = lfsr[14].get_sout();
		shift_in = NOR(AND(C13, C14), NOR3(C13, C14, pla[5]));

		// SR15

		for (size_t n = 0; n < 15; n++)
		{
			lfsr[n].sim(ACLK1, F1, F2, shift_in);
			shift_in = lfsr[n].get_sout();
		}
	}

	void SoftCLK_SRBit::sim(TriState ACLK1, TriState F1, TriState F2, TriState shift_in)
	{
		in_latch.set(MUX(F2, MUX(F1, TriState::Z, TriState::One), shift_in), TriState::One);
		out_latch.set(in_latch.nget(), ACLK1);
	}

	TriState SoftCLK_SRBit::get_sout()
	{
		return out_latch.nget();
	}

	TriState SoftCLK_SRBit::get_nsout()
	{
		return out_latch.get();
	}

	TriState CLKGen::GetINTFF()
	{
		return int_ff.get();
	}

	// 6502 Core Binding

	CoreBinding::CoreBinding(APU* parent)
	{
		apu = parent;
	}

	CoreBinding::~CoreBinding()
	{
	}

	void CoreBinding::sim()
	{
		sim_DividerBeforeCore();

		if (apu->wire.PHI0 != apu->PrevPHI_Core)
		{
			TriState inputs[(size_t)M6502Core::InputPad::Max]{};
			TriState outputs[(size_t)M6502Core::OutputPad::Max];

			TriState nIRQINT = NOR(NOT(apu->wire.n_IRQ), apu->wire.INT);

			inputs[(size_t)M6502Core::InputPad::n_NMI] = apu->wire.n_NMI;
			inputs[(size_t)M6502Core::InputPad::n_IRQ] = nIRQINT;
			inputs[(size_t)M6502Core::InputPad::n_RES] = NOT(apu->wire.RES);
			inputs[(size_t)M6502Core::InputPad::PHI0] = apu->wire.PHI0;
			inputs[(size_t)M6502Core::InputPad::RDY] = AND(apu->wire.RDY, apu->wire.RDY2);
			inputs[(size_t)M6502Core::InputPad::SO] = TriState::One;

			apu->core->sim(inputs, outputs, &apu->CPU_Addr, &apu->DB);

			apu->wire.PHI1 = outputs[(size_t)M6502Core::OutputPad::PHI1];
			apu->wire.PHI2 = outputs[(size_t)M6502Core::OutputPad::PHI2];
			apu->wire.RnW = outputs[(size_t)M6502Core::OutputPad::RnW];
			apu->wire.SYNC = outputs[(size_t)M6502Core::OutputPad::SYNC];

			apu->PrevPHI_Core = apu->wire.PHI0;
		}

		sim_DividerAfterCore();
	}

	void CoreBinding::sim_DividerBeforeCore()
	{
		TriState n_CLK = apu->wire.n_CLK;
		TriState prev_phi = apu->wire.PHI0;

		// Phase splitter

		CLK_FF.set(NOR(NOR(NOT(n_CLK), CLK_FF.get()), NOT(NOT(n_CLK))));
		TriState q = CLK_FF.get();
		TriState nq = CLK_FF.nget();

		// Jhonson counter (back propagate)

		div[5].sim(q, nq, TriState::Zero, div[4].get_sout(TriState::Zero));
		div[4].sim(q, nq, TriState::Zero, div[3].get_sout(TriState::Zero));

		TriState new_phi = NOT(div[5].get_sout(TriState::Zero));
		apu->wire.PHI0 = new_phi;
		TriState rst = NOR(apu->wire.PHI0, div[4].get_sout(TriState::Zero));

		div[3].sim(q, nq, rst, div[2].get_sout(rst));
		div[2].sim(q, nq, rst, div[1].get_sout(rst));
		div[1].sim(q, nq, rst, div[0].get_sout(rst));
		div[0].sim(q, nq, rst, apu->wire.PHI0);

		// TBD: Other APU revisions

		// The software PHI counter is triggered by the falling edge.
		// This is purely a software design for convenience, and has nothing to do with APU hardware circuitry.
		// CPU cycles count by falling edge means that it is a counter of FULL cycles (i.e. PHI1+PHI2 of the processor are executed)

		if (IsNegedge(prev_phi, new_phi))
		{
			apu->phi_counter++;
		}
	}

	void CoreBinding::sim_DividerAfterCore()
	{
		apu->wire.n_M2 = NOR(div[4].get_nval(), apu->wire.PHI2);

		// TBD: Other APU revisions
	}

	void DIV_SRBit::sim(TriState q, TriState nq, TriState rst, TriState sin)
	{
		in_latch.set(sin, q);
		out_latch.set(in_latch.nget(), nq);
	}

	TriState DIV_SRBit::get_sout(TriState rst)
	{
		return NOR(out_latch.get(), rst);
	}

	TriState DIV_SRBit::get_nval()
	{
		return in_latch.nget();
	}

	// OAM DMA

	DMA::DMA(APU* parent)
	{
		apu = parent;
	}

	DMA::~DMA()
	{
	}

	void DMA::sim()
	{
		sim_DMA_Control();
		sim_DMA_Address();
	}

	void DMA::sim_DMA_Address()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W4014 = apu->wire.W4014;	// Load / SetAddr
		TriState RES = apu->wire.RES;		// Clear

		// Low

		TriState carry = TriState::One;
		for (size_t n = 0; n < 8; n++)
		{
			carry = spr_lo[n].sim(carry, RES, W4014, SPRS, ACLK1, TriState::Zero);
		}
		SPRE = carry;

		spre_latch.set(SPRE, ACLK1);

		// High

		for (size_t n = 0; n < 8; n++)
		{
			spr_hi[n].sim(ACLK1, W4014, apu->GetDBBit(n));
		}

		// SPR_Addr

		apu->SPR_Addr = 0;

		for (size_t n = 0; n < 8; n++)
		{
			apu->SPR_Addr |= ((spr_lo[n].get() == TriState::One ? 1 : 0) << n);
			apu->SPR_Addr |= ((spr_hi[n].get() == TriState::One ? 1 : 0) << (8 + n));
		}
	}

	void DMA::sim_DMA_Control()
	{
		TriState ACLK2 = NOT(apu->wire.nACLK2);
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;
		TriState W4014 = apu->wire.W4014;
		TriState RUNDMC = apu->wire.RUNDMC;
		TriState PHI1 = apu->wire.PHI1;
		TriState RnW = apu->wire.RnW;
		TriState DMCRDY = apu->wire.DMCRDY;

		DMADirToggle.set(NOR(ACLK1, NOR(ACLK2, DMADirToggle.get())));

		NOSPR = nospr_latch.nget();

		StartDMA.set(NOR3(NOT(NOSPR), RES, NOR(W4014, StartDMA.get())));
		dospr_latch.set(StartDMA.nget(), ACLK2);
		DOSPR = NOR(dospr_latch.get(), NAND(NOT(PHI1), RnW));

		SPRS = NOR3(NOSPR, RUNDMC, NOT(ACLK2));
		StopDMA.set(NOR3(AND(SPRS, spre_latch.get()), RES, NOR(DOSPR, StopDMA.get())));
		nospr_latch.set(StopDMA.get(), ACLK1);

		sprdma_rdy = NOR(NOT(NOSPR), StartDMA.get());
		apu->wire.RDY = AND(sprdma_rdy, DMCRDY);

		apu->wire.SPR_PPU = NOR3(NOSPR, RUNDMC, NOT(DMADirToggle.get()));
		apu->wire.SPR_CPU = NOR3(NOSPR, RUNDMC, DMADirToggle.get());
	}

	void DMA::sim_DMA_Buffer()
	{
		apu->wire.RW = NOR(apu->wire.SPR_PPU, NOT(apu->wire.RnW));
		apu->wire.RD = apu->wire.RW;
		apu->wire.WR = NAND3(apu->wire.n_R4015, apu->wire.n_DBGRD, apu->wire.RW);

		for (size_t n = 0; n < 8; n++)
		{
			spr_buf[n].set(apu->GetDBBit(n), apu->wire.PHI2);
		}

		if (apu->wire.SPR_PPU)
		{
			for (size_t n = 0; n < 8; n++)
			{
				apu->SetDBBit(n, NOT(spr_buf[n].nget()));
			}
		}
	}

	void DMA::sim_AddressMux()
	{
		TriState DMC_AB = NOT(apu->wire.n_DMC_AB);
		TriState CPU_AB = NOR3(apu->wire.SPR_CPU, apu->wire.SPR_PPU, DMC_AB);

		// Only one tristate can be active

		if (DMC_AB == TriState::One)
		{
			apu->Ax = apu->DMC_Addr;
		}
		if (apu->wire.SPR_CPU == TriState::One)
		{
			apu->Ax = apu->SPR_Addr;
		}
		if (apu->wire.SPR_PPU == TriState::One)
		{
			apu->Ax = PPU_Addr;
		}
		if (CPU_AB == TriState::One)
		{
			apu->Ax = apu->CPU_Addr;
		}
	}

	// Differential Pulse-code Modulation (DPCM) Channel

	DpcmChan::DpcmChan(APU* parent)
	{
		apu = parent;
		// msb is not used. This is done for the convenience of packing the value in byte.
		apu->DMC_Out[7] = TriState::Zero;
	}

	DpcmChan::~DpcmChan()
	{
	}

	void DpcmChan::sim()
	{
		ACLK2 = NOT(apu->wire.nACLK2);

		sim_SampleCounterReg();

		sim_ControlReg();
		sim_Decoder1();
		sim_Decoder2();

		sim_FreqLFSR();

		sim_IntControl();
		sim_EnableControl();
		sim_DMAControl();

		sim_SampleCounterControl();
		sim_SampleCounter();
		sim_SampleBitCounter();

		sim_SampleBufferControl();
		sim_SampleBuffer();

		sim_AddressReg();
		sim_AddressCounter();
		sim_Output();
	}

#pragma region "DPCM Control"

	void DpcmChan::sim_ControlReg()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W4010 = apu->wire.W4010;

		for (size_t n = 0; n < 4; n++)
		{
			freq_reg[n].sim(ACLK1, W4010, apu->GetDBBit(n));
			Fx[n] = freq_reg[n].get();
		}

		loop_reg.sim(ACLK1, W4010, apu->GetDBBit(6));
		LOOPMode = loop_reg.get();

		irq_reg.sim(ACLK1, W4010, apu->GetDBBit(7));
		n_IRQEN = irq_reg.nget();
	}

	void DpcmChan::sim_IntControl()
	{
		TriState W4015 = apu->wire.W4015;
		TriState n_R4015 = apu->wire.n_R4015;
		TriState RES = apu->wire.RES;
		TriState AssertInt = ED1;

		int_ff.set(NOR4(NOR(int_ff.get(), AssertInt), W4015, n_IRQEN, RES));
		apu->SetDBBit(7, NOT(n_R4015) == TriState::One ? NOT(int_ff.nget()) : TriState::Z);
		apu->wire.DMCINT = NOR(int_ff.nget(), n_IRQEN);
	}

	void DpcmChan::sim_EnableControl()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W4015 = apu->wire.W4015;
		TriState n_R4015 = apu->wire.n_R4015;
		TriState RES = apu->wire.RES;
		TriState PCMDone = get_DMC1();

		sout_latch.set(SOUT, ACLK1);
		DMC2 = sout_latch.get();
		ED1 = NOR3(LOOPMode, sout_latch.nget(), NOT(PCMDone));
		ena_ff.sim(ACLK1, W4015, apu->GetDBBit(4), ED1, RES);
		ED2 = ena_ff.get();
		apu->SetDBBit(4, NOT(n_R4015) == TriState::One ? NOT(ena_ff.nget()) : TriState::Z);
	}

	void DpcmChan::sim_DMAControl()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState PHI1 = apu->wire.PHI1;
		TriState RnW = apu->wire.RnW;
		TriState RES = apu->wire.RES;
		TriState nDMCEnableDelay = get_CTRL2();
		TriState nDMAStop = get_CTRL1();

		run_latch1.set(start_ff.get(), ACLK2);
		run_latch2.set(run_latch1.nget(), ACLK1);
		TriState start_set = NOR3(NAND(RnW, NOT(PHI1)), nDMCEnableDelay, NOT(nDMAStop));
		start_ff.set(NOR4(NOR(start_ff.get(), start_set), nDMCEnableDelay, RES, NOT(nDMAStop)));
		rdy_ff.set(NOR(AND(run_latch1.get(), ACLK1), NOR(rdy_ff.get(), ACLK2)));

		apu->wire.RUNDMC = run_latch2.nget();
		apu->wire.n_DMC_AB = rdy_ff.get();
		apu->wire.DMCRDY = NOR(start_ff.get(), rdy_ff.nget());
	}

	void DpcmChan::sim_SampleCounterControl()
	{
		TriState nACLK2 = apu->wire.nACLK2;
		TriState ACLK1 = apu->wire.ACLK1;
		TriState PCMDone = get_DMC1();
		TriState DMCFinish = DMC2;
		TriState DMCEnable = ED2;

		en_latch1.set(DMCEnable, ACLK1);
		en_latch2.set(en_latch1.nget(), ACLK2);
		en_latch3.set(en_latch2.nget(), ACLK1);
		CTRL2 = en_latch3.nget();
		TriState DMC3 = NOR3(nACLK2, en_latch1.nget(), en_latch3.get());

		NSTEP = NOT(NOT(DFLOAD));
		DSLOAD = NOT(NOR(AND(DMCFinish, PCMDone), DMC3));
		DSSTEP = NOR3(NOT(PCMDone), DMC3, DMCFinish);
	}

	void DpcmChan::sim_SampleBufferControl()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;
		TriState PHI1 = apu->wire.PHI1;
		TriState n_DMC_AB = apu->wire.n_DMC_AB;
		TriState LOCK = apu->wire.LOCK;
		TriState n_DFLOAD = NOT(DFLOAD);

		step_ff.set(NOR3(NOR(step_ff.get(), BLOAD), NOR3(NOT(stop_latch.nget()), n_DFLOAD, n_NOUT), RES));
		stop_ff.set(NOR3(NOR(stop_ff.get(), PCM), BLOAD, RES));
		CTRL1 = stop_ff.nget();
		pcm_ff.set(NOR3(NOR(pcm_ff.get(), PCM), DMC1, RES));

		dout_latch.set(DOUT, ACLK1);
		dstep_latch.set(step_ff.nget(), ACLK1);
		stop_latch.set(stop_ff.get(), ACLK1);
		pcm_latch.set(pcm_ff.nget(), ACLK1);

		PCM = NOR(PHI1, n_DMC_AB);
		DMC1 = NOR(pcm_latch.get(), NOT(ACLK2));
		DSTEP = NOR4(dout_latch.get(), dstep_latch.get(), n_DFLOAD, LOCK);
		BLOAD = NOR3(stop_latch.nget(), n_DFLOAD, n_NOUT);
		BSTEP = NOR(n_DFLOAD, NOT(n_NOUT));
	}

	TriState DpcmChan::get_CTRL1()
	{
		return stop_ff.nget();
	}

	TriState DpcmChan::get_CTRL2()
	{
		return en_latch3.nget();
	}

	TriState DpcmChan::get_DMC1()
	{
		return NOR(pcm_latch.get(), NOT(ACLK2));
	}

#pragma endregion "DPCM Control"

#pragma region "DPCM Sampling"

	void DpcmChan::sim_Decoder1()
	{
		TriState F[4]{};
		TriState nF[4]{};

		for (size_t n = 0; n < 4; n++)
		{
			F[n] = Fx[n];
			nF[n] = NOT(Fx[n]);
		}

		Dec1_out[0] = NOR4(F[0], F[1], F[2], F[3]);
		Dec1_out[1] = NOR4(nF[0], F[1], F[2], F[3]);
		Dec1_out[2] = NOR4(F[0], nF[1], F[2], F[3]);
		Dec1_out[3] = NOR4(nF[0], nF[1], F[2], F[3]);
		Dec1_out[4] = NOR4(F[0], F[1], nF[2], F[3]);
		Dec1_out[5] = NOR4(nF[0], F[1], nF[2], F[3]);
		Dec1_out[6] = NOR4(F[0], nF[1], nF[2], F[3]);
		Dec1_out[7] = NOR4(nF[0], nF[1], nF[2], F[3]);

		Dec1_out[8] = NOR4(F[0], F[1], F[2], nF[3]);
		Dec1_out[9] = NOR4(nF[0], F[1], F[2], nF[3]);
		Dec1_out[10] = NOR4(F[0], nF[1], F[2], nF[3]);
		Dec1_out[11] = NOR4(nF[0], nF[1], F[2], nF[3]);
		Dec1_out[12] = NOR4(F[0], F[1], nF[2], nF[3]);
		Dec1_out[13] = NOR4(nF[0], F[1], nF[2], nF[3]);
		Dec1_out[14] = NOR4(F[0], nF[1], nF[2], nF[3]);
		Dec1_out[15] = NOR4(nF[0], nF[1], nF[2], nF[3]);
	}

	void DpcmChan::sim_Decoder2()
	{
		TriState* d = Dec1_out;

		FR[0] = NOR5(d[1], d[4], d[9], d[14], d[15]);
		FR[1] = NOR8(d[6], d[7], d[8], d[9], d[10], d[11], d[12], d[13]);
		FR[2] = NOR9(d[0], d[1], d[2], d[3], d[7], d[8], d[10], d[11], d[13]);
		FR[3] = NOR5(d[0], d[2], d[7], d[10], d[15]);
		FR[4] = NOR7(d[1], d[2], d[4], d[8], d[12], d[13], d[14]);
		FR[5] = NOR10(d[1], d[2], d[3], d[7], d[8], d[9], d[12], d[13], d[14], d[15]);
		FR[6] = NOR6(d[1], d[5], d[8], d[12], d[13], d[14]);
		FR[7] = NOR6(d[0], d[2], d[5], d[6], d[8], d[15]);
		FR[8] = NOR9(d[1], d[3], d[5], d[6], d[8], d[9], d[10], d[11], d[12]);
	}

	void DpcmChan::sim_FreqLFSR()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;

		TriState sout[9]{};

		for (size_t n = 0; n < 9; n++)
		{
			sout[n] = lfsr[n].get_sout();
		}

		TriState nor1_out = NOR9(sout[0], sout[1], sout[2], sout[3], sout[4], sout[5], sout[6], sout[7], sout[8]);
		TriState nor2_out = NOR9(NOT(sout[0]), sout[1], sout[2], sout[3], sout[4], sout[5], sout[6], sout[7], sout[8]);

		TriState feedback = NOR3(AND(sout[0], sout[4]), RES, NOR3(sout[0], sout[4], nor1_out));
		TriState nor3_out = NOR(RES, NOT(nor2_out));
		DFLOAD = NOR(NOT(ACLK2), NOT(nor3_out));
		TriState DFSTEP = NOR(NOT(ACLK2), nor3_out);

		TriState shift_in = feedback;

		for (int n = 8; n >= 0; n--)
		{
			lfsr[n].sim(ACLK1, DFLOAD, DFSTEP, FR[n], shift_in);
			shift_in = lfsr[n].get_sout();
		}
	}

	void DPCM_LFSRBit::sim(TriState ACLK1, TriState load, TriState step, TriState val, TriState shift_in)
	{
		in_latch.set(MUX(load, MUX(step, TriState::Z, shift_in), val), TriState::One);
		out_latch.set(in_latch.nget(), ACLK1);
	}

	TriState DPCM_LFSRBit::get_sout()
	{
		return out_latch.nget();
	}

	void DpcmChan::sim_SampleCounterReg()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W4013 = apu->wire.W4013;

		for (size_t n = 0; n < 8; n++)
		{
			scnt_reg[n].sim(ACLK1, W4013, apu->GetDBBit(n));
		}
	}

	void DpcmChan::sim_SampleCounter()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;

		TriState carry = TriState::One;

		for (size_t n = 0; n < 12; n++)
		{
			carry = scnt[n].sim(carry, RES, DSLOAD, DSSTEP, ACLK1, n < 4 ? TriState::Zero : scnt_reg[n - 4].get());
		}

		SOUT = carry;
	}

	void DpcmChan::sim_SampleBitCounter()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;

		TriState carry = TriState::One;

		for (size_t n = 0; n < 3; n++)
		{
			carry = sbcnt[n].sim(carry, RES, RES, NSTEP, ACLK1, TriState::Zero);
		}

		nout_latch.set(carry, ACLK1);
		n_NOUT = nout_latch.nget();
	}

	void DpcmChan::sim_SampleBuffer()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;

		for (size_t n = 0; n < 8; n++)
		{
			buf_reg[n].sim(ACLK1, PCM, apu->GetDBBit(n));
		}

		TriState shift_in = TriState::Zero;

		for (int n = 7; n >= 0; n--)
		{
			shift_reg[n].sim(ACLK1, RES, BLOAD, BSTEP, buf_reg[n].nget(), shift_in);
			shift_in = shift_reg[n].get_sout();
		}

		n_BOUT = NOT(shift_in);
	}

	void DPCM_SRBit::sim(TriState ACLK1, TriState clear, TriState load, TriState step, TriState n_val, TriState shift_in)
	{
		in_latch.set(shift_in, ACLK1);
		TriState d =
			MUX(clear,
				MUX(load,
					MUX(step,
						MUX(ACLK1, TriState::Z, NOT(out_latch.nget())),
						in_latch.nget()),
					n_val),
				TriState::Zero);
		out_latch.set(d, TriState::One);
	}

	TriState DPCM_SRBit::get_sout()
	{
		return out_latch.nget();
	}

#pragma endregion "DPCM Sampling"

#pragma region "DPCM Addressing & Output"

	void DpcmChan::sim_AddressReg()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W4012 = apu->wire.W4012;

		for (size_t n = 0; n < 8; n++)
		{
			addr_reg[n].sim(ACLK1, W4012, apu->GetDBBit(n));
		}
	}

	void DpcmChan::sim_AddressCounter()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;

		TriState carry = TriState::One;

		for (size_t n = 0; n < 8; n++)
		{
			carry = addr_lo[n].sim(carry, RES, DSLOAD, DSSTEP, ACLK1, n < 6 ? TriState::Zero : addr_reg[n - 6].get());
		}

		for (size_t n = 0; n < 7; n++)
		{
			carry = addr_hi[n].sim(carry, RES, DSLOAD, DSSTEP, ACLK1, n < 6 ? addr_reg[n + 2].get() : TriState::One);
		}

		apu->DMC_Addr = 0;
		for (size_t n = 0; n < 8; n++)
		{
			apu->DMC_Addr |= (addr_lo[n].get() == TriState::One ? 1 : 0) << n;
			if (n < 7)
			{
				apu->DMC_Addr |= (addr_hi[n].get() == TriState::One ? 1 : 0) << (8 + n);
			}
		}
		apu->DMC_Addr |= 0x8000;
	}

	void DpcmChan::sim_Output()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;
		TriState W4011 = apu->wire.W4011;
		TriState CountDown = n_BOUT;

		TriState carry = TriState::One;

		for (size_t n = 0; n < 6; n++)
		{
			carry = out_cnt[n].sim(carry, CountDown, RES, W4011, DSTEP, ACLK1, apu->GetDBBit(n + 1));
		}

		DOUT = carry;

		out_reg.sim(ACLK1, W4011, apu->GetDBBit(0));

		apu->DMC_Out[0] = out_reg.get();
		for (size_t n = 0; n < 6; n++)
		{
			apu->DMC_Out[n + 1] = out_cnt[n].get();
		}
	}

#pragma endregion "DPCM Addressing & Output"

	// Length Counters

	LengthCounter::LengthCounter(APU* parent)
	{
		apu = parent;
	}

	LengthCounter::~LengthCounter()
	{
	}

	void LengthCounter::sim(size_t bit_ena, TriState WriteEn, TriState LC_CarryIn, TriState& LC_NoCount)
	{
		sim_Decoder1();
		sim_Decoder2();
		sim_Counter(LC_CarryIn, WriteEn);
		sim_Control(bit_ena, WriteEn, LC_NoCount);
	}

	void LengthCounter::sim_Control(size_t bit_ena, TriState WriteEn, TriState& LC_NoCount)
	{
		TriState nACLK2 = apu->wire.nACLK2;
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;
		TriState n_LFO2 = apu->wire.n_LFO2;
		TriState W4015 = apu->wire.W4015;
		TriState n_R4015 = apu->wire.n_R4015;
		TriState ACLK4 = NOT(nACLK2);
		TriState LCDIS;

		reg_enable_latch.set(
			MUX(ACLK1,
				MUX(W4015,
					MUX(RES, TriState::Z, TriState::Zero),
					apu->GetDBBit(bit_ena)),
				NOT(reg_enable_latch.nget())), TriState::One);
		LCDIS = reg_enable_latch.nget();

		ena_latch.set(LCDIS, ACLK1);
		cout_latch.set(carry_out, ACLK1);

		STEP = NOR(step_latch.get(), n_LFO2);
		step_latch.set(stat_ff.nget(), ACLK1);

		stat_ff.set(NOR4(AND(ena_latch.get(), ACLK4), AND(cout_latch.get(), STEP), RES, NOR(stat_ff.get(), WriteEn)));

		LC_NoCount = NOT(stat_ff.get());
		apu->SetDBBit(bit_ena, NOT(n_R4015) == TriState::One ? NOT(stat_ff.nget()) : TriState::Z);
	}

	void LengthCounter::sim_Decoder1()
	{
		TriState d[5]{};
		TriState nd[5]{};

		for (size_t n = 0; n < 5; n++)
		{
			dec_latch[n].set(apu->GetDBBit(3 + n), TriState::One);
			d[n] = dec_latch[n].get();
			nd[n] = dec_latch[n].nget();
		}

		Dec1_out[0] = NOR5(d[0], d[1], d[2], d[3], d[4]);
		Dec1_out[1] = NOR5(d[0], nd[1], d[2], d[3], d[4]);
		Dec1_out[2] = NOR5(d[0], d[1], nd[2], d[3], d[4]);
		Dec1_out[3] = NOR5(d[0], nd[1], nd[2], d[3], d[4]);
		Dec1_out[4] = NOR5(d[0], d[1], d[2], nd[3], d[4]);
		Dec1_out[5] = NOR5(d[0], nd[1], d[2], nd[3], d[4]);
		Dec1_out[6] = NOR5(d[0], d[1], nd[2], nd[3], d[4]);
		Dec1_out[7] = NOR5(d[0], nd[1], nd[2], nd[3], d[4]);

		Dec1_out[8] = NOR5(d[0], d[1], d[2], d[3], nd[4]);
		Dec1_out[9] = NOR5(d[0], nd[1], d[2], d[3], nd[4]);
		Dec1_out[10] = NOR5(d[0], d[1], nd[2], d[3], nd[4]);
		Dec1_out[11] = NOR5(d[0], nd[1], nd[2], d[3], nd[4]);
		Dec1_out[12] = NOR5(d[0], d[1], d[2], nd[3], nd[4]);
		Dec1_out[13] = NOR5(d[0], nd[1], d[2], nd[3], nd[4]);
		Dec1_out[14] = NOR5(d[0], d[1], nd[2], nd[3], nd[4]);
		Dec1_out[15] = NOR5(d[0], nd[1], nd[2], nd[3], nd[4]);

		Dec1_out[16] = NOR5(nd[0], d[1], d[2], d[3], d[4]);
		Dec1_out[17] = NOR5(nd[0], nd[1], d[2], d[3], d[4]);
		Dec1_out[18] = NOR5(nd[0], d[1], nd[2], d[3], d[4]);
		Dec1_out[19] = NOR5(nd[0], nd[1], nd[2], d[3], d[4]);
		Dec1_out[20] = NOR5(nd[0], d[1], d[2], nd[3], d[4]);
		Dec1_out[21] = NOR5(nd[0], nd[1], d[2], nd[3], d[4]);
		Dec1_out[22] = NOR5(nd[0], d[1], nd[2], nd[3], d[4]);
		Dec1_out[23] = NOR5(nd[0], nd[1], nd[2], nd[3], d[4]);

		Dec1_out[24] = NOR5(nd[0], d[1], d[2], d[3], nd[4]);
		Dec1_out[25] = NOR5(nd[0], nd[1], d[2], d[3], nd[4]);
		Dec1_out[26] = NOR5(nd[0], d[1], nd[2], d[3], nd[4]);
		Dec1_out[27] = NOR5(nd[0], nd[1], nd[2], d[3], nd[4]);
		Dec1_out[28] = NOR5(nd[0], d[1], d[2], nd[3], nd[4]);
		Dec1_out[29] = NOR5(nd[0], nd[1], d[2], nd[3], nd[4]);
		Dec1_out[30] = NOR5(nd[0], d[1], nd[2], nd[3], nd[4]);
		Dec1_out[31] = NOR5(nd[0], nd[1], nd[2], nd[3], nd[4]);
	}

	void LengthCounter::sim_Decoder2()
	{
		TriState* d = Dec1_out;

		LC[7] = NOR29(
			d[0], d[1], d[2], d[3], d[5], d[6], d[7],
			d[8], d[9], d[10], d[11], d[13], d[14], d[15],
			d[17], d[18], d[19], d[20], d[21], d[22], d[23],
			d[24], d[25], d[26], d[27], d[28], d[29], d[30], d[31]);
		LC[6] = NOR28(
			d[0], d[1], d[2], d[4], d[5], d[6], d[7],
			d[8], d[9], d[10], d[12], d[14], d[15],
			d[17], d[18], d[19], d[20], d[21], d[22], d[23],
			d[24], d[25], d[26], d[27], d[28], d[29], d[30], d[31]);
		LC[5] = NOR27(
			d[0], d[1], d[3], d[4], d[6], d[7],
			d[8], d[9], d[11], d[13], d[14], d[15],
			d[17], d[18], d[19], d[20], d[21], d[22], d[23],
			d[24], d[25], d[26], d[27], d[28], d[29], d[30], d[31]);
		LC[4] = NOR16(
			d[0], d[2], d[3], d[6],
			d[8], d[10], d[13], d[14],
			d[17], d[18], d[19], d[20], d[21], d[22], d[23],
			d[24]);
		LC[3] = NOR12(
			d[1], d[2],
			d[9], d[13],
			d[17], d[18], d[19], d[20],
			d[25], d[26], d[27], d[28]);
		LC[2] = NOR13(
			d[0], d[1], d[5], d[7],
			d[8],
			d[17], d[18], d[21], d[22],
			d[25], d[26], d[29], d[30]);
		LC[1] = NOR12(
			d[0], d[6], d[7],
			//...
			d[16], d[17], d[19], d[21], d[23],
			d[25], d[27], d[29], d[31]);
		LC[0] = TriState::One;
	}

	void LengthCounter::sim_Counter(TriState LC_CarryIn, TriState WriteEn)
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;
		TriState carry = LC_CarryIn;

		for (size_t n = 0; n < 8; n++)
		{
			carry = cnt[n].sim(carry, RES, WriteEn, STEP, ACLK1, LC[n]);
		}

		carry_out = carry;
	}

	// Envelope Unit
	// Shared between the square channels and the noise generator.

	EnvelopeUnit::EnvelopeUnit(APU* parent)
	{
		apu = parent;
	}

	EnvelopeUnit::~EnvelopeUnit()
	{
	}

	void EnvelopeUnit::sim(TriState V[4], TriState WR_Reg, TriState WR_LC)
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState n_LFO1 = apu->wire.n_LFO1;
		TriState RES = apu->wire.RES;

		// Controls

		TriState RLOAD = NOR(n_LFO1, rco_latch.get());
		TriState RSTEP = NOR(n_LFO1, rco_latch.nget());
		TriState EIN = NAND(eco_latch.get(), get_LC());
		TriState eco_reload = NOR(eco_latch.get(), reload_latch.get());
		TriState ESTEP = NOR(NOT(RLOAD), NOT(eco_reload));
		TriState ERES = NOR(NOT(RLOAD), eco_reload);

		// Reg/counters

		envdis_reg.sim(ACLK1, WR_Reg, apu->GetDBBit(4));
		lc_reg.sim(ACLK1, WR_Reg, apu->GetDBBit(5));

		TriState RCO{};
		TriState ECO{};
		RCO = TriState::One;
		ECO = TriState::One;
		for (size_t n = 0; n < 4; n++)
		{
			vol_reg[n].sim(ACLK1, WR_Reg, apu->GetDBBit(n));
			RCO = decay_cnt[n].sim(RCO, RES, RLOAD, RSTEP, ACLK1, vol_reg[n].get());
			ECO = env_cnt[n].sim(ECO, RES, ERES, ESTEP, ACLK1, EIN);
		}

		EnvReload.set(NOR(NOR(EnvReload.get(), NOR(n_LFO1, erld_latch.get())), WR_LC));
		TriState RELOAD = EnvReload.nget();

		erld_latch.set(EnvReload.get(), ACLK1);
		reload_latch.set(RELOAD, ACLK1);
		rco_latch.set(NOR(RCO, RELOAD), ACLK1);
		eco_latch.set(AND(ECO, NOT(RELOAD)), ACLK1);

		TriState ENVDIS = envdis_reg.get();
		for (size_t n = 0; n < 4; n++)
		{
			V[n] = MUX(ENVDIS, env_cnt[n].get(), vol_reg[n].get());
		}
	}

	TriState EnvelopeUnit::get_LC()
	{
		return lc_reg.nget();
	}

	// Noise Channel

	NoiseChan::NoiseChan(APU* parent)
	{
		apu = parent;
		env_unit = new EnvelopeUnit(apu);
	}

	NoiseChan::~NoiseChan()
	{
		delete env_unit;
	}

	void NoiseChan::sim()
	{
		sim_FreqReg();
		sim_Decoder1();
		sim_Decoder2();
		sim_FreqLFSR();
		sim_RandomLFSR();

		env_unit->sim(Vol, apu->wire.W400C, apu->wire.W400F);
		for (size_t n = 0; n < 4; n++)
		{
			apu->RND_Out[n] = NOR(NOT(Vol[n]), RNDOUT);
		}
	}

	void NoiseChan::sim_FreqReg()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W400E = apu->wire.W400E;
		TriState RES = apu->wire.RES;

		for (size_t n = 0; n < 4; n++)
		{
			freq_reg[n].sim(ACLK1, W400E, apu->GetDBBit(n), RES);
		}
	}

	void NoiseChan::sim_Decoder1()
	{
		TriState F[4]{};
		TriState nF[4]{};

		for (size_t n = 0; n < 4; n++)
		{
			F[n] = freq_reg[n].get();
			nF[n] = NOT(freq_reg[n].get());
		}

		sim_Decoder1_Calc(F, nF);
	}

	void NoiseChan::sim_Decoder1_Calc(BaseLogic::TriState* F, BaseLogic::TriState* nF)
	{
		Dec1_out[0] = NOR4(F[0], F[1], F[2], F[3]);
		Dec1_out[1] = NOR4(nF[0], F[1], F[2], F[3]);
		Dec1_out[2] = NOR4(F[0], nF[1], F[2], F[3]);
		Dec1_out[3] = NOR4(nF[0], nF[1], F[2], F[3]);
		Dec1_out[4] = NOR4(F[0], F[1], nF[2], F[3]);
		Dec1_out[5] = NOR4(nF[0], F[1], nF[2], F[3]);
		Dec1_out[6] = NOR4(F[0], nF[1], nF[2], F[3]);
		Dec1_out[7] = NOR4(nF[0], nF[1], nF[2], F[3]);

		Dec1_out[8] = NOR4(F[0], F[1], F[2], nF[3]);
		Dec1_out[9] = NOR4(nF[0], F[1], F[2], nF[3]);
		Dec1_out[10] = NOR4(F[0], nF[1], F[2], nF[3]);
		Dec1_out[11] = NOR4(nF[0], nF[1], F[2], nF[3]);
		Dec1_out[12] = NOR4(F[0], F[1], nF[2], nF[3]);
		Dec1_out[13] = NOR4(nF[0], F[1], nF[2], nF[3]);
		Dec1_out[14] = NOR4(F[0], nF[1], nF[2], nF[3]);
		Dec1_out[15] = NOR4(nF[0], nF[1], nF[2], nF[3]);
	}

	void NoiseChan::sim_Decoder2()
	{
		TriState* d = Dec1_out;

		NNF[0] = NOR8(d[0], d[1], d[2], d[9], d[11], d[12], d[14], d[15]);
		NNF[1] = NOR4(d[4], d[8], d[14], d[15]);
		NNF[2] = NOR12(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[8], d[11], d[12], d[14], d[15]);
		NNF[3] = NOR10(d[0], d[5], d[6], d[7], d[9], d[10], d[11], d[12], d[14], d[15]);
		NNF[4] = NOR13(d[0], d[1], d[2], d[6], d[7], d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15]);
		NNF[5] = NOR7(d[0], d[1], d[9], d[12], d[13], d[14], d[15]);
		NNF[6] = NOR10(d[0], d[1], d[2], d[3], d[4], d[8], d[9], d[10], d[13], d[14]);
		NNF[7] = NOR13(d[0], d[1], d[4], d[5], d[6], d[7], d[9], d[10], d[11], d[12], d[13], d[14], d[15]);
		NNF[8] = NOR10(d[0], d[1], d[2], d[3], d[6], d[7], d[10], d[11], d[12], d[13]);
		NNF[9] = NOR11(d[0], d[1], d[2], d[4], d[5], d[6], d[7], d[8], d[9], d[11], d[15]);
		NNF[10] = NOR15(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11], d[13], d[14], d[15]);
	}

	void NoiseChan::sim_FreqLFSR()
	{
		TriState nACLK2 = apu->wire.nACLK2;
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;
		TriState ACLK4 = NOT(nACLK2);
		TriState sout[11]{};

		for (size_t n = 0; n < 11; n++)
		{
			sout[n] = freq_lfsr[n].get_sout();
		}

		TriState NFZ = NOR11(sout[0], sout[1], sout[2], sout[3], sout[4], sout[5], sout[6], sout[7], sout[8], sout[9], sout[10]);
		TriState NFOUT = NOR11(NOT(sout[0]), sout[1], sout[2], sout[3], sout[4], sout[5], sout[6], sout[7], sout[8], sout[9], sout[10]);
		TriState step_load = NOR(NOT(NFOUT), RES);
		TriState NFLOAD = NOR(NOT(ACLK4), NOT(step_load));
		TriState NFSTEP = NOR(NOT(ACLK4), step_load);
		TriState NSIN = NOR3(AND(sout[0], sout[2]), NOR3(sout[0], sout[2], NFZ), RES);
		RSTEP = NFLOAD;

		TriState shift_in = NSIN;

		for (int n = 10; n >= 0; n--)
		{
			TriState nnf = NNF[n];
			freq_lfsr[n].sim(ACLK1, NFLOAD, NFSTEP, nnf, shift_in);
			shift_in = freq_lfsr[n].get_sout();
		}
	}

	void NoiseChan::sim_RandomLFSR()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W400E = apu->wire.W400E;
		TriState NORND = apu->wire.NORND;
		TriState LOCK = apu->wire.LOCK;
		TriState sout[15]{};

		rmod_reg.sim(ACLK1, W400E, apu->GetDBBit(7));

		for (size_t n = 0; n < 15; n++)
		{
			sout[n] = rnd_lfsr[n].get_sout();
		}

		TriState RSOZ = NOR15(sout[0], sout[1], sout[2], sout[3], sout[4], sout[5], sout[6], sout[7], sout[8], sout[9], sout[10], sout[11], sout[12], sout[13], sout[14]);
		TriState mux_out = MUX(rmod_reg.get(), sout[1], sout[6]);
		TriState RIN = NOR3(LOCK, NOR3(RSOZ, sout[0], mux_out), AND(sout[0], mux_out));
		RNDOUT = NOR(NOR(sout[0], NORND), LOCK);

		TriState shift_in = RIN;

		for (int n = 14; n >= 0; n--)
		{
			rnd_lfsr[n].sim(ACLK1, RSTEP, shift_in);
			shift_in = rnd_lfsr[n].get_sout();
		}
	}

	void FreqLFSRBit::sim(TriState ACLK1, TriState load, TriState step, TriState val, TriState shift_in)
	{
		TriState d = MUX(load, MUX(step, TriState::Z, shift_in), val);
		in_latch.set(d, TriState::One);
		out_latch.set(in_latch.nget(), ACLK1);
	}

	TriState FreqLFSRBit::get_sout()
	{
		return out_latch.nget();
	}

	void RandomLFSRBit::sim(TriState ACLK1, TriState load, TriState shift_in)
	{
		in_reg.sim(ACLK1, load, shift_in);
		out_latch.set(in_reg.nget(), ACLK1);
	}

	TriState RandomLFSRBit::get_sout()
	{
		return out_latch.nget();
	}

	TriState NoiseChan::get_LC()
	{
		return env_unit->get_LC();
	}

	// Square Channels

	SquareChan::SquareChan(APU* parent, SquareChanCarryIn carry_routing)
	{
		apu = parent;
		cin_type = carry_routing;
		env_unit = new EnvelopeUnit(apu);
	}

	SquareChan::~SquareChan()
	{
		delete env_unit;
	}

	void SquareChan::sim(TriState WR0, TriState WR1, TriState WR2, TriState WR3, TriState NOSQ, TriState* SQ_Out)
	{
		dir_reg.sim(apu->wire.ACLK1, WR1, apu->GetDBBit(3));
		DEC = dir_reg.get();
		INC = NOT(DEC);

		sim_ShiftReg(WR1);
		sim_Sweep(WR1, NOSQ);

		sim_FreqReg(WR2, WR3);
		sim_BarrelShifter();
		sim_Adder();
		sim_FreqCounter();
		sim_Duty(WR0, WR3);

		env_unit->sim(Vol, WR0, WR3);
		sim_Output(NOSQ, SQ_Out);
	}

	void SquareChan::sim_FreqReg(TriState WR2, TriState WR3)
	{
		TriState nACLK2 = apu->wire.nACLK2;
		TriState ACLK1 = apu->wire.ACLK1;

		TriState ACLK3 = NOT(nACLK2);

		for (size_t n = 0; n < 11; n++)
		{
			freq_reg[n].sim(ACLK3, ACLK1,
				n < 8 ? WR2 : WR3,
				n < 8 ? apu->GetDBBit(n) : apu->GetDBBit(n - 8), DO_SWEEP, n_sum[n]);
		}
	}

	void SquareChan::sim_ShiftReg(TriState WR1)
	{
		TriState ACLK1 = apu->wire.ACLK1;

		for (size_t n = 0; n < 3; n++)
		{
			sr_reg[n].sim(ACLK1, WR1, apu->GetDBBit(n));
			SR[n] = sr_reg[n].get();
		}
	}

	void SquareChan::sim_BarrelShifter()
	{
		TriState q1[11]{};
		TriState q2[11]{};

		for (size_t n = 0; n < 11; n++)
		{
			BS[n] = MUX(DEC, freq_reg[n].get_Fx(DO_SWEEP), freq_reg[n].get_nFx(DO_SWEEP));
		}
		BS[11] = DEC;

		for (size_t n = 0; n < 11; n++)
		{
			q1[n] = MUX(SR[0], BS[n], BS[n + 1]);
		}

		for (size_t n = 0; n < 11; n++)
		{
			q2[n] = MUX(SR[1], q1[n], n < 9 ? q1[n + 2] : BS[11]);
		}

		for (size_t n = 0; n < 11; n++)
		{
			S[n] = MUX(SR[2], q2[n], n < 7 ? q2[n + 4] : BS[11]);
		}
	}

	void SquareChan::sim_Adder()
	{
		TriState n_carry = cin_type == SquareChanCarryIn::Vdd ? TriState::One : INC;
		TriState carry = NOT(n_carry);
		TriState Fx[11]{};

		for (size_t n = 0; n < 11; n++)
		{
			Fx[n] = freq_reg[n].get_Fx(DO_SWEEP);
			adder[n].sim(Fx[n], freq_reg[n].get_nFx(DO_SWEEP),
				S[n], NOT(S[n]),
				carry, n_carry,
				carry, n_carry, n_sum[n]);
		}

		n_COUT = n_carry;
		SW_UVF = NOR9(Fx[2], Fx[3], Fx[4], Fx[5], Fx[6], Fx[7], Fx[8], Fx[9], Fx[10]);
	}

	void SquareChan::sim_FreqCounter()
	{
		TriState nACLK2 = apu->wire.nACLK2;
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;

		FLOAD = NOR(nACLK2, fco_latch.nget());
		TriState FSTEP = NOR(nACLK2, NOT(fco_latch.nget()));

		TriState carry = TriState::One;

		for (size_t n = 0; n < 11; n++)
		{
			carry = freq_cnt[n].sim(carry, RES, FLOAD, FSTEP, ACLK1, freq_reg[n].get_Fx(DO_SWEEP));
		}

		FCO = carry;
		fco_latch.set(FCO, ACLK1);
	}

	void SquareChan::sim_Sweep(TriState WR1, TriState NOSQ)
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;
		TriState n_LFO2 = apu->wire.n_LFO2;

		reload_latch.set(reload_ff.get(), ACLK1);
		reload_ff.set(NOR(NOR3(reload_ff.get(), n_LFO2, reload_latch.get()), WR1));

		swdis_reg.sim(ACLK1, WR1, apu->GetDBBit(7));
		TriState SWDIS = swdis_reg.nget();

		for (size_t n = 0; n < 3; n++)
		{
			sweep_reg[n].sim(ACLK1, WR1, apu->GetDBBit(n + 4));
		}

		TriState temp_reload = NOR(reload_latch.nget(), sco_latch.get());
		TriState SSTEP = NOR(n_LFO2, NOT(temp_reload));
		TriState SLOAD = NOR(n_LFO2, temp_reload);

		TriState SCO{};
		SCO = TriState::One;
		for (size_t n = 0; n < 3; n++)
		{
			SCO = sweep_cnt[n].sim(SCO, RES, SLOAD, SSTEP, ACLK1, sweep_reg[n].get());
		}

		sco_latch.set(SCO, ACLK1);

		SW_OVF = NOR(DEC, n_COUT);
		TriState SRZ = NOR3(SR[0], SR[1], SR[2]);
		DO_SWEEP = NOR7(SRZ, SWDIS, NOSQ, SW_OVF, sco_latch.nget(), n_LFO2, SW_UVF);
	}

	void SquareChan::sim_Duty(TriState WR0, TriState WR3)
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;
		TriState DT[3]{};

		for (size_t n = 0; n < 2; n++)
		{
			duty_reg[n].sim(ACLK1, WR0, apu->GetDBBit(n + 6));
		}

		TriState carry = FCO;
		for (size_t n = 0; n < 3; n++)
		{
			carry = duty_cnt[n].sim(carry, RES, WR3, FLOAD, ACLK1, TriState::Zero);
		}
		DT[0] = duty_cnt[0].get();
		DT[1] = duty_cnt[1].get();
		DT[2] = duty_cnt[2].get();

		TriState sel[2]{};
		sel[0] = duty_reg[0].get();
		sel[1] = duty_reg[1].get();

		TriState in[4]{};
		in[3] = NAND(DT[1], DT[2]);
		in[0] = NOR(NOT(DT[0]), in[3]);
		in[1] = NOT(in[3]);
		in[2] = DT[2];

		DUTY = MUX2(sel, in);
	}

	void SquareChan::sim_Output(TriState NOSQ, TriState* SQ_Out)
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState LOCK = apu->wire.LOCK;

		TriState d = NOR4(NOT(DUTY), SW_UVF, NOSQ, SW_OVF);
		sqo_latch.set(d, ACLK1);
		TriState sqv = NOR(sqo_latch.get(), LOCK);

		for (size_t n = 0; n < 4; n++)
		{
			SQ_Out[n] = NOR(sqv, NOT(Vol[n]));
		}
	}

	void FreqRegBit::sim(TriState ACLK3, TriState ACLK1, TriState WR, TriState DB_in, TriState ADDOUT, TriState n_sum)
	{
		TriState d = MUX(WR, MUX(ACLK3, TriState::Z, get_Fx(ADDOUT)), DB_in);
		transp_latch.set(d, TriState::One);
		sum_latch.set(n_sum, ACLK1);
	}

	TriState FreqRegBit::get_nFx(TriState ADDOUT)
	{
		return NOR(AND(sum_latch.nget(), ADDOUT), transp_latch.get());
	}

	TriState FreqRegBit::get_Fx(TriState ADDOUT)
	{
		return NOR(get_nFx(ADDOUT), AND(sum_latch.get(), ADDOUT));
	}

	TriState FreqRegBit::get()
	{
		return transp_latch.get();
	}

	void FreqRegBit::set(TriState value)
	{
		transp_latch.set(value, TriState::One);
	}

	void AdderBit::sim(TriState F, TriState nF, TriState S, TriState nS, TriState C, TriState nC,
		TriState& cout, TriState& n_cout, TriState& n_sum)
	{
		n_cout = NOR3(AND(F, S), AND3(F, nS, C), AND3(nF, S, C));
		n_sum = NOR4(AND3(F, nS, nC), AND3(nF, S, nC), AND3(nF, nS, C), AND3(F, S, C));
		cout = NOT(n_cout);
	}

	TriState SquareChan::get_LC()
	{
		return env_unit->get_LC();
	}

	// Triangle Channel

	TriangleChan::TriangleChan(APU* parent)
	{
		apu = parent;
	}

	TriangleChan::~TriangleChan()
	{
	}

	void TriangleChan::sim()
	{
		sim_Control();
		sim_LinearReg();
		sim_LinearCounter();
		sim_FreqReg();
		sim_FreqCounter();
		sim_Output();
	}

	void TriangleChan::sim_Control()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W4008 = apu->wire.W4008;
		TriState W400B = apu->wire.W400B;
		TriState n_LFO1 = apu->wire.n_LFO1;

		n_FOUT = fout_latch.nget();

		lc_reg.sim(ACLK1, W4008, apu->GetDBBit(7));

		TriState set_reload = NOR3(reload_latch1.get(), lc_reg.get(), n_LFO1);
		Reload_FF.set(NOR(NOR(Reload_FF.get(), set_reload), W400B));
		TriState TRELOAD = Reload_FF.nget();

		reload_latch1.set(Reload_FF.get(), ACLK1);
		reload_latch2.set(TRELOAD, ACLK1);
		tco_latch.set(TCO, ACLK1);

		LOAD = NOR(n_LFO1, reload_latch2.nget());
		STEP = NOR3(n_LFO1, reload_latch2.get(), tco_latch.get());
		TSTEP = NOR5(TCO, apu->wire.LOCK, apu->wire.PHI1, apu->wire.NOTRI, n_FOUT);
	}

	void TriangleChan::sim_LinearReg()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W4008 = apu->wire.W4008;

		for (size_t n = 0; n < 7; n++)
		{
			lin_reg[n].sim(ACLK1, W4008, apu->GetDBBit(n));
		}
	}

	void TriangleChan::sim_LinearCounter()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;

		TriState carry = TriState::One;

		for (size_t n = 0; n < 7; n++)
		{
			carry = lin_cnt[n].sim(carry, RES, LOAD, STEP, ACLK1, lin_reg[n].get());
		}

		TCO = carry;
	}

	void TriangleChan::sim_FreqReg()
	{
		TriState PHI1 = apu->wire.PHI1;
		TriState W400A = apu->wire.W400A;
		TriState W400B = apu->wire.W400B;

		for (size_t n = 0; n < 11; n++)
		{
			freq_reg[n].sim(PHI1, n < 8 ? W400A : W400B, n < 8 ? apu->GetDBBit(n) : apu->GetDBBit(n - 8));
		}
	}

	void TriangleChan::sim_FreqCounter()
	{
		TriState PHI1 = apu->wire.PHI1;
		TriState RES = apu->wire.RES;

		TriState carry = TriState::One;
		TriState FLOAD = NOR(PHI1, n_FOUT);
		TriState FSTEP = NOR(PHI1, NOT(n_FOUT));

		for (size_t n = 0; n < 11; n++)
		{
			carry = freq_cnt[n].sim(carry, RES, FLOAD, FSTEP, PHI1, freq_reg[n].get());
		}

		fout_latch.set(carry, PHI1);
	}

	void TriangleChan::sim_Output()
	{
		TriState PHI1 = apu->wire.PHI1;
		TriState RES = apu->wire.RES;
		TriState W401A = apu->wire.W401A;
		TriState T[5]{};
		TriState nT[5]{};

		TriState carry{};

		// The developers decided to use PHI1 for the triangle channel instead of ACLK to smooth out the "stepped" signal.

		carry = TriState::One;
		for (size_t n = 0; n < 5; n++)
		{
			carry = out_cnt[n].sim(carry, RES, W401A, TSTEP, PHI1 /* !!! */, apu->GetDBBit(n));
		}

		TriState T4 = out_cnt[4].get();

		for (size_t n = 0; n < 4; n++)
		{
			T[n] = out_cnt[n].get();
			nT[n] = out_cnt[n].nget();
			apu->TRI_Out[n] = NOT(MUX(T4, T[n], nT[n]));
		}
	}

	TriState TriangleChan::get_LC()
	{
		return lc_reg.nget();
	}

	// Register Decoder

	RegsDecoder::RegsDecoder(APU* parent)
	{
		apu = parent;
	}

	RegsDecoder::~RegsDecoder()
	{
	}

	void RegsDecoder::sim()
	{
		sim_Predecode();
		sim_Decoder();
		sim_RegOps();
		sim_DebugLock();
	}

	void RegsDecoder::sim_Predecode()
	{
		TriState CpuA[16]{};

		for (size_t n = 0; n < 16; n++)
		{
			CpuA[n] = ((apu->CPU_Addr >> n) & 1) != 0 ? TriState::One : TriState::Zero;
		}

		auto REGWR = NOR12(CpuA[5], CpuA[6], CpuA[7], CpuA[8], CpuA[9], CpuA[10], CpuA[11], CpuA[12], CpuA[13], NOT(CpuA[14]), CpuA[15], apu->wire.RnW);
		nREGWR = NOT(REGWR);

		auto REGRD = NOR12(CpuA[5], CpuA[6], CpuA[7], CpuA[8], CpuA[9], CpuA[10], CpuA[11], CpuA[12], CpuA[13], NOT(CpuA[14]), CpuA[15], NOT(apu->wire.RnW));
		nREGRD = NOT(REGRD);

		apu->wire.n_DBGRD = NAND(apu->wire.DBG, NOT(nREGRD));
	}

	void RegsDecoder::sim_Decoder()
	{
		TriState A[5]{};
		TriState nA[5]{};

		// Decoder

		for (size_t n = 0; n < 5; n++)
		{
			A[n] = ((apu->Ax >> n) & 1) != 0 ? TriState::One : TriState::Zero;
		}

		for (size_t n = 0; n < 5; n++)
		{
			nA[n] = NOT(A[n]);
		}

		pla[0] = NOR6(nREGRD, A[0], A[1], A[2], nA[3], nA[4]);
		pla[1] = NOR6(nREGRD, nA[0], A[1], A[2], nA[3], nA[4]);
		pla[2] = NOR6(nREGRD, A[0], nA[1], A[2], nA[3], nA[4]);
		pla[3] = NOR6(nREGWR, A[0], nA[1], A[2], nA[3], nA[4]);

		pla[4] = NOR6(nREGRD, nA[0], A[1], nA[2], A[3], nA[4]);
		pla[5] = NOR6(nREGWR, nA[0], nA[1], A[2], A[3], A[4]);
		pla[6] = NOR6(nREGWR, A[0], nA[1], A[2], A[3], A[4]);
		pla[7] = NOR6(nREGWR, nA[0], nA[1], nA[2], A[3], A[4]);
		pla[8] = NOR6(nREGWR, nA[0], A[1], A[2], A[3], A[4]);

		pla[9] = NOR6(nREGWR, A[0], A[1], nA[2], A[3], A[4]);
		pla[10] = NOR6(nREGWR, nA[0], A[1], nA[2], A[3], A[4]);
		pla[11] = NOR6(nREGWR, A[0], A[1], nA[2], nA[3], A[4]);
		pla[12] = NOR6(nREGWR, A[0], nA[1], nA[2], A[3], A[4]);
		pla[13] = NOR6(nREGWR, A[0], A[1], A[2], A[3], A[4]);

		pla[14] = NOR6(nREGWR, A[0], A[1], A[2], nA[3], A[4]);
		pla[15] = NOR6(nREGWR, nA[0], A[1], nA[2], A[3], nA[4]);
		pla[16] = NOR6(nREGWR, A[0], nA[1], A[2], nA[3], A[4]);
		pla[17] = NOR6(nREGWR, nA[0], A[1], A[2], A[3], nA[4]);
		pla[18] = NOR6(nREGWR, nA[0], nA[1], A[2], nA[3], A[4]);

		pla[19] = NOR6(nREGWR, nA[0], nA[1], nA[2], nA[3], A[4]);
		pla[20] = NOR6(nREGWR, A[0], nA[1], nA[2], nA[3], A[4]);
		pla[21] = NOR6(nREGRD, nA[0], nA[1], nA[2], A[3], nA[4]);
		pla[22] = NOR6(nREGWR, nA[0], nA[1], A[2], A[3], nA[4]);
		pla[23] = NOR6(nREGRD, A[0], nA[1], nA[2], A[3], nA[4]);

		pla[24] = NOR6(nREGWR, A[0], nA[1], A[2], A[3], nA[4]);
		pla[25] = NOR6(nREGWR, A[0], nA[1], nA[2], A[3], nA[4]);
		pla[26] = NOR6(nREGWR, A[0], A[1], A[2], A[3], nA[4]);
		pla[27] = NOR6(nREGWR, nA[0], nA[1], nA[2], A[3], nA[4]);
		pla[28] = NOR6(nREGWR, A[0], A[1], nA[2], A[3], nA[4]);
	}

	void RegsDecoder::sim_RegOps()
	{
		TriState PHI1 = apu->wire.PHI1;
		TriState DBG = apu->wire.DBG;

		// Select a register index.
		// Note that during PHI1 all write operations are disabled.

		apu->wire.n_R4018 = NAND(DBG, pla[0]);
		apu->wire.n_R401A = NAND(DBG, pla[2]);

		apu->wire.n_R4015 = NOT(pla[4]);
		apu->wire.W4002 = NOR(PHI1, NOT(pla[6]));
		apu->wire.W4001 = NOR(PHI1, NOT(pla[8]));
		apu->wire.W4005 = NOR(PHI1, NOT(pla[10]));
		apu->wire.W4006 = NOR(PHI1, NOT(pla[12]));
		apu->wire.W4008 = NOR(PHI1, NOT(pla[14]));
		apu->wire.W400A = NOR(PHI1, NOT(pla[16]));
		apu->wire.W400B = NOR(PHI1, NOT(pla[18]));
		apu->wire.W400E = NOR(PHI1, NOT(pla[20]));
		apu->wire.W4013 = NOR(PHI1, NOT(pla[22]));
		apu->wire.W4012 = NOR(PHI1, NOT(pla[24]));
		apu->wire.W4010 = NOR(PHI1, NOT(pla[26]));
		apu->wire.W4014 = NOR(PHI1, NOT(pla[28]));

		apu->wire.n_R4019 = NAND(DBG, pla[1]);
		apu->wire.W401A = NOR(PHI1, NAND(DBG, pla[3]));

		apu->wire.W4003 = NOR(PHI1, NOT(pla[5]));
		apu->wire.W4007 = NOR(PHI1, NOT(pla[7]));
		apu->wire.W4004 = NOR(PHI1, NOT(pla[9]));
		apu->wire.W400C = NOR(PHI1, NOT(pla[11]));
		apu->wire.W4000 = NOR(PHI1, NOT(pla[13]));
		apu->wire.W4015 = NOR(PHI1, NOT(pla[15]));
		apu->wire.W4011 = NOR(PHI1, NOT(pla[17]));
		apu->wire.W400F = NOR(PHI1, NOT(pla[19]));
		apu->wire.n_R4017 = NOT(pla[21]);
		apu->wire.n_R4016 = NOT(pla[23]);
		apu->wire.W4016 = NOR(PHI1, NOT(pla[25]));
		apu->wire.W4017 = NOR(PHI1, NOT(pla[27]));
	}

	void RegsDecoder::sim_DebugLock()
	{
		TriState ACLK1 = apu->wire.ACLK1;
		TriState RES = apu->wire.RES;
		TriState W401A = apu->wire.W401A;

		TriState d = MUX(W401A, MUX(ACLK1, TriState::Z, NOR(lock_latch.nget(), RES)), apu->GetDBBit(7));
		lock_latch.set(d, TriState::One);
		apu->wire.LOCK = NOR(lock_latch.nget(), RES);
	}

	void RegsDecoder::sim_DebugRegisters()
	{
		if (apu->wire.DBG == TriState::Zero)
		{
			return;
		}

		for (size_t n = 0; n < 4; n++)
		{
			apu->SetDBBit(n, NOT(apu->wire.n_R4018) ? apu->SQA_Out[n] : TriState::Z);
			apu->SetDBBit(n + 4, NOT(apu->wire.n_R4018) ? apu->SQB_Out[n] : TriState::Z);
			apu->SetDBBit(n, NOT(apu->wire.n_R4019) ? apu->TRI_Out[n] : TriState::Z);
			apu->SetDBBit(n + 4, NOT(apu->wire.n_R4019) ? apu->RND_Out[n] : TriState::Z);
		}

		for (size_t n = 0; n < 7; n++)
		{
			apu->SetDBBit(n, NOT(apu->wire.n_R401A) ? apu->DMC_Out[n] : TriState::Z);
		}
	}

	// Simulation of APU chip terminals and everything related to them.

	Pads::Pads(APU* parent)
	{
		apu = parent;
	}

	Pads::~Pads()
	{
	}

	void Pads::sim_InputPads(TriState inputs[])
	{
		apu->wire.n_CLK = NOT(inputs[(size_t)APU_Input::CLK]);
		apu->wire.DBG = inputs[(size_t)APU_Input::DBG];
		apu->wire.RES = NOT(inputs[(size_t)APU_Input::n_RES]);

		// In the original #NMI and #IRQ terminals overuse the BIDIR terminal circuit.
		// In order to avoid all sorts of gimmicks, let's just do a pass-through.

		apu->wire.n_NMI = inputs[(size_t)APU_Input::n_NMI];
		//n_nmi.sim(inputs[(size_t)APU_Input::n_NMI], TriState::One,
		//	apu->wire.n_NMI, unused, TriState::One, TriState::Zero);

		apu->wire.n_IRQ = inputs[(size_t)APU_Input::n_IRQ];
		//n_irq.sim(inputs[(size_t)APU_Input::n_IRQ], TriState::One,
		//	apu->wire.n_IRQ, unused, TriState::One, TriState::Zero);
	}

	void Pads::sim_OutputPads(TriState outputs[], uint16_t* addr)
	{
		TriState NotDBG_RES = NOR(apu->wire.DBG, NOT(apu->wire.RES));
		outputs[(size_t)APU_Output::M2] = NOT(NotDBG_RES) == TriState::One ? NOR(apu->wire.n_M2, NotDBG_RES) : TriState::Z;

		if (apu->wire.RES == TriState::Zero)
		{
			*addr = apu->Ax;
		}

		outputs[(size_t)APU_Output::RnW] = NOT(apu->wire.RES) == TriState::One ? NOT(NOR(apu->wire.RW, apu->wire.RES)) : TriState::Z;

		// I/O

		sim_OutReg();

		n_in[0].sim(unused, apu->wire.n_R4016, unused, outputs[(size_t)APU_Output::n_IN0], apu->wire.RES, TriState::One);
		n_in[1].sim(unused, apu->wire.n_R4017, unused, outputs[(size_t)APU_Output::n_IN1], apu->wire.RES, TriState::One);

		out[0].sim(unused, OUT_Signal[0], unused, outputs[(size_t)APU_Output::OUT_0], apu->wire.RES, TriState::One);
		out[1].sim(unused, OUT_Signal[1], unused, outputs[(size_t)APU_Output::OUT_1], apu->wire.RES, TriState::One);
		out[2].sim(unused, OUT_Signal[2], unused, outputs[(size_t)APU_Output::OUT_2], apu->wire.RES, TriState::One);
	}

	/// <summary>
	/// External data bus -> Internal data bus
	/// </summary>
	/// <param name="data">External data bus</param>
	void Pads::sim_DataBusInput(uint8_t* data)
	{
		TriState val[8]{};

		if (apu->wire.RD != TriState::One)
		{
			apu->DB_Dirty = false;
			return;
		}

		for (size_t n = 0; n < 8; n++)
		{
			TriState db_bit = ((*data) >> n) & 1 ? TriState::One : TriState::Zero;
			data_bus[n].sim_Input(db_bit, val[n], apu->wire.RD);
		}

		apu->DB = Pack(val);
		apu->DB_Dirty = true;
	}

	/// <summary>
	/// Internal data bus -> External data bus
	/// </summary>
	/// <param name="data">External data bus</param>
	void Pads::sim_DataBusOutput(uint8_t* data)
	{
		TriState val[8]{};

		if (apu->wire.WR != TriState::One)
		{
			return;
		}

		for (size_t n = 0; n < 8; n++)
		{
			data_bus[n].sim_Output(apu->GetDBBit(n), val[n], apu->wire.WR);
			;
		}

		*data = Pack(val);
	}

	void Pads::sim_OutReg()
	{
		TriState nACLK2 = apu->wire.nACLK2;
		TriState ACLK1 = apu->wire.ACLK1;
		TriState W4016 = apu->wire.W4016;
		TriState ACLK5 = NOT(nACLK2);

		for (size_t n = 0; n < 3; n++)
		{
			out_reg[n].sim(ACLK1, W4016, apu->GetDBBit(n));
			out_latch[n].set(out_reg[n].get(), ACLK5);
			OUT_Signal[n] = NOT(out_latch[n].nget());
		}
	}

	void BIDIR::sim(TriState pad_in, TriState to_pad,
		TriState& from_pad, TriState& pad_out,
		TriState rd, TriState wr)
	{
		in_latch.set(pad_in, TriState::One);
		out_latch.set(to_pad, TriState::One);

		if (NOT(wr) == TriState::One)
		{
			from_pad = NOT(in_latch.nget());
		}
		else
		{
			from_pad = TriState::Z;
		}

		if (NOT(rd) == TriState::One)
		{
			pad_out = NOT(NOR(out_latch.get(), rd));
		}
		else
		{
			pad_out = TriState::Z;
		}
	}

	void BIDIR::sim_Input(TriState pad_in, TriState& from_pad, TriState rd)
	{
		in_latch.set(pad_in, TriState::One);

		if (rd == TriState::One)
		{
			from_pad = NOT(in_latch.nget());
		}
	}

	void BIDIR::sim_Output(TriState to_pad, TriState& pad_out, TriState wr)
	{
		out_latch.set(to_pad, TriState::One);

		if (wr == TriState::One)
		{
			pad_out = out_latch.get();
		}
	}

	TriState BIDIR::get_in()
	{
		return in_latch.get();
	}

	TriState BIDIR::get_out()
	{
		return out_latch.get();
	}

	void BIDIR::set_in(TriState val)
	{
		in_latch.set(val, TriState::One);
	}

	void BIDIR::set_out(TriState val)
	{
		out_latch.set(val, TriState::One);
	}

	// Obtaining the analog value of the AUX A/B signals from the digital outputs of the generators.

	DAC::DAC(APU* parent)
	{
		apu = parent;
		gen_DACTabs();
	}

	DAC::~DAC()
	{
	}

	void DAC::sim(AudioOutSignal& AUX)
	{
		sim_DACA(AUX);
		sim_DACB(AUX);
	}

	void DAC::sim_DACA(AudioOutSignal& aout)
	{
		if (raw_mode)
		{
			aout.RAW.sqa = PackNibble(apu->SQA_Out);
			aout.RAW.sqb = PackNibble(apu->SQB_Out);
		}
		else
		{
			size_t v = ((size_t)PackNibble(apu->SQB_Out) << 4) | PackNibble(apu->SQA_Out);
			if (norm_mode)
			{
				aout.normalized.a = auxa_norm[v];
			}
			else
			{
				aout.AUX.a = auxa_mv[v];
			}
		}
	}

	void DAC::sim_DACB(AudioOutSignal& aout)
	{
		if (raw_mode)
		{
			aout.RAW.tri = PackNibble(apu->TRI_Out);
			aout.RAW.rnd = PackNibble(apu->RND_Out);
			aout.RAW.dmc = Pack(apu->DMC_Out);
		}
		else
		{
			size_t v = ((size_t)Pack(apu->DMC_Out) << 8) | ((size_t)PackNibble(apu->RND_Out) << 4) | PackNibble(apu->TRI_Out);
			if (norm_mode)
			{
				aout.normalized.b = auxb_norm[v];
			}
			else
			{
				aout.AUX.b = auxb_mv[v];
			}
		}
	}

	void DAC::SetRAWOutput(bool enable)
	{
		raw_mode = enable;
	}

	void DAC::SetNormalizedOutput(bool enable)
	{
		norm_mode = enable;
	}

	void DAC::gen_DACTabs()
	{
		// AUX A

		float maxv = 0.f;

		for (size_t sqb = 0; sqb < 16; sqb++)
		{
			for (size_t sqa = 0; sqa < 16; sqa++)
			{
				float r = AUX_A_Resistance(sqa, sqb);
				float i = Vdd / (r + ExtRes);
				float v = i * ExtRes * 1000.f;
				if (v > maxv)
				{
					maxv = v;
				}
				auxa_mv[(sqb << 4) | sqa] = v;
			}
		}

		for (size_t sqb = 0; sqb < 16; sqb++)
		{
			for (size_t sqa = 0; sqa < 16; sqa++)
			{
				float r = AUX_A_Resistance(sqa, sqb);
				float i = Vdd / (r + ExtRes);
				float v = i * ExtRes * 1000.f;
				auxa_norm[(sqb << 4) | sqa] = v / maxv;
			}
		}

		// AUX B

		maxv = 0.f;

		for (size_t dmc = 0; dmc < 128; dmc++)
		{
			for (size_t rnd = 0; rnd < 16; rnd++)
			{
				for (size_t tri = 0; tri < 16; tri++)
				{
					float r = AUX_B_Resistance(tri, rnd, dmc);
					float i = Vdd / (r + ExtRes);
					float v = i * ExtRes * 1000.f;
					if (v > maxv)
					{
						maxv = v;
					}
					auxb_mv[(dmc << 8) | (rnd << 4) | tri] = v;
				}
			}
		}

		for (size_t dmc = 0; dmc < 128; dmc++)
		{
			for (size_t rnd = 0; rnd < 16; rnd++)
			{
				for (size_t tri = 0; tri < 16; tri++)
				{
					float r = AUX_B_Resistance(tri, rnd, dmc);
					float i = Vdd / (r + ExtRes);
					float v = i * ExtRes * 1000.f;
					auxb_norm[(dmc << 8) | (rnd << 4) | tri] = v / maxv;
				}
			}
		}
	}

	/// <summary>
	/// Return the internal resistance of the AUX A terminal based on the SQA/SQB digital inputs.
	/// </summary>
	/// <param name="sqa"></param>
	/// <param name="sqb"></param>
	/// <returns></returns>
	float DAC::AUX_A_Resistance(size_t sqa, size_t sqb)
	{
		bool sqa_r_exists[] = { false, false, false, false };
		bool sqb_r_exists[] = { false, false, false, false };
		static float stage_reciprocal[] = { 1.f, 2.f, 4.f, 8.f };
		bool any_exists = false;

		// First determine which resistances are present

		for (size_t n = 0; n < 4; n++)
		{
			if ((sqa >> n) & 1)
			{
				sqa_r_exists[n] = true;
				any_exists = true;
			}
			if ((sqb >> n) & 1)
			{
				sqb_r_exists[n] = true;
				any_exists = true;
			}
		}

		// Calculate the total resistance

		float r = 0.f;

		for (size_t n = 0; n < 4; n++)
		{
			if (sqa_r_exists[n])
				r += 1.f / (IntRes / stage_reciprocal[n]);
			if (sqb_r_exists[n])
				r += 1.f / (IntRes / stage_reciprocal[n]);
		}

		return any_exists ? (1.f / r) : IntUnloaded;
	}

	/// <summary>
	/// Return the internal resistance of the AUX B terminal based on the TRI/RND/DMC digital inputs.
	/// </summary>
	/// <param name="tri"></param>
	/// <param name="rnd"></param>
	/// <param name="dmc"></param>
	/// <returns></returns>
	float DAC::AUX_B_Resistance(size_t tri, size_t rnd, size_t dmc)
	{
		bool tri_r_exists[] = { false, false, false, false };
		bool rnd_r_exists[] = { false, false, false, false };
		bool dmc_r_exists[] = { false, false, false, false, false, false, false };
		static float tri_reciprocal[] = { 2.f, 4.f, 8.f, 16.f };
		static float rnd_reciprocal[] = { 1.f, 2.f, 4.f, 8.f };
		static float dmc_reciprocal[] = { 0.5f, 1.f, 2.f, 4.f, 8.f, 16.f, 32.f };
		bool any_exists = false;

		// First determine which resistances are present

		for (size_t n = 0; n < 4; n++)
		{
			if ((tri >> n) & 1)
			{
				tri_r_exists[n] = true;
				any_exists = true;
			}
			if ((rnd >> n) & 1)
			{
				rnd_r_exists[n] = true;
				any_exists = true;
			}
		}
		for (size_t n = 0; n < 7; n++)
		{
			if ((dmc >> n) & 1)
			{
				dmc_r_exists[n] = true;
				any_exists = true;
			}
		}

		// Calculate the total resistance

		float r = 0.f;

		for (size_t n = 0; n < 4; n++)
		{
			if (tri_r_exists[n])
				r += 1.f / (IntRes / tri_reciprocal[n]);
			if (rnd_r_exists[n])
				r += 1.f / (IntRes / rnd_reciprocal[n]);
		}
		for (size_t n = 0; n < 7; n++)
		{
			if (dmc_r_exists[n])
				r += 1.f / (IntRes / dmc_reciprocal[n]);
		}

		return any_exists ? (1.f / r) : IntUnloaded;
	}

	// APU Main

	APU::APU(M6502Core::M6502* _core, Revision _rev, ChipFeature features)
	{
		// For ease of integration, the core instance is created by the consumer
		core = _core;
		rev = _rev;
		fx = features;

		core_int = new CoreBinding(this);
		clkgen = new CLKGen(this);
		lc[0] = new LengthCounter(this);
		lc[1] = new LengthCounter(this);
		lc[2] = new LengthCounter(this);
		lc[3] = new LengthCounter(this);
		dpcm = new DpcmChan(this);
		noise = new NoiseChan(this);
		square[0] = new SquareChan(this, SquareChanCarryIn::Vdd);
		square[1] = new SquareChan(this, SquareChanCarryIn::Inc);
		tri = new TriangleChan(this);
		regs = new RegsDecoder(this);
		dma = new DMA(this);
		pads = new Pads(this);
		dac = new DAC(this);

		wire.RDY2 = TriState::One;
	}

	APU::~APU()
	{
		delete core_int;
		delete clkgen;
		delete lc[0];
		delete lc[1];
		delete lc[2];
		delete lc[3];
		delete dpcm;
		delete noise;
		delete square[0];
		delete square[1];
		delete tri;
		delete regs;
		delete dma;
		delete pads;
		delete dac;
	}

	void APU::sim(TriState inputs[], TriState outputs[], uint8_t* data, uint16_t* addr, AudioOutSignal& AUX)
	{
		pads->sim_InputPads(inputs);
		pads->sim_DataBusInput(data);

		dpcm->sim();

		dma->sim();
		dma->sim_DMA_Buffer();
		dma->sim_AddressMux();

		sim_CoreIntegration();

		sim_SoundGenerators();

		pads->sim_OutputPads(outputs, addr);
		pads->sim_DataBusOutput(data);
		dac->sim(AUX);
	}

	void APU::sim_CoreIntegration()
	{
		// Core & stuff

		core_int->sim();
		clkgen->sim();
		regs->sim();
		regs->sim_DebugRegisters();
	}

	void APU::sim_SoundGenerators()
	{
		// The audio generator circuits are discrete to PHI (Core clock), there is no point in simulating them every CLK

		if (wire.PHI0 == PrevPHI_SoundGen) {
			return;
		}
		PrevPHI_SoundGen = wire.PHI0;

		// Sound channels

		wire.SQA_LC = square[0]->get_LC();
		wire.SQB_LC = square[1]->get_LC();
		wire.TRI_LC = tri->get_LC();
		wire.RND_LC = noise->get_LC();

		lc[0]->sim(0, wire.W4003, wire.SQA_LC, wire.NOSQA);
		lc[1]->sim(1, wire.W4007, wire.SQB_LC, wire.NOSQB);
		lc[2]->sim(2, wire.W400B, wire.TRI_LC, wire.NOTRI);
		lc[3]->sim(3, wire.W400F, wire.RND_LC, wire.NORND);

		square[0]->sim(wire.W4000, wire.W4001, wire.W4002, wire.W4003, wire.NOSQA, SQA_Out);
		square[1]->sim(wire.W4004, wire.W4005, wire.W4006, wire.W4007, wire.NOSQB, SQB_Out);
		tri->sim();
		noise->sim();
	}

	TriState APU::GetDBBit(size_t n)
	{
		TriState DBBit = (DB & (1 << n)) != 0 ? TriState::One : TriState::Zero;
		return DBBit;
	}

	void APU::SetDBBit(size_t n, TriState bit_val)
	{
		if (bit_val != TriState::Z)
		{
			uint8_t out = DB & ~(1 << n);
			out |= (bit_val == BaseLogic::One ? 1 : 0) << n;
			DB = out;
		}
	}

	void APU::SetRAWOutput(bool enable)
	{
		dac->SetRAWOutput(enable);
	}

	void APU::SetNormalizedOutput(bool enable)
	{
		dac->SetNormalizedOutput(enable);
	}

	size_t APU::GetACLKCounter()
	{
		return aclk_counter;
	}

	void APU::ResetACLKCounter()
	{
		aclk_counter = 0;
	}

	size_t APU::GetPHICounter()
	{
		return phi_counter;
	}

	void APU::ResetPHICounter()
	{
		phi_counter = 0;
	}

	void APU::GetSignalFeatures(AudioSignalFeatures& features)
	{
		size_t clk = 21477272;	// Hz
		size_t div = 12;

		features.SampleRate = (int32_t)(clk * 2);

		// TBD: Add other APU
	}

	TriState APU::GetPHI2()
	{
		return wire.PHI2;
	}
}