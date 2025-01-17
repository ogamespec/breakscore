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

namespace M6502Core
{
	class M6502;

	union DecoderInput
	{
		struct
		{
			unsigned n_T1X : 1;		// lsb (bit0)
			unsigned n_T0 : 1;
			unsigned n_IR5 : 1;
			unsigned IR5 : 1;
			unsigned n_IR6 : 1;
			unsigned IR6 : 1;
			unsigned n_IR2 : 1;
			unsigned IR2 : 1;
			unsigned n_IR3 : 1;
			unsigned IR3 : 1;
			unsigned n_IR4 : 1;
			unsigned IR4 : 1;
			unsigned n_IR7 : 1;
			unsigned IR7 : 1;
			unsigned n_IR0 : 1;
			unsigned IR01 : 1;
			unsigned n_IR1 : 1;
			unsigned n_T2 : 1;
			unsigned n_T3 : 1;
			unsigned n_T4 : 1;
			unsigned n_T5 : 1;		// msb
		};
		size_t packed_bits;
	};

	class Decoder
	{
		BaseLogic::PLA* pla = nullptr;

	public:
		static const size_t inputs_count = 21;
		static const size_t outputs_count = 130;

		Decoder();
		~Decoder();

		void sim(size_t input_bits, BaseLogic::TriState** outputs);
	};

	class IR
	{
		uint8_t ir_latch = 0;

		M6502* core = nullptr;

	public:

		IR(M6502* parent) { core = parent; }

		uint8_t IROut = 0;

		void sim();
	};

	class PreDecode
	{
		uint8_t pd_latch = 0;

		M6502* core = nullptr;

		BaseLogic::TriState precalc_n_TWOCYCLE[0x100];
		BaseLogic::TriState precalc_n_IMPLIED[0x100];

	public:

		PreDecode(M6502* parent);

		uint8_t PD = 0;
		uint8_t n_PD = 0xff;

		void sim(uint8_t* data_bus);
	};

	class ExtraCounter
	{
		BaseLogic::DLatch t1_latch{};
		BaseLogic::DLatch t2_latch1{};
		BaseLogic::DLatch t2_latch2{};
		BaseLogic::DLatch t3_latch1{};
		BaseLogic::DLatch t3_latch2{};
		BaseLogic::DLatch t4_latch1{};
		BaseLogic::DLatch t4_latch2{};
		BaseLogic::DLatch t5_latch1{};
		BaseLogic::DLatch t5_latch2{};

		M6502* core = nullptr;

		// HLE
		uint8_t latch1 = 0;
		uint8_t latch2 = 0;

	public:

		ExtraCounter(M6502* parent) { core = parent; }

		void sim();

		void sim_HLE();
	};

	class BRKProcessing
	{
		BaseLogic::DLatch brk5_latch{};
		BaseLogic::DLatch brk6_latch1{};
		BaseLogic::DLatch brk6_latch2{};
		BaseLogic::DLatch res_latch1{};
		BaseLogic::DLatch res_latch2{};
		BaseLogic::DLatch brk6e_latch{};
		BaseLogic::DLatch brk7_latch{};
		BaseLogic::DLatch nmip_latch{};
		BaseLogic::DLatch donmi_latch{};
		BaseLogic::DLatch ff1_latch{};
		BaseLogic::DLatch ff2_latch{};
		BaseLogic::DLatch delay_latch1{};
		BaseLogic::DLatch delay_latch2{};
		BaseLogic::DLatch b_latch1{};
		BaseLogic::DLatch b_latch2{};

		BaseLogic::DLatch zadl_latch[3]{};

		M6502* core = nullptr;

	public:

		BRKProcessing(M6502* parent) { core = parent; }

		void sim_BeforeRandom();
		void sim_AfterRandom();

		BaseLogic::TriState getDORES();
		BaseLogic::TriState getB_OUT(BaseLogic::TriState BRK6E);
		BaseLogic::TriState getn_BRK6_LATCH2();
	};

	class Flags
	{
		// There is no point in making FFs, the DLatch outputs will be their role.

		BaseLogic::DLatch z_latch1{};
		BaseLogic::DLatch z_latch2{};
		BaseLogic::DLatch n_latch1{};
		BaseLogic::DLatch n_latch2{};
		BaseLogic::DLatch c_latch1{};
		BaseLogic::DLatch c_latch2{};
		BaseLogic::DLatch d_latch1{};
		BaseLogic::DLatch d_latch2{};
		BaseLogic::DLatch i_latch1{};
		BaseLogic::DLatch i_latch2{};
		BaseLogic::DLatch v_latch1{};
		BaseLogic::DLatch v_latch2{};
		BaseLogic::DLatch avr_latch{};
		BaseLogic::DLatch so_latch1{};
		BaseLogic::DLatch so_latch2{};
		BaseLogic::DLatch so_latch3{};
		BaseLogic::DLatch vset_latch{};

		M6502* core = nullptr;

	public:

		Flags(M6502* parent) { core = parent; }

		void sim_Load();

		void sim_Store();

		BaseLogic::TriState getn_Z_OUT();
		BaseLogic::TriState getn_N_OUT();
		BaseLogic::TriState getn_C_OUT();
		BaseLogic::TriState getn_D_OUT();
		BaseLogic::TriState getn_I_OUT(BaseLogic::TriState BRK6E);
		BaseLogic::TriState getn_V_OUT();

		void set_Z_OUT(BaseLogic::TriState val);
		void set_N_OUT(BaseLogic::TriState val);
		void set_C_OUT(BaseLogic::TriState val);
		void set_D_OUT(BaseLogic::TriState val);
		void set_I_OUT(BaseLogic::TriState val);
		void set_V_OUT(BaseLogic::TriState val);
	};

	union RegsControl_TempWire
	{
		struct
		{
			unsigned n_Y_SB : 1;
			unsigned n_X_SB : 1;
			unsigned n_SB_X : 1;
			unsigned n_SB_Y : 1;
			unsigned n_SB_S : 1;
			unsigned n_S_ADL : 1;
		};
		uint8_t bits;
	};

	class RegsControl
	{
		BaseLogic::DLatch nready_latch{};

		BaseLogic::DLatch ysb_latch{};
		BaseLogic::DLatch xsb_latch{};
		BaseLogic::DLatch ssb_latch{};
		BaseLogic::DLatch sbx_latch{};
		BaseLogic::DLatch sby_latch{};
		BaseLogic::DLatch sbs_latch{};
		BaseLogic::DLatch ss_latch{};
		BaseLogic::DLatch sadl_latch{};

		M6502* core = nullptr;

		RegsControl_TempWire temp_tab[0x10000]{};
		RegsControl_TempWire prev_temp;

		RegsControl_TempWire PreCalc(uint8_t ir, bool n_T0, bool n_T1X, bool n_T2, bool n_T3, bool n_T4, bool n_T5, bool n_ready, bool n_ready_latch);

	public:

		RegsControl(M6502* parent);

		void sim();
	};

	union CarryBCD_TempWire
	{
		struct
		{
			unsigned n_ADL_ADD : 1;
			unsigned n_ADL_ADD_Derived : 1;
			unsigned INC_SB : 1;
			unsigned BRX : 1;
			unsigned CSET : 1;
		};
		uint8_t bits;
	};

	class ALUControl
	{
		BaseLogic::DLatch acin_latch1{};
		BaseLogic::DLatch acin_latch2{};
		BaseLogic::DLatch acin_latch3{};
		BaseLogic::DLatch acin_latch4{};
		BaseLogic::DLatch acin_latch5{};

		BaseLogic::DLatch ndbadd_latch{};
		BaseLogic::DLatch dbadd_latch{};
		BaseLogic::DLatch zadd_latch{};
		BaseLogic::DLatch sbadd_latch{};
		BaseLogic::DLatch adladd_latch{};

		BaseLogic::DLatch ands_latch1{};
		BaseLogic::DLatch ands_latch2{};
		BaseLogic::DLatch eors_latch1{};
		BaseLogic::DLatch eors_latch2{};
		BaseLogic::DLatch ors_latch1{};
		BaseLogic::DLatch ors_latch2{};
		BaseLogic::DLatch srs_latch1{};
		BaseLogic::DLatch srs_latch2{};
		BaseLogic::DLatch sums_latch1{};
		BaseLogic::DLatch sums_latch2{};

		BaseLogic::DLatch addsb7_latch{};
		BaseLogic::DLatch addsb06_latch{};
		BaseLogic::DLatch addadl_latch{};

		BaseLogic::DLatch daa_latch1{};
		BaseLogic::DLatch daa_latch2{};
		BaseLogic::DLatch dsa_latch1{};
		BaseLogic::DLatch dsa_latch2{};

		// ADD/SB7 latches

		BaseLogic::DLatch cout_latch{};
		BaseLogic::DLatch nready_latch{};
		BaseLogic::DLatch mux_latch1{};
		BaseLogic::DLatch ff_latch1{};
		BaseLogic::DLatch ff_latch2{};
		BaseLogic::DLatch sr_latch1{};
		BaseLogic::DLatch sr_latch2{};

		M6502* core = nullptr;

		BaseLogic::TriState STKOP = BaseLogic::TriState::Zero;
		BaseLogic::TriState n_ADL_ADD = BaseLogic::TriState::Zero;
		BaseLogic::TriState INC_SB = BaseLogic::TriState::Zero;
		BaseLogic::TriState BRX = BaseLogic::TriState::Zero;
		BaseLogic::TriState n_ADD_SB7 = BaseLogic::TriState::Zero;

		CarryBCD_TempWire temp_tab1[1 << 19];
		CarryBCD_TempWire prev_temp1;

		CarryBCD_TempWire PreCalc1(uint8_t ir, bool n_T0, bool n_T1X, bool n_T2, bool n_T3, bool n_T4, bool n_T5,
			bool n_ready, bool T0, bool T5, bool BRFW, bool n_C_OUT);

	public:

		ALUControl(M6502* parent);

		void sim_CarryBCD();
		void sim_ALUInput();
		void sim_ALUOps();
		void sim_ADDOut();

		void sim();
	};

	class BusControl
	{
		BaseLogic::DLatch z_adh0_latch{};
		BaseLogic::DLatch z_adh17_latch{};
		BaseLogic::DLatch sb_ac_latch{};
		BaseLogic::DLatch adl_abl_latch{};
		BaseLogic::DLatch ac_sb_latch{};
		BaseLogic::DLatch sb_db_latch{};
		BaseLogic::DLatch ac_db_latch{};
		BaseLogic::DLatch sb_adh_latch{};
		BaseLogic::DLatch adh_abh_latch{};
		BaseLogic::DLatch dl_adh_latch{};
		BaseLogic::DLatch dl_adl_latch{};
		BaseLogic::DLatch dl_db_latch{};
		BaseLogic::DLatch nready_latch{};

		M6502* core = nullptr;

	public:

		BusControl(M6502* parent) { core = parent; }

		void sim();
	};

	class PC_Control
	{
		BaseLogic::DLatch pcl_db_latch1{};
		BaseLogic::DLatch pcl_db_latch2{};
		BaseLogic::DLatch pch_db_latch1{};
		BaseLogic::DLatch pch_db_latch2{};
		BaseLogic::DLatch nready_latch{};
		BaseLogic::DLatch pcl_adl_latch{};
		BaseLogic::DLatch pch_adh_latch{};
		BaseLogic::DLatch pcl_pcl_latch{};
		BaseLogic::DLatch adl_pcl_latch{};
		BaseLogic::DLatch adh_pch_latch{};
		BaseLogic::DLatch pch_pch_latch{};

		M6502* core = nullptr;

	public:

		PC_Control(M6502* parent) { core = parent; }

		void sim();
	};

	class Dispatcher
	{
		BaseLogic::DLatch acr_latch1{};
		BaseLogic::DLatch acr_latch2{};

		BaseLogic::DLatch t67_latch{};
		BaseLogic::DLatch t6_latch1{};
		BaseLogic::DLatch t6_latch2{};
		BaseLogic::DLatch t7_latch1{};
		BaseLogic::DLatch t7_latch2{};

		BaseLogic::DLatch tres2_latch{};
		BaseLogic::DLatch tresx_latch1{};
		BaseLogic::DLatch tresx_latch2{};

		BaseLogic::DLatch fetch_latch{};
		BaseLogic::DLatch wr_latch{};
		BaseLogic::DLatch ready_latch1{};
		BaseLogic::DLatch ready_latch2{};

		BaseLogic::DLatch ends_latch1{};
		BaseLogic::DLatch ends_latch2{};

		BaseLogic::DLatch nready_latch{};
		BaseLogic::DLatch step_latch1{};
		BaseLogic::DLatch step_latch2{};
		BaseLogic::DLatch t1_latch{};

		BaseLogic::DLatch comp_latch1{};
		BaseLogic::DLatch comp_latch2{};
		BaseLogic::DLatch comp_latch3{};

		BaseLogic::DLatch rdydelay_latch1{};
		BaseLogic::DLatch rdydelay_latch2{};

		BaseLogic::DLatch t0_latch{};
		BaseLogic::DLatch t1x_latch{};

		BaseLogic::DLatch br_latch1{};
		BaseLogic::DLatch br_latch2{};
		BaseLogic::DLatch ipc_latch1{};
		BaseLogic::DLatch ipc_latch2{};
		BaseLogic::DLatch ipc_latch3{};

		M6502* core = nullptr;

	public:

		Dispatcher(M6502* parent) { core = parent; }

		void sim_BeforeDecoder();

		void sim_BeforeRandomLogic();

		void sim_AfterRandomLogic();

		BaseLogic::TriState getTRES2();

		BaseLogic::TriState getT1();

		BaseLogic::TriState getSTOR(BaseLogic::TriState d[]);
	};

	union FlagsControl_TempWire
	{
		struct
		{
			unsigned n_POUT : 1;
			unsigned n_ARIT : 1;
			unsigned n_PIN : 1;
			unsigned ZTST : 1;
			unsigned SR : 1;
		};
		uint8_t bits;
	};

	class FlagsControl
	{
		BaseLogic::DLatch pdb_latch{};
		BaseLogic::DLatch iri_latch{};
		BaseLogic::DLatch irc_latch{};
		BaseLogic::DLatch ird_latch{};
		BaseLogic::DLatch zv_latch{};
		BaseLogic::DLatch acrc_latch{};
		BaseLogic::DLatch dbz_latch{};
		BaseLogic::DLatch dbn_latch{};
		BaseLogic::DLatch dbc_latch{};
		BaseLogic::DLatch pin_latch{};
		BaseLogic::DLatch bit_latch{};

		M6502* core = nullptr;

		FlagsControl_TempWire temp_tab[0x10000]{};
		RegsControl_TempWire prev_temp;

		FlagsControl_TempWire PreCalc(uint8_t ir, bool n_T0, bool n_T1X, bool n_T2, bool n_T3, bool n_T4, bool n_T5, bool T5, bool T6);

	public:

		FlagsControl(M6502* parent);

		void sim();
	};

	class BranchLogic
	{
		BaseLogic::DLatch br2_latch{};
		BaseLogic::DLatch brfw_latch1{};
		BaseLogic::DLatch brfw_latch2{};

		M6502* core = nullptr;

	public:

		BranchLogic(M6502* parent) { core = parent; }

		void sim();

		BaseLogic::TriState getBRFW();
	};

	class RandomLogic
	{
		M6502* core = nullptr;

	public:
		RegsControl* regs_control = nullptr;
		ALUControl* alu_control = nullptr;
		PC_Control* pc_control = nullptr;
		BusControl* bus_control = nullptr;
		FlagsControl* flags_control = nullptr;
		Flags* flags = nullptr;
		BranchLogic* branch_logic = nullptr;

		RandomLogic(M6502* parent);
		~RandomLogic();

		void sim();
	};

	class AddressBus
	{
		uint8_t ABL = 0;
		uint8_t ABH = 0;

		M6502* core = nullptr;

	public:

		AddressBus(M6502* parent) { core = parent; }

		void sim_ConstGen();

		void sim_Output(uint16_t* addr_bus);

		uint8_t getABL();
		uint8_t getABH();

		void setABL(uint8_t val);
		void setABH(uint8_t val);
	};

	class Regs
	{
		uint8_t Y = 0;
		uint8_t X = 0;
		uint8_t S_in = 0;
		uint8_t S_out = 0;

		M6502* core = nullptr;

	public:

		Regs(M6502* parent) { core = parent; }

		void sim_LoadSB();

		void sim_StoreSB();

		void sim_StoreOldS();

		uint8_t getY();
		uint8_t getX();
		uint8_t getS();

		void setY(uint8_t val);
		void setX(uint8_t val);
		void setS(uint8_t val);
	};

	class ALU
	{
		uint8_t AI = 0;
		uint8_t BI = 0;
		uint8_t n_ADD = 0xff;
		uint8_t AC = 0;

		BaseLogic::DLatch BC7_latch{};
		BaseLogic::DLatch DC7_latch{};

		BaseLogic::DLatch daal_latch{};
		BaseLogic::DLatch daah_latch{};
		BaseLogic::DLatch dsal_latch{};
		BaseLogic::DLatch dsah_latch{};

		BaseLogic::DLatch DCLatch{};
		BaseLogic::DLatch ACLatch{};
		BaseLogic::DLatch AVRLatch{};

		bool BCD_Hack = false;		// BCD correction hack for NES/Famicom.

		M6502* core = nullptr;

	public:

		ALU(M6502* parent) { core = parent; }

		void sim_Load();

		void sim();

		void sim_HLE();

		void sim_StoreADD();

		void sim_StoreAC();

		void sim_BusMux();

		BaseLogic::TriState getACR();

		BaseLogic::TriState getAVR();
		void setAVR(BaseLogic::TriState);

		uint8_t getAI();
		uint8_t getBI();
		uint8_t getADD();
		uint8_t getAC();

		void setAI(uint8_t val);
		void setBI(uint8_t val);
		void setADD(uint8_t val);
		void setAC(uint8_t val);

		void SetBCDHack(bool enable) { BCD_Hack = enable; }
	};

	class ProgramCounter
	{
		BaseLogic::DLatch PCL[8]{};
		BaseLogic::DLatch PCLS[8]{};
		BaseLogic::DLatch PCH[8]{};
		BaseLogic::DLatch PCHS[8]{};

		uint8_t PackedPCL;
		uint8_t PackedPCLS;
		uint8_t PackedPCH;
		uint8_t PackedPCHS;

		void sim_EvenBit(BaseLogic::TriState PHI2, BaseLogic::TriState cin, BaseLogic::TriState& cout, BaseLogic::TriState& sout, size_t n, BaseLogic::DLatch PCx[], BaseLogic::DLatch PCxS[]);

		void sim_OddBit(BaseLogic::TriState PHI2, BaseLogic::TriState cin, BaseLogic::TriState& cout, BaseLogic::TriState& sout, size_t n, BaseLogic::DLatch PCx[], BaseLogic::DLatch PCxS[]);

		M6502* core = nullptr;

		bool HLE = false;

	public:

		ProgramCounter(M6502* parent, bool hle)
		{
			core = parent;
			HLE = hle;
		}

		void sim_Load();
		void sim_LoadHLE();

		void sim_Store();
		void sim_StoreHLE();

		void sim();
		void sim_HLE();

		uint8_t getPCL();
		uint8_t getPCH();
		uint8_t getPCLS();
		uint8_t getPCHS();

		void setPCL(uint8_t val);
		void setPCH(uint8_t val);
		void setPCLS(uint8_t val);
		void setPCHS(uint8_t val);
	};

	class DataBus
	{
		BaseLogic::DLatch rd_latch{};
		uint8_t DL = 0;
		uint8_t DOR = 0;

		M6502* core = nullptr;

	public:

		DataBus(M6502* parent) { core = parent; }

		void sim_SetExternalBus(uint8_t* data_bus);

		void sim_GetExternalBus(uint8_t* data_bus);

		uint8_t getDL();
		uint8_t getDOR();

		void setDL(uint8_t val);
		void setDOR(uint8_t val);
	};

	enum class InputPad
	{
		n_NMI = 0,
		n_IRQ,
		n_RES,
		PHI0,
		RDY,
		SO,
		Max,
	};

	enum class OutputPad
	{
		PHI1 = 0,
		PHI2,
		RnW,
		SYNC,
		Max,
	};

	class M6502
	{
		friend IR;
		friend PreDecode;
		friend ExtraCounter;
		friend BRKProcessing;
		friend Dispatcher;
		friend RandomLogic;
		friend RegsControl;
		friend ALUControl;
		friend BusControl;
		friend PC_Control;
		friend FlagsControl;
		friend BranchLogic;
		friend AddressBus;
		friend Regs;
		friend Flags;
		friend ALU;
		friend ProgramCounter;
		friend DataBus;

		BaseLogic::FF nmip_ff;
		BaseLogic::FF irqp_ff;
		BaseLogic::FF resp_ff;
		BaseLogic::DLatch irqp_latch;
		BaseLogic::DLatch resp_latch;

		BaseLogic::DLatch prdy_latch1;
		BaseLogic::DLatch prdy_latch2;

		BaseLogic::DLatch rw_latch;

		uint8_t SB = 0;
		uint8_t DB = 0;
		uint8_t ADL = 0;
		uint8_t ADH = 0;

		bool SB_Dirty = false;
		bool DB_Dirty = false;
		bool ADL_Dirty = false;
		bool ADH_Dirty = false;

		Decoder* decoder = nullptr;
		PreDecode* predecode = nullptr;
		IR* ir = nullptr;
		ExtraCounter* ext = nullptr;
		BRKProcessing* brk = nullptr;
		Dispatcher* disp = nullptr;
		RandomLogic* random = nullptr;

		AddressBus* addr_bus = nullptr;
		Regs* regs = nullptr;
		ALU* alu = nullptr;
		ProgramCounter* pc = nullptr;
		DataBus* data_bus = nullptr;

		BaseLogic::TriState* decoder_out = nullptr;
		size_t TxBits = 0;		// Used to optimize table indexing

		void sim_Top(BaseLogic::TriState inputs[], uint8_t* data_bus);

		void sim_Bottom(BaseLogic::TriState inputs[], BaseLogic::TriState outputs[], uint16_t* addr_bus, uint8_t* data_bus);

		BaseLogic::TriState nNMI_Cache = BaseLogic::TriState::Z;
		BaseLogic::TriState nIRQ_Cache = BaseLogic::TriState::Z;
		BaseLogic::TriState nRES_Cache = BaseLogic::TriState::Z;

		bool HLE_Mode = false;		// Acceleration mode for fast applications. In this case we are cheating a little bit.

		/// <summary>
		/// Internal auxiliary and intermediate connections.
		/// </summary>
		struct InternalWires
		{
			BaseLogic::TriState n_NMI;
			BaseLogic::TriState n_IRQ;
			BaseLogic::TriState n_RES;
			BaseLogic::TriState PHI0;
			BaseLogic::TriState RDY;
			BaseLogic::TriState SO;
			BaseLogic::TriState PHI1;
			BaseLogic::TriState PHI2;
			BaseLogic::TriState n_PRDY;
			BaseLogic::TriState n_NMIP;
			BaseLogic::TriState n_IRQP;
			BaseLogic::TriState RESP;
			BaseLogic::TriState n_ready;		// Global internal processor readiness signal
			BaseLogic::TriState T0;				// Processor in the T0 instruction execution cycle
			BaseLogic::TriState FETCH;
			BaseLogic::TriState Z_IR;			// "injects" the BRK operation into the instruction stream
			BaseLogic::TriState ACRL1;
			BaseLogic::TriState ACRL2;
			BaseLogic::TriState n_T0;
			BaseLogic::TriState n_T1X;
			BaseLogic::TriState WR;
			BaseLogic::TriState RMW_T6;			// Processor in cycle RMW T6 (the name T5 is the old name of the signal)
			BaseLogic::TriState RMW_T7;			// Processor in cycle RMW T7 (the name T6 is the old name of the signal)
			BaseLogic::TriState n_1PC;			// 0: Increment the program counter
			BaseLogic::TriState ENDS;
			BaseLogic::TriState ENDX;
			BaseLogic::TriState TRES1;
			BaseLogic::TriState n_TRESX;
			BaseLogic::TriState n_T2;
			BaseLogic::TriState n_T3;
			BaseLogic::TriState n_T4;
			BaseLogic::TriState n_T5;
			BaseLogic::TriState n_IMPLIED;		// 0: An instruction of type IMPLIED which has no operands (takes 1 byte in memory)
			BaseLogic::TriState n_TWOCYCLE;		// 0: A short instruction which is executed in 2 clock cycles
			BaseLogic::TriState BRK6E;
			BaseLogic::TriState BRK7;
			BaseLogic::TriState DORES;
			BaseLogic::TriState n_DONMI;
			BaseLogic::TriState BRK5_RDY;
			BaseLogic::TriState B_OUT;
			BaseLogic::TriState BRFW;			// Branch forward (whenever taken)
			BaseLogic::TriState n_BRTAKEN;		// 0: Branch taken
			BaseLogic::TriState PC_DB;			// Auxiliary output signal for the RW Control circuit that is part of the dispatcher
			BaseLogic::TriState n_ADL_PCL;
			BaseLogic::TriState n_IR5;			// /IR5 goes additionally to flags and is used in set/clear flags instructions (SEI/CLI, SED/CLD, SEC/CLC)
		} wire{};

		/// <summary>
		/// Commands for bottom and flags.
		/// </summary>
		union RandomLogic_Output
		{
			struct {
				unsigned Y_SB : 1;			// Y => SB
				unsigned SB_Y : 1;			// SB => Y
				unsigned X_SB : 1;			// X => SB
				unsigned SB_X : 1;			// SB => X
				unsigned S_ADL : 1;			// S => ADL
				unsigned S_SB : 1;			// S => SB
				unsigned SB_S : 1;			// SB => S
				unsigned S_S : 1;			// The S/S command is active if the SB/S command is inactive. This command simply "refreshes" the current state of the S register.
				unsigned NDB_ADD : 1;		// ~DB => BI
				unsigned DB_ADD : 1;		// DB => BI
				unsigned Z_ADD : 1;			// 0 => AI
				unsigned SB_ADD : 1;		// SB => AI
				unsigned ADL_ADD : 1;		// ADL => BI
				unsigned n_ACIN : 1;		// ALU input carry. The ALU also returns the result of carry (ACR) and overflow (AVR)
				unsigned ANDS : 1;			// AI & BI
				unsigned EORS : 1;			// AI ^ BI
				unsigned ORS : 1;			// AI | BI
				unsigned SRS : 1;			// >>= 1
				unsigned SUMS : 1;			// AI + BI
				unsigned n_DAA : 1;			// 0: Perform BCD correction after addition
				unsigned n_DSA : 1;			// 0: Perform BCD correction after subtraction
				unsigned ADD_SB7 : 1;		// ADD[7] => SB[7]
				unsigned ADD_SB06 : 1;		// ADD[0-6] => SB[0-6]
				unsigned ADD_ADL : 1;		// ADD => ADL
				unsigned SB_AC : 1;			// Place the value from the SB bus/BCD correction circuit into the accumulator
				unsigned AC_SB : 1;			// Place the AC value on the SB bus
				unsigned AC_DB : 1;			// Place the AC value on the DB bus
				unsigned ADH_PCH : 1;		// ADH => PCH
				unsigned PCH_PCH : 1;		// If ADH/PCH is not performed, this command is performed (refresh PCH)
				unsigned PCH_ADH : 1;		// PCH => ADH
				unsigned PCH_DB : 1;		// PCH => DB
				unsigned ADL_PCL : 1;		// ADL => PCL
				unsigned PCL_PCL : 1;		// If ADL/PCL is not performed, this command is performed (refresh PCL)
				unsigned PCL_ADL : 1;		// PCL => ADL
				unsigned PCL_DB : 1;		// PCL => DB
				unsigned ADH_ABH : 1;		// Set the high 8 bits of the external address bus, in accordance with the value of the internal bus ADH
				unsigned ADL_ABL : 1;		// Set the low-order 8 bits of the external address bus, in accordance with the value of the internal bus ADL
				unsigned Z_ADL0 : 1;		// Forced to clear the ADL[0] bit
				unsigned Z_ADL1 : 1;		// Forced to clear the ADL[1] bit
				unsigned Z_ADL2 : 1;		// Forced to clear the ADL[2] bit
				unsigned Z_ADH0 : 1;		// Forced to clear the ADH[0] bit
				unsigned Z_ADH17 : 1;		// Forced to clear the ADH[1-7] bits
				unsigned SB_DB : 1;			// Connect the SB and DB buses
				unsigned SB_ADH : 1;		// Connect SB and ADH buses
				unsigned DL_ADL : 1;		// Write the DL value to the ADL bus
				unsigned DL_ADH : 1;		// Write the DL value to the ADH bus
				unsigned DL_DB : 1;			// Exchange the value of the DL and the internal bus DB. The direction of the exchange depends on the operating mode of the external data bus (read/write)

				unsigned P_DB : 1;			// Place the value of the flags register P on the DB bus
				unsigned DB_P : 1;			// Place the DB bus value on the flag register P
				unsigned DBZ_Z : 1;			// Change the value of the flag according to the /DBZ value
				unsigned DB_N : 1;			// Change the value of the flag according to DB7 bit
				unsigned IR5_C : 1;			// Change the value of the flag according to the IR5 bit
				unsigned DB_C : 1;			// Change the value of the flag according to DB0 bit
				unsigned ACR_C : 1;			// Change the value of the flag according to the ACR value
				unsigned IR5_D : 1;			// Change the value of the flag according to the IR5 bit
				unsigned IR5_I : 1;			// Change the value of the flag according to the IR5 bit
				unsigned DB_V : 1;			// Change the value of the flag according to DB6 bit
				unsigned AVR_V : 1;			// Change the value of the flag according to the AVR value
				unsigned Z_V : 1;			// Clear flag V
			};
			uint64_t raw;
		} cmd{};

	public:
		M6502() {}
		M6502(bool HLE, bool BCD_Hack);
		~M6502();

		virtual void sim(BaseLogic::TriState inputs[], BaseLogic::TriState outputs[], uint16_t* addr_bus, uint8_t* data_bus);
	};
}