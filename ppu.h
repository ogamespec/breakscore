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

namespace PPUSim
{
	class PPU;

#pragma pack(push, 1)

	/// <summary>
	/// A software descriptor of the current video sample. Due to the different PPU revisions the output value may differ in content, so union is used.
	/// </summary>
	union VideoOutSignal
	{
		/// <summary>
		/// Sample for "composite" PPUs
		/// </summary>
		float composite;	// For NTSC/PAL variations of PPU. A sample of the composite video signal.

		/// <summary>
		/// Sample for RGB PPU
		/// </summary>
		struct RGBOut
		{
			uint8_t RED;
			uint8_t GREEN;
			uint8_t BLUE;
			uint8_t nSYNC;	// This field is reserved for the "/SYNC" output of the RGB PPU (Sync Level).
		} RGB;

		/// <summary>
		/// Raw PPU color, which is obtained from the PPU circuits BEFORE the video signal generator.
		/// The user can switch the PPUSim video output to use "raw" color, instead of the original (Composite/RGB).
		/// </summary>
		union RAWOut
		{
			struct
			{
				unsigned CC0 : 1;	// Chroma (CB[0-3])
				unsigned CC1 : 1;
				unsigned CC2 : 1;
				unsigned CC3 : 1;
				unsigned LL0 : 1;	// Luma (CB[4-5])
				unsigned LL1 : 1;
				unsigned TR : 1;	// "Tint Red", $2001[5]
				unsigned TG : 1;	// "Tint Green", $2001[6]
				unsigned TB : 1;	// "Tint Blue", $2001[7]
				unsigned Sync : 1;	// 1: Sync level
			};
			uint16_t raw;
		} RAW;
	};

	/// <summary>
	/// The structure describes all the features of the signal and helps organize its rendering.
	/// </summary>
	struct VideoSignalFeatures
	{
		int32_t SamplesPerPCLK;
		int32_t PixelsPerScan;		// Excluding Dot Crawl
		int32_t ScansPerField;
		int32_t BackPorchSize;		// BackPorch size in pixels.
		int32_t Composite;			// 1: Composite, 0: RGB
		float BlackLevel;			// IRE = 0
		float WhiteLevel;			// IRE = 110
		float SyncLevel;			// SYNC low level
		int32_t PhaseAlteration;	// 1: PAL
	};

	/// <summary>
	/// How to handle the OAM Decay effect.
	/// </summary>
	enum class OAMDecayBehavior
	{
		Keep = 0,
		Evaporate,
		ToZero,
		ToOne,
		Randomize,
	};

#pragma pack(pop)

	// Background Color (BG COL)

	class BGC_SRBit
	{
		BaseLogic::DLatch in_latch;
		BaseLogic::DLatch out_latch;

	public:
		void sim(BaseLogic::TriState shift_in, BaseLogic::TriState val_in,
			BaseLogic::TriState Load, BaseLogic::TriState Step, BaseLogic::TriState Next,
			BaseLogic::TriState& shift_out);
	};

	class BGC_SR8
	{
		BGC_SRBit sr[8]{};

	public:
		void sim(BaseLogic::TriState val[8], BaseLogic::TriState sin,
			BaseLogic::TriState Load, BaseLogic::TriState Step, BaseLogic::TriState Next,
			BaseLogic::TriState sout[8]);
	};

	class BGCol
	{
		PPU* ppu = nullptr;

		BaseLogic::DLatch fat_latch;
		BaseLogic::DLatch tho1_latch;

		BaseLogic::DLatch clpb_latch;
		BaseLogic::DLatch bgc0_latch[2];
		BaseLogic::DLatch bgc1_latch[2];
		BaseLogic::DLatch bgc2_latch[2];
		BaseLogic::DLatch bgc3_latch[2];

		BaseLogic::TriState PD_SR = BaseLogic::TriState::X;
		BaseLogic::TriState SRLOAD = BaseLogic::TriState::X;
		BaseLogic::TriState STEP = BaseLogic::TriState::X;
		BaseLogic::TriState STEP2 = BaseLogic::TriState::X;
		BaseLogic::TriState PD_SEL = BaseLogic::TriState::X;
		BaseLogic::TriState NEXT = BaseLogic::TriState::X;
		BaseLogic::TriState H01 = BaseLogic::TriState::X;

		BaseLogic::DLatch BGC0_Latch[8]{};
		BGC_SR8 BGC0_SR1;
		BGC_SR8 BGC0_SR2;

		BGC_SR8 BGC1_SR1;
		BGC_SR8 BGC1_SR2;

		BaseLogic::DLatch pd_latch[8]{};

		BGC_SRBit BGC2_SRBit1;
		BGC_SR8 BGC2_SR1;

		BGC_SRBit BGC3_SRBit1;
		BGC_SR8 BGC3_SR1;

		BaseLogic::TriState n_BGC0_Out = BaseLogic::TriState::X;
		BaseLogic::TriState BGC1_Out = BaseLogic::TriState::X;
		BaseLogic::TriState n_BGC2_Out = BaseLogic::TriState::X;
		BaseLogic::TriState n_BGC3_Out = BaseLogic::TriState::X;

		BaseLogic::TriState unused[8]{};

		void sim_Control();
		void sim_BGC0();
		void sim_BGC1();
		void sim_BGC2();
		void sim_BGC3();
		void sim_Output();

	public:
		BGCol(PPU* parent);
		~BGCol();

		void sim();
	};

	// Color Generator RAM (Palette)

	class CBBit
	{
	protected:
		PPU* ppu = nullptr;

		BaseLogic::FF ff;
		BaseLogic::DLatch latch1;
		BaseLogic::DLatch latch2;

	public:
		CBBit(PPU* parent) { ppu = parent; }
		~CBBit() {}

		virtual void sim(size_t bit_num, BaseLogic::TriState* cell, BaseLogic::TriState n_OE);

		BaseLogic::TriState get_CBOut(BaseLogic::TriState n_OE);
	};

	/// <summary>
	/// Special version for RGB PPU.
	/// </summary>
	class CBBit_RGB : public CBBit
	{
	public:
		CBBit_RGB(PPU* parent) : CBBit(parent) {}
		~CBBit_RGB() {}

		virtual void sim(size_t bit_num, BaseLogic::TriState* cell, BaseLogic::TriState n_OE);
	};

	class CRAM
	{
		PPU* ppu = nullptr;

		BaseLogic::DLatch dbpar_latch;

		BaseLogic::DLatch LL0_latch[3];
		BaseLogic::DLatch LL1_latch[3];
		BaseLogic::DLatch CC_latch[4];

		static const size_t cb_num = 6;
		CBBit* cb[cb_num];

		void sim_CRAMControl();
		void sim_CRAMDecoder();
		void sim_ColorBuffer();

		/// <summary>
		/// The organization of CRAM is very intricate. Rows = 7 (Rows 0+4 are combined). Columns = 4.
		/// One lane is not a byte, but a `6-bit` (corresponds to the number of bits of the Color Buffer).
		/// </summary>
		static const size_t cram_lane_rows = 7;
		static const size_t cram_lane_cols = 4;
		BaseLogic::TriState cram[cram_lane_rows * cram_lane_cols][cb_num]{};

		BaseLogic::TriState COL[4]{};
		BaseLogic::TriState ROW[8]{};	// ROW[0] = ROW[4]

		/// <summary>
		/// This cell is used to address closed memory.
		/// </summary>
		BaseLogic::TriState z_cell = BaseLogic::TriState::Z;

		BaseLogic::TriState* AddressCell(size_t bit_num);

		size_t MapRow(size_t rowNum);

	public:
		CRAM(PPU* parent);
		~CRAM();

		void sim();

		uint8_t Dbg_CRAMReadByte(size_t addr);

		void Dbg_CRAMWriteByte(size_t addr, uint8_t val);
	};

	// Object FIFO (Motion picture buffer memory)

	class FIFO_CounterBit
	{
		BaseLogic::FF keep_ff;
		BaseLogic::DLatch step_latch;

	public:
		BaseLogic::TriState sim(BaseLogic::TriState Clock, BaseLogic::TriState Load, BaseLogic::TriState Step,
			BaseLogic::TriState val_in,
			BaseLogic::TriState carry_in,
			BaseLogic::TriState& val_out);
		BaseLogic::TriState get();
	};

	class FIFO_SRBit
	{
		BaseLogic::DLatch in_latch;
		BaseLogic::DLatch out_latch;

	public:
		BaseLogic::TriState sim(BaseLogic::TriState n_PCLK, BaseLogic::TriState T_SR, BaseLogic::TriState SR_EN,
			BaseLogic::TriState nTx, BaseLogic::TriState shift_in);
	};

	struct FIFOLaneOutput
	{
		BaseLogic::TriState nZ_COL0;
		BaseLogic::TriState nZ_COL1;
		BaseLogic::TriState Z_COL2;
		BaseLogic::TriState Z_COL3;
		BaseLogic::TriState nZ_PRIO;
		BaseLogic::TriState n_xEN;
	};

	class FIFOLane
	{
		PPU* ppu = nullptr;

		FIFO_SRBit paired_sr[2][8]{};
		FIFO_CounterBit down_cnt[8]{};

		BaseLogic::DLatch ob0_latch[2];
		BaseLogic::DLatch ob1_latch[2];
		BaseLogic::DLatch ob5_latch[2];
		BaseLogic::DLatch hsel_latch;

		BaseLogic::TriState nZ_COL0 = BaseLogic::TriState::X;
		BaseLogic::TriState nZ_COL1 = BaseLogic::TriState::X;
		BaseLogic::TriState Z_COL2 = BaseLogic::TriState::X;
		BaseLogic::TriState Z_COL3 = BaseLogic::TriState::X;
		BaseLogic::TriState nZ_PRIO = BaseLogic::TriState::X;
		BaseLogic::TriState SR_EN = BaseLogic::TriState::X;
		BaseLogic::TriState LDAT = BaseLogic::TriState::X;
		BaseLogic::TriState LOAD = BaseLogic::TriState::X;
		BaseLogic::TriState T_SR0 = BaseLogic::TriState::X;
		BaseLogic::TriState T_SR1 = BaseLogic::TriState::X;

		BaseLogic::FF ZH_FF;
		BaseLogic::DLatch en_latch;

		BaseLogic::TriState UPD = BaseLogic::TriState::X;
		BaseLogic::TriState STEP = BaseLogic::TriState::X;
		BaseLogic::TriState n_EN = BaseLogic::TriState::X;

		void sim_LaneControl(BaseLogic::TriState HSel);
		void sim_CounterControl();
		BaseLogic::TriState sim_Counter();
		void sim_CounterCarry(BaseLogic::TriState Carry);
		void sim_PairedSREnable();
		void sim_PairedSR(BaseLogic::TriState n_TX[8], uint8_t packed_nTX);

		size_t get_Counter();

	public:
		FIFOLane(PPU* parent);
		~FIFOLane();

		void sim(BaseLogic::TriState HSel, BaseLogic::TriState n_TX[8], uint8_t packed_nTX, FIFOLaneOutput& ZOut);
	};

	class FIFO
	{
		PPU* ppu = nullptr;

		FIFOLane* lane[8];

		BaseLogic::DLatch zh_latch1;
		BaseLogic::DLatch zh_latch2;
		BaseLogic::DLatch zh_latch3;

		BaseLogic::FF HINV_FF;
		BaseLogic::DLatch tout_latch[8];
		BaseLogic::TriState n_TX[8]{};
		uint8_t packed_nTX = 0;

		BaseLogic::DLatch sh2_latch;
		BaseLogic::DLatch sh3_latch;
		BaseLogic::DLatch sh5_latch;
		BaseLogic::DLatch sh7_latch;

		BaseLogic::DLatch s0_latch;
		BaseLogic::DLatch col2_latch;
		BaseLogic::DLatch col3_latch;
		BaseLogic::DLatch prio_latch;

		FIFOLaneOutput LaneOut[8]{};

		void sim_HInv();
		void sim_Lanes();
		void sim_Prio();

		BaseLogic::TriState get_nZCOL0(size_t lane);
		BaseLogic::TriState get_nZCOL1(size_t lane);
		BaseLogic::TriState get_nxEN(size_t lane);

	public:
		FIFO(PPU* parent);
		~FIFO();

		void sim();

		/// <summary>
		/// You can call right after the FSM.
		/// </summary>
		void sim_SpriteH();
	};

	// PPU FSM

	class FSM
	{
		PPU* ppu = nullptr;

		BaseLogic::DLatch h_latch1[6];
		BaseLogic::DLatch h_latch2[6];

		BaseLogic::DLatch fp_latch1;
		BaseLogic::DLatch fp_latch2;
		BaseLogic::DLatch sev_latch1;
		BaseLogic::DLatch sev_latch2;
		BaseLogic::DLatch clip_latch1;
		BaseLogic::DLatch clip_latch2;
		BaseLogic::DLatch clpo_latch;
		BaseLogic::DLatch clpb_latch;
		BaseLogic::DLatch hpos_latch1;
		BaseLogic::DLatch hpos_latch2;
		BaseLogic::DLatch eval_latch1;
		BaseLogic::DLatch eval_latch2;
		BaseLogic::DLatch eev_latch1;
		BaseLogic::DLatch eev_latch2;
		BaseLogic::DLatch ioam_latch1;
		BaseLogic::DLatch ioam_latch2;
		BaseLogic::DLatch objrd_latch1;
		BaseLogic::DLatch objrd_latch2;
		BaseLogic::DLatch nvis_latch1;
		BaseLogic::DLatch nvis_latch2;
		BaseLogic::DLatch fnt_latch1;
		BaseLogic::DLatch fnt_latch2;
		BaseLogic::DLatch ftb_latch1;
		BaseLogic::DLatch ftb_latch2;
		BaseLogic::DLatch fta_latch1;
		BaseLogic::DLatch fta_latch2;
		BaseLogic::DLatch fo_latch1;
		BaseLogic::DLatch fo_latch2;
		BaseLogic::DLatch fo_latch3;
		BaseLogic::DLatch fat_latch1;
		BaseLogic::DLatch bp_latch1;
		BaseLogic::DLatch bp_latch2;
		BaseLogic::DLatch hb_latch1;
		BaseLogic::DLatch hb_latch2;
		BaseLogic::DLatch cb_latch1;
		BaseLogic::DLatch cb_latch2;
		BaseLogic::DLatch sync_latch1;
		BaseLogic::DLatch sync_latch2;
		BaseLogic::FF FPORCH_FF;
		BaseLogic::FF BPORCH_FF;
		BaseLogic::FF HBLANK_FF;
		BaseLogic::FF BURST_FF;

		BaseLogic::DLatch vsync_latch1;
		BaseLogic::DLatch pic_latch1;
		BaseLogic::DLatch pic_latch2;
		BaseLogic::DLatch vset_latch1;
		BaseLogic::DLatch vb_latch1;
		BaseLogic::DLatch vb_latch2;
		BaseLogic::DLatch blnk_latch1;
		BaseLogic::DLatch vclr_latch1;
		BaseLogic::DLatch vclr_latch2;
		BaseLogic::FF VSYNC_FF;
		BaseLogic::FF PICTURE_FF;
		BaseLogic::FF VB_FF;
		BaseLogic::FF BLNK_FF;

		BaseLogic::DLatch edge_vset_latch1;
		BaseLogic::DLatch edge_vset_latch2;
		BaseLogic::DLatch db_latch;
		BaseLogic::FF INT_FF;

		BaseLogic::FF EvenOdd_FF1;
		BaseLogic::FF EvenOdd_FF2;

		// PAL PPU only.
		BaseLogic::DLatch EvenOdd_latch1;
		BaseLogic::DLatch EvenOdd_latch2;
		BaseLogic::DLatch EvenOdd_latch3;

		BaseLogic::DLatch ctrl_latch1;
		BaseLogic::DLatch ctrl_latch2;

		void sim_DelayedH();
		void sim_HPosLogic(BaseLogic::TriState* HPLA, BaseLogic::TriState* VPLA);
		void sim_VSYNCEarly(BaseLogic::TriState* VPLA);
		void sim_VPosLogic(BaseLogic::TriState* VPLA);
		void sim_VBlankInt();
		void sim_EvenOdd(BaseLogic::TriState* HPLA, BaseLogic::TriState* VPLA);
		void sim_CountersControl(BaseLogic::TriState* HPLA, BaseLogic::TriState* VPLA);

		BaseLogic::TriState* prev_hpla = nullptr;
		BaseLogic::TriState* prev_vpla = nullptr;

		BaseLogic::TriState Prev_n_OBCLIP = BaseLogic::TriState::X;
		BaseLogic::TriState Prev_n_BGCLIP = BaseLogic::TriState::X;
		BaseLogic::TriState Prev_BLACK = BaseLogic::TriState::X;

	public:
		FSM(PPU* parent);
		~FSM();

		void sim(BaseLogic::TriState* HPLA, BaseLogic::TriState* VPLA);

		// These methods are called BEFORE the FSM simulation, by consumer circuits.

		void sim_RESCL_early();
		BaseLogic::TriState get_VB();
		BaseLogic::TriState get_BLNK(BaseLogic::TriState BLACK);
	};

	// H/V Counters

	/// <summary>
	/// One stage of H/V counters.
	/// </summary>
	class HVCounterBit
	{
		PPU* ppu = nullptr;

		BaseLogic::FF ff;
		BaseLogic::DLatch latch;

	public:
		HVCounterBit(PPU* parent) { ppu = parent; }

		BaseLogic::TriState sim(BaseLogic::TriState Carry, BaseLogic::TriState CLR);
		BaseLogic::TriState getOut();
		void set(BaseLogic::TriState val);
	};

	/// <summary>
	/// Implementation of a full counter (H or V).
	/// This does not simulate the propagation delay carry optimization for the low-order bits of the counter, as is done in the real circuit.
	/// The `bits` constructor parameter specifies the bits of the counter.
	/// </summary>
	class HVCounter
	{
		PPU* ppu = nullptr;

		// To simplify, we reserve space for the maximum number of CounterBit instances.
		static const size_t bitCountMax = 16;
		HVCounterBit* bit[bitCountMax] = { 0 };
		size_t bitCount = 0;

	public:
		HVCounter(PPU* parent, size_t bits);
		~HVCounter();

		void sim(BaseLogic::TriState Carry, BaseLogic::TriState CLR);

		size_t get();
		void set(size_t val);

		BaseLogic::TriState getBit(size_t n);
	};

	// H/V Decoder

	union HDecoderInput
	{
		struct
		{
			unsigned H8 : 1;		// input 0
			unsigned n_H8 : 1;
			unsigned H7 : 1;
			unsigned n_H7 : 1;
			unsigned H6 : 1;
			unsigned n_H6 : 1;
			unsigned H5 : 1;
			unsigned n_H5 : 1;
			unsigned H4 : 1;
			unsigned n_H4 : 1;
			unsigned H3 : 1;
			unsigned n_H3 : 1;
			unsigned H2 : 1;
			unsigned n_H2 : 1;
			unsigned H1 : 1;
			unsigned n_H1 : 1;
			unsigned H0 : 1;
			unsigned n_H0 : 1;
			unsigned VB : 1;
			unsigned BLNK : 1;		// input Max
		};
		size_t packed_bits;
	};

	union VDecoderInput
	{
		struct
		{
			unsigned V8 : 1;		// input 0
			unsigned n_V8 : 1;
			unsigned V7 : 1;
			unsigned n_V7 : 1;
			unsigned V6 : 1;
			unsigned n_V6 : 1;
			unsigned V5 : 1;
			unsigned n_V5 : 1;
			unsigned V4 : 1;
			unsigned n_V4 : 1;
			unsigned V3 : 1;
			unsigned n_V3 : 1;
			unsigned V2 : 1;
			unsigned n_V2 : 1;
			unsigned V1 : 1;
			unsigned n_V1 : 1;
			unsigned V0 : 1;
			unsigned n_V0 : 1;		// input Max
		};
		size_t packed_bits;
	};

	class HVDecoder
	{
		PPU* ppu = nullptr;

		// The number of inputs is fixed in all known PPU revisions studied.

		const size_t hpla_inputs = 20;
		const size_t vpla_inputs = 18;

		// The number of HPLA outputs is fixed, and the VPLA outputs differ slightly between NTSC-like and PAL-like PPU revisions.

		const size_t hpla_outputs = 24;
		size_t vpla_outputs = 0;

		BaseLogic::PLA* hpla = nullptr;
		BaseLogic::PLA* vpla = nullptr;

	public:
		HVDecoder(PPU* parent);
		~HVDecoder();

		void sim_HDecoder(BaseLogic::TriState VB, BaseLogic::TriState BLNK, BaseLogic::TriState** outputs);
		void sim_VDecoder(BaseLogic::TriState** outputs);
	};

	// Multiplexer

	class Mux
	{
		PPU* ppu = nullptr;

		BaseLogic::DLatch step1[4];
		BaseLogic::DLatch step2[4];
		BaseLogic::DLatch step3[4];
		BaseLogic::DLatch dir_color[4];

		BaseLogic::DLatch zprio_latch;
		BaseLogic::DLatch bgc_latch;
		BaseLogic::DLatch zcol_latch;
		BaseLogic::DLatch ocol_latch;
		BaseLogic::DLatch tho4_latch;
		BaseLogic::DLatch pal4_latch;

		BaseLogic::FF StrikeFF;

		// Internal intermediate signals

		BaseLogic::TriState n_PAL4 = BaseLogic::TriState::X;
		BaseLogic::TriState OCOL = BaseLogic::TriState::X;
		BaseLogic::TriState EXT = BaseLogic::TriState::X;

		void sim_MuxControl();
		void sim_Spr0Hit();

	public:
		Mux(PPU* parent) { ppu = parent; }
		~Mux() {}

		void sim();
	};

	// OAM

	enum class OAMCellTopology
	{
		TopLeft = 0,
		TopRight,
		BottomLeft,
		BottomRight,
		Middle,
	};

	/// <summary>
	/// OAM Cell. The decay simulation is done simply by the PCLK counter. 
	/// If a cell has not been updated for a long time, its value "fades away" (becomes equal to `z`).
	/// </summary>
	class OAMCell
	{
		PPU* ppu = nullptr;
		BaseLogic::FF decay_ff;

		// The value of the global PCLK counter at the time of writing to the cell.
		// Initially, all cells are in limbo, since there is no drive on them.
		size_t savedPclk = (size_t)-1;

		// Timeout after which the cell value "fades away".
		size_t pclksToDecay = 0;

		void DetermineDecay();

		// Determines the geometric location of the cell in the bank. May be useful for determining the decay time.
		OAMCellTopology topo = OAMCellTopology::Middle;

		// Bank number (0: upper OAM bank, 1: lower OAM bank).
		size_t bank = 0;

	public:
		OAMCell(PPU* parent) { ppu = parent; }
		~OAMCell() {}

		BaseLogic::TriState get();
		void set(BaseLogic::TriState val);

		void SetTopo(OAMCellTopology place, size_t bank_num);
	};

	class OAMLane
	{
		PPU* ppu = nullptr;

		static const size_t cells_per_lane = 32 * 8;	// 32 Rows x 8 Bits

		OAMCell* cells[cells_per_lane];

		bool skip_attr_bits = false;

	public:
		OAMLane(PPU* parent, bool SkipAttrBits);
		~OAMLane();

		void sim(size_t Row, size_t bit_num, BaseLogic::TriState& inOut);
	};

	class OAMBufferBit
	{
	protected:
		PPU* ppu = nullptr;

		BaseLogic::FF Input_FF;
		BaseLogic::FF OB_FF;
		BaseLogic::DLatch R4_out_latch;
		BaseLogic::DLatch out_latch;

	public:
		OAMBufferBit(PPU* parent) { ppu = parent; }
		~OAMBufferBit() {}

		virtual void sim(OAMLane* lane, size_t row, size_t bit_num, BaseLogic::TriState OB_OAM, BaseLogic::TriState n_WE);

		BaseLogic::TriState get();
		void set(BaseLogic::TriState val);
	};

	/// <summary>
	/// Special version for RGB PPU.
	/// </summary>
	class OAMBufferBit_RGB : public OAMBufferBit
	{
	public:
		OAMBufferBit_RGB(PPU* parent) : OAMBufferBit(parent) {}
		~OAMBufferBit_RGB() {}

		virtual void sim(OAMLane* lane, size_t row, size_t bit_num, BaseLogic::TriState OB_OAM, BaseLogic::TriState n_WE);
	};

	class OAM
	{
		PPU* ppu = nullptr;

		static const size_t num_lanes = 8 + 1;	// 0-7: OAM, 8: TempOAM
		OAMLane* lane[num_lanes];

		OAMBufferBit* ob[8];

		BaseLogic::FF OFETCH_FF;
		BaseLogic::FF W4_FF;
		BaseLogic::DLatch latch[4];

		BaseLogic::TriState OB_OAM = BaseLogic::TriState::X;

		// The physical location of the row on the chip.
		size_t ROW = 0;

		OAMLane* sim_AddressDecoder();
		void sim_OBControl();
		void sim_OB(OAMLane* lane);

		size_t RowMap(size_t n);

		OAMDecayBehavior decay_behav = OAMDecayBehavior::Keep;

		void sim_OFETCH_Default();
		void sim_OFETCH_RGB_PPU();

	public:
		OAM(PPU* parent);
		~OAM();

		void sim();

		void sim_OFETCH();

		BaseLogic::TriState get_OB(size_t bit_num);
		void set_OB(size_t bit_num, BaseLogic::TriState val);

		void SetOamDecayBehavior(OAMDecayBehavior behavior);
		OAMDecayBehavior GetOamDecayBehavior();
	};

	// Picture Address Register

	class ParBitInv
	{
		BaseLogic::DLatch in_latch;
		BaseLogic::DLatch out_latch;

	public:
		void sim(BaseLogic::TriState n_PCLK, BaseLogic::TriState O, BaseLogic::TriState INV, BaseLogic::TriState val_in,
			BaseLogic::TriState& val_out);
	};

	class ParBit
	{
		BaseLogic::DLatch pdin_latch;
		BaseLogic::DLatch pdout_latch;
		BaseLogic::DLatch ob_latch;
		BaseLogic::DLatch padx_latch;

	public:
		void sim(BaseLogic::TriState n_PCLK, BaseLogic::TriState O, BaseLogic::TriState val_OB, BaseLogic::TriState val_PD, BaseLogic::TriState OBJ_READ,
			BaseLogic::TriState& PADx);
	};

	class PAR
	{
		PPU* ppu = nullptr;

		ParBitInv inv_bits[4]{};
		ParBit bits[7]{};

		BaseLogic::DLatch fnt_latch;
		BaseLogic::DLatch ob0_latch;
		BaseLogic::DLatch pad12_latch;

		BaseLogic::FF VINV_FF;

		BaseLogic::DLatch pdin_latch;
		BaseLogic::DLatch pdout_latch;
		BaseLogic::DLatch ob_latch;
		BaseLogic::DLatch pad4_latch;

		BaseLogic::DLatch pad0_latch;
		BaseLogic::DLatch pad1_latch;
		BaseLogic::DLatch pad2_latch;

		BaseLogic::TriState O = BaseLogic::TriState::X;
		BaseLogic::TriState VINV = BaseLogic::TriState::X;
		BaseLogic::TriState inv_bits_out[4]{};

		void sim_Control();
		void sim_VInv();
		void sim_ParBitsInv();
		void sim_ParBit4();
		void sim_ParBits();

	public:
		PAR(PPU* parent);
		~PAR();

		void sim();
	};

	// Tile Counters (nesdev `v`)

	class TileCounterBit
	{
		BaseLogic::FF ff;
		BaseLogic::DLatch step_latch;

	public:
		BaseLogic::TriState sim(BaseLogic::TriState Clock, BaseLogic::TriState Load, BaseLogic::TriState Step,
			BaseLogic::TriState val_in, BaseLogic::TriState carry_in,
			BaseLogic::TriState& val_out, BaseLogic::TriState& n_val_out);

		BaseLogic::TriState sim_res(BaseLogic::TriState Clock, BaseLogic::TriState Load, BaseLogic::TriState Step,
			BaseLogic::TriState val_in, BaseLogic::TriState carry_in, BaseLogic::TriState Reset,
			BaseLogic::TriState& val_out, BaseLogic::TriState& n_val_out);
	};

	class TileCnt
	{
		PPU* ppu = nullptr;

		BaseLogic::DLatch w62_latch;
		BaseLogic::FF W62_FF1;
		BaseLogic::FF W62_FF2;
		BaseLogic::DLatch sccnt_latch;
		BaseLogic::DLatch eev_latch1;
		BaseLogic::DLatch eev_latch2;
		BaseLogic::DLatch tvz_latch1;
		BaseLogic::DLatch tvz_latch2;
		BaseLogic::DLatch tvstep_latch;

		TileCounterBit FVCounter[3]{};
		TileCounterBit NTHCounter;
		TileCounterBit NTVCounter;
		TileCounterBit TVCounter[5]{};
		TileCounterBit THCounter[5]{};

		BaseLogic::TriState THZB = BaseLogic::TriState::X;
		BaseLogic::TriState THZ = BaseLogic::TriState::X;
		BaseLogic::TriState TVZB = BaseLogic::TriState::X;
		BaseLogic::TriState TVZ = BaseLogic::TriState::X;
		BaseLogic::TriState FVZ = BaseLogic::TriState::X;

		BaseLogic::TriState TVLOAD = BaseLogic::TriState::X;
		BaseLogic::TriState THLOAD = BaseLogic::TriState::X;
		BaseLogic::TriState TVSTEP = BaseLogic::TriState::X;
		BaseLogic::TriState THSTEP = BaseLogic::TriState::X;

		BaseLogic::TriState NTHIN = BaseLogic::TriState::X;
		BaseLogic::TriState NTVIN = BaseLogic::TriState::X;
		BaseLogic::TriState FVIN = BaseLogic::TriState::X;
		BaseLogic::TriState TVIN = BaseLogic::TriState::X;
		BaseLogic::TriState THIN = BaseLogic::TriState::X;
		BaseLogic::TriState Z_TV = BaseLogic::TriState::X;

		BaseLogic::TriState NTHO = BaseLogic::TriState::X;
		BaseLogic::TriState NTVO = BaseLogic::TriState::X;

		void sim_CountersControl();
		void sim_CountersCarry();
		void sim_FVCounter();
		void sim_NTCounters();
		void sim_TVCounter();
		void sim_THCounter();

	public:
		TileCnt(PPU* parent);
		~TileCnt();

		void sim();
	};

	// PPU Address Mux

	class PAMUX_LowBit
	{
		BaseLogic::DLatch in_latch;
		BaseLogic::DLatch out_latch;

	public:
		void sim(BaseLogic::TriState PCLK, BaseLogic::TriState PARR, BaseLogic::TriState DB_PAR, BaseLogic::TriState PAL, BaseLogic::TriState F_AT,
			BaseLogic::TriState AT_ADR, BaseLogic::TriState NT_ADR, BaseLogic::TriState PAT_ADR, BaseLogic::TriState DB_in,
			BaseLogic::TriState& n_PAx);
	};

	class PAMUX_HighBit
	{
		BaseLogic::DLatch in_latch;
		BaseLogic::DLatch out_latch;

	public:
		void sim(BaseLogic::TriState PCLK, BaseLogic::TriState PARR, BaseLogic::TriState PAH, BaseLogic::TriState F_AT,
			BaseLogic::TriState AT_ADR, BaseLogic::TriState NT_ADR, BaseLogic::TriState PAT_ADR,
			BaseLogic::TriState& n_PAx);
	};

	class PAMUX
	{
		PPU* ppu = nullptr;

		PAMUX_LowBit par_lo[8]{};
		PAMUX_HighBit par_hi[6]{};

		BaseLogic::TriState PARR = BaseLogic::TriState::X;
		BaseLogic::TriState PAH = BaseLogic::TriState::X;
		BaseLogic::TriState PAL = BaseLogic::TriState::X;

		BaseLogic::TriState AT_ADR[14]{};
		BaseLogic::TriState NT_ADR[14]{};
		BaseLogic::TriState PAT_ADR[14]{};

		void sim_Control();

	public:
		PAMUX(PPU* parent);
		~PAMUX();

		void sim();

		void sim_MuxInputs();
		void sim_MuxOutputs();
	};

	// Control Registers

	class ControlRegs
	{
		PPU* ppu = nullptr;

		BaseLogic::DLatch nvis_latch;
		BaseLogic::DLatch clipb_latch;
		BaseLogic::DLatch clipo_latch;

		BaseLogic::FF SCCX_FF1;
		BaseLogic::FF SCCX_FF2;

		BaseLogic::TriState n_W56 = BaseLogic::TriState::X;

		BaseLogic::FF PPU_CTRL0[8];
		BaseLogic::FF PPU_CTRL1[8];

		BaseLogic::DLatch i132_latch;
		BaseLogic::DLatch obsel_latch;
		BaseLogic::DLatch bgsel_latch;
		BaseLogic::DLatch o816_latch;

		BaseLogic::DLatch bgclip_latch;
		BaseLogic::DLatch obclip_latch;
		BaseLogic::DLatch bge_latch;
		BaseLogic::DLatch obe_latch;
		BaseLogic::DLatch tr_latch;
		BaseLogic::DLatch tg_latch;

		// For PAL-like PPUs
		BaseLogic::FF BLACK_FF1;
		BaseLogic::FF BLACK_FF2;
		BaseLogic::DLatch black_latch1;
		BaseLogic::DLatch black_latch2;
		BaseLogic::DLatch vbl_latch;

		void sim_RegularRegOps();
		void sim_W56RegOps();
		void sim_FirstSecond_SCCX_Write();
		void sim_RegFFs();

		BaseLogic::TriState get_Frst();
		BaseLogic::TriState get_Scnd();

		bool RenderAlwaysEnabled = false;
		bool ClippingAlwaysDisabled = false;

		void sim_PalBLACK();

	public:
		ControlRegs(PPU* parent);
		~ControlRegs();

		void sim_RWDecoder();
		void sim();
		void sim_CLP();

		BaseLogic::TriState get_nSLAVE();
	};

	// Scrolling Registers

	class SCC_FF
	{
		BaseLogic::FF ff;

	public:
		void sim(BaseLogic::TriState val_in, BaseLogic::TriState n_DBE, BaseLogic::TriState RC, BaseLogic::TriState& val_out);
		BaseLogic::TriState get();
		void set(BaseLogic::TriState val);
	};

	class ScrollRegs
	{
		friend ControlRegs;
		PPU* ppu = nullptr;

		SCC_FF FineH[3]{};
		SCC_FF FineV[3]{};
		SCC_FF NTV;
		SCC_FF NTH;
		SCC_FF TileV[5]{};
		SCC_FF TileH[5]{};

	public:
		ScrollRegs(PPU* parent);
		~ScrollRegs();

		void sim();
	};

	// Data Reader (Still Picture Generator)

	class DataReader
	{
		PPU* ppu = nullptr;

	public:

		PAR* par = nullptr;
		TileCnt* tilecnt = nullptr;
		PAMUX* pamux = nullptr;
		ScrollRegs* sccx = nullptr;
		BGCol* bgcol = nullptr;

		DataReader(PPU* parent);
		~DataReader();

		void sim();
	};

	// Sprite Comparison

	class OAMCounterBit
	{
		BaseLogic::DLatch cnt_latch;
		BaseLogic::FF keep_ff;

	public:
		BaseLogic::TriState sim(
			BaseLogic::TriState Clock,
			BaseLogic::TriState Load,
			BaseLogic::TriState Step,
			BaseLogic::TriState BlockCount,
			BaseLogic::TriState Reset,
			BaseLogic::TriState val_in,
			BaseLogic::TriState carry_in,
			BaseLogic::TriState& val_out,
			BaseLogic::TriState& n_val_out);

		BaseLogic::TriState get();
		void set(BaseLogic::TriState value);
	};

	class OAMCmprBit
	{
	public:
		BaseLogic::TriState sim(
			BaseLogic::TriState OB_Even,
			BaseLogic::TriState V_Even,
			BaseLogic::TriState OB_Odd,
			BaseLogic::TriState V_Odd,
			BaseLogic::TriState carry_in,
			BaseLogic::TriState& OV_Even,
			BaseLogic::TriState& OV_Odd);
	};

	class OAMPosedgeDFFE
	{
		BaseLogic::FF ff;

	public:
		void sim(
			BaseLogic::TriState CLK,
			BaseLogic::TriState n_EN,
			BaseLogic::TriState val_in,
			BaseLogic::TriState& Q,
			BaseLogic::TriState& n_Q);
	};

	class ObjEval
	{
		PPU* ppu = nullptr;

		OAMCounterBit MainCounter[8]{};
		OAMCounterBit TempCounter[5]{};
		OAMCmprBit cmpr[4]{};

		BaseLogic::TriState OAM_x[8]{};
		BaseLogic::TriState OAM_Temp[5]{};
		BaseLogic::TriState OMSTEP = BaseLogic::TriState::X;
		BaseLogic::TriState OMOUT = BaseLogic::TriState::X;
		BaseLogic::TriState ORES = BaseLogic::TriState::X;
		BaseLogic::TriState OSTEP = BaseLogic::TriState::X;
		BaseLogic::TriState OVZ = BaseLogic::TriState::X;
		BaseLogic::TriState OMFG = BaseLogic::TriState::X;
		BaseLogic::TriState OMV = BaseLogic::TriState::X;
		BaseLogic::TriState TMV = BaseLogic::TriState::X;
		BaseLogic::TriState COPY_STEP = BaseLogic::TriState::X;
		BaseLogic::TriState DO_COPY = BaseLogic::TriState::X;
		BaseLogic::TriState COPY_OVF = BaseLogic::TriState::X;
		BaseLogic::TriState OB_Bits[8]{};
		BaseLogic::TriState W3_Enable = BaseLogic::TriState::X;

		// For PAL PPU
		BaseLogic::DLatch blnk_latch;
		BaseLogic::FF W3_FF1;
		BaseLogic::FF W3_FF2;
		BaseLogic::DLatch w3_latch1;
		BaseLogic::DLatch w3_latch2;
		BaseLogic::DLatch w3_latch3;
		BaseLogic::DLatch w3_latch4;

		BaseLogic::DLatch init_latch;
		BaseLogic::DLatch ofetch_latch;
		BaseLogic::DLatch omv_latch;
		BaseLogic::DLatch eval_latch;
		BaseLogic::DLatch tmv_latch;
		BaseLogic::DLatch nomfg_latch;
		BaseLogic::DLatch ioam2_latch;
		BaseLogic::DLatch temp_latch1;
		BaseLogic::DLatch omfg_latch;
		BaseLogic::DLatch setov_latch;
		BaseLogic::FF OAMCTR2_FF;
		BaseLogic::FF SPR_OV_REG_FF;
		BaseLogic::FF SPR_OV_FF;

		OAMPosedgeDFFE eval_FF1;
		OAMPosedgeDFFE eval_FF2;
		OAMPosedgeDFFE eval_FF3;
		BaseLogic::DLatch fnt_latch;
		BaseLogic::DLatch novz_latch;
		BaseLogic::DLatch i2_latch[6];

		BaseLogic::DLatch OB_latch[8];
		BaseLogic::DLatch ovz_latch;

		// The blocks are simulated in the signal propagation order as the developers intended or we think they intended.

		void sim_StepJohnson();
		void sim_Comparator();
		void sim_ComparisonFSM();
		void sim_MainCounterControl();
		void sim_MainCounter();
		void sim_TempCounterControlBeforeCounter();
		void sim_TempCounter();
		void sim_TempCounterControlAfterCounter();
		void sim_SpriteOVF();
		void sim_OAMAddress();

		// These auxiliary methods are needed to retrieve old values of signals that have not yet been simulated (`uroboros` signals).

		BaseLogic::TriState get_SPR_OV();

	public:
		ObjEval(PPU* parent);
		~ObjEval();

		void sim();
	};

	// Video Signal Generator

	/// <summary>
	/// Implementation of a single bit of the shift register.
	/// </summary>
	class VideoOutSRBit
	{
		BaseLogic::DLatch in_latch;
		BaseLogic::DLatch out_latch;

	public:
		void sim(BaseLogic::TriState n_shift_in, BaseLogic::TriState n_CLK, BaseLogic::TriState CLK, BaseLogic::TriState RES,
			BaseLogic::TriState& shift_out, BaseLogic::TriState& n_shift_out, BaseLogic::TriState& val);

		BaseLogic::TriState get_Out(BaseLogic::TriState RES);
		BaseLogic::TriState get_ShiftOut();
		BaseLogic::TriState getn_ShiftOut();
	};

	class RGB_SEL12x3
	{
		BaseLogic::FF ff[3]{};

	public:
		void sim(BaseLogic::TriState PCLK, BaseLogic::TriState n_Tx, BaseLogic::TriState col_in[12], BaseLogic::TriState lum_in[2]);

		void getOut(BaseLogic::TriState col_out[3]);
	};

	class VideoOut
	{
		PPU* ppu = nullptr;

		float SyncLevel[2]{};
		float BurstLevel[2]{};
		float LumaLevel[4][2]{};
		float EmphasizedLumaLevel[4][2]{};

		BaseLogic::DLatch cc_latch1[4]{};
		BaseLogic::DLatch cc_latch2[4]{};
		BaseLogic::DLatch cc_burst_latch;
		BaseLogic::DLatch sync_latch;
		BaseLogic::DLatch pic_out_latch;
		BaseLogic::DLatch black_latch;
		BaseLogic::DLatch cb_latch;

		// For PAL PPU
		BaseLogic::DLatch npicture_latch1;
		BaseLogic::DLatch npicture_latch2;
		BaseLogic::DLatch v0_latch;
		const size_t chroma_decoder_inputs = 10;
		const size_t chroma_decoder_outputs = 12 * 2 + 1;	// 12*2 phases + 1 for grays
		BaseLogic::PLA* chroma_decoder = nullptr;

		// For RGB PPU
		BaseLogic::DLatch rgb_sync_latch[3]{};
		BaseLogic::DLatch rgb_red_latch[8]{};
		BaseLogic::DLatch rgb_green_latch[8]{};
		BaseLogic::DLatch rgb_blue_latch[8]{};
		BaseLogic::TriState* rgb_output;
		RGB_SEL12x3 red_sel;
		RGB_SEL12x3 green_sel;
		RGB_SEL12x3 blue_sel;

		// RGB PPU Color Matrix
		const size_t color_matrix_inputs = 16;
		const size_t color_matrix_outputs = 12 * 3;	// R->G->B
		BaseLogic::PLA* color_matrix = nullptr;

		VideoOutSRBit sr[6]{};		// Individual bits of the shift register for the phase shifter.

		BaseLogic::TriState n_PZ = BaseLogic::TriState::X;
		BaseLogic::TriState n_POUT = BaseLogic::TriState::X;
		BaseLogic::TriState n_LU[4]{};
		BaseLogic::TriState TINT = BaseLogic::TriState::X;
		BaseLogic::TriState n_PR = BaseLogic::TriState::X;
		BaseLogic::TriState n_PG = BaseLogic::TriState::X;
		BaseLogic::TriState n_PB = BaseLogic::TriState::X;
		BaseLogic::TriState P[13]{};		// Color decoder outputs. The numbering is topological, from left to right starting from 0.
		BaseLogic::TriState PZ[13]{};		// Phase shifter bits corresponding to the Color decoder outputs. The numbering for the Color 0 output is skipped in order to keep the order.
		BaseLogic::TriState PBLACK = BaseLogic::TriState::X;	// Makes colors 14-15 forced "Black".
		BaseLogic::TriState VidOut_n_PICTURE = BaseLogic::TriState::X;	// Local /PICTURE signal

		void sim_InputLatches();
		void sim_PhaseShifter(BaseLogic::TriState n_CLK, BaseLogic::TriState CLK, BaseLogic::TriState RES);
		void sim_ChromaDecoder();
		void sim_ChromaDecoder_PAL();
		void sim_ChromaDecoder_NTSC();
		void sim_OutputLatches();
		void sim_LumaDecoder(BaseLogic::TriState n_LL[4]);
		void sim_Emphasis(BaseLogic::TriState n_TR, BaseLogic::TriState n_TG, BaseLogic::TriState n_TB);
		void sim_CompositeDAC(VideoOutSignal& vout);
		void sim_RGB_DAC(VideoOutSignal& vout);
		void sim_nPICTURE();
		void sim_RAWOutput(VideoOutSignal& vout);
		void sim_ColorMatrix();
		void sim_Select12To3();

		float PhaseSwing(float v, BaseLogic::TriState sel, float level_from, float level_to);

		bool composite = false;
		bool raw = false;

		bool noise_enable = false;
		float noise = 0.0f;
		float GetNoise();

		float Clamp(float val, float min, float max);

		void SetupColorMatrix();
		void SetupChromaDecoderPAL();

		void ConvertRAWToRGB_Composite(VideoOutSignal& rawIn, VideoOutSignal& rgbOut);
		void ConvertRAWToRGB_Component(VideoOutSignal& rawIn, VideoOutSignal& rgbOut);

	public:

		VideoOut(PPU* parent);
		~VideoOut();

		void sim(VideoOutSignal& vout);

		void GetSignalFeatures(VideoSignalFeatures& features);

		void SetRAWOutput(bool enable);

		void ConvertRAWToRGB(VideoOutSignal& rawIn, VideoOutSignal& rgbOut);

		bool IsComposite();

		void SetCompositeNoise(float volts);
	};

	union PALChromaInputs
	{
		struct
		{
			unsigned n_V0 : 1;
			unsigned V0 : 1;
			unsigned n_CC0 : 1;
			unsigned CC0 : 1;
			unsigned n_CC1 : 1;
			unsigned CC1 : 1;
			unsigned n_CC2 : 1;
			unsigned CC2 : 1;
			unsigned n_CC3 : 1;
			unsigned CC3 : 1;
		};
		size_t packed_bits;
	};

	// VRAM Controller

	class RB_Bit
	{
		PPU* ppu = nullptr;

		BaseLogic::FF ff;

	public:
		RB_Bit(PPU* parent) { ppu = parent; }
		~RB_Bit() {}

		void sim(size_t bit_num);

		BaseLogic::TriState get();
		void set(BaseLogic::TriState value);
	};

	class VRAM_Control
	{
		PPU* ppu = nullptr;

		BaseLogic::DLatch wr_latch1;
		BaseLogic::DLatch wr_latch2;
		BaseLogic::DLatch wr_latch3;
		BaseLogic::DLatch wr_latch4;

		BaseLogic::DLatch rd_latch1;
		BaseLogic::DLatch rd_latch2;
		BaseLogic::DLatch rd_latch3;
		BaseLogic::DLatch rd_latch4;

		BaseLogic::FF W7_FF;
		BaseLogic::FF R7_FF;
		BaseLogic::FF WR_FF;
		BaseLogic::FF RD_FF;

		BaseLogic::DLatch tstep_latch;
		BaseLogic::DLatch h0_latch;
		BaseLogic::DLatch blnk_latch;

		BaseLogic::TriState tmp_1 = BaseLogic::TriState::Z;
		BaseLogic::TriState tmp_2 = BaseLogic::TriState::Z;

		RB_Bit* RB[8];

		void sim_RD();
		void sim_WR();
		void sim_ALE();

	public:
		VRAM_Control(PPU* parent);
		~VRAM_Control();

		void sim();

		void sim_TH_MUX();

		void sim_ReadBuffer();
	};

	/// <summary>
	/// Version of the PPU chip, including all known official and pirate variants.
	/// TODO: The full list of official revisions is not yet very clear, since the area has not been researched on our wiki.
	/// See also here: https://www.nesdev.org/wiki/User:Lidnariq/Known_PPU_revisions
	/// </summary>
	enum class Revision
	{
		Unknown = 0,

		// NTSC PPUs.

		RP2C02G,		// It is known that there are two different revisions of `G` (old and new), which differ at least in the PCLK Distribution circuit.
		RP2C02H,

		// RGB PPUs.
		// http://johnsarcade.com/nintendo_vs_ppu_info.php

		RP2C03B,
		RP2C03C,
		RC2C03B,
		RC2C03C,
		RP2C04_0001,
		RP2C04_0002,
		RP2C04_0003,
		RP2C04_0004,
		RC2C05_01,
		RC2C05_02,
		RC2C05_03,
		RC2C05_04,
		RC2C05_99,

		// PAL PPUs.
		// It is unknown how many PAL PPU revisions there were, so far it looks like only one.

		RP2C07_0,

		// TBD: Clone PPUs. Here you can expect a variety of zoo, but in principle they should not differ in circuitry.

		UMC_UA6538,

		Max,
	};

	enum class InputPad
	{
		RnW = 0,
		RS0,
		RS1,
		RS2,
		n_DBE,
		CLK,
		n_RES,
		Max,
	};

	enum class OutputPad
	{
		n_INT = 0,
		ALE,
		n_RD,
		n_WR,
		Max,
	};

	class PPU
	{
		friend ControlRegs;
		friend HVCounterBit;
		friend HVCounter;
		friend HVDecoder;
		friend FSM;
		friend CBBit;
		friend CBBit_RGB;
		friend CRAM;
		friend VideoOut;
		friend Mux;
		friend ObjEval;
		friend OAMBufferBit;
		friend OAMBufferBit_RGB;
		friend OAMCell;
		friend OAM;
		friend FIFOLane;
		friend FIFO;
		friend DataReader;
		friend PAR;
		friend TileCnt;
		friend PAMUX;
		friend ScrollRegs;
		friend BGCol;
		friend RB_Bit;
		friend VRAM_Control;

		/// <summary>
		/// Internal auxiliary and intermediate connections.
		/// See: https://github.com/emu-russia/breaks/blob/master/BreakingNESWiki_DeepL/PPU/rails.md
		/// </summary>
		struct InternalWires
		{
			BaseLogic::TriState n_CLK;			// First half of the PPU cycle
			BaseLogic::TriState CLK;			// Second half of the PPU cycle
			BaseLogic::TriState RES;			// 1: Global reset
			BaseLogic::TriState RC;				// "Registers Clear"
			BaseLogic::TriState n_PCLK;			// The PPU is in the PCLK=0 state
			BaseLogic::TriState PCLK;			// The PPU is in the PCLK=1 state

			BaseLogic::TriState RnW;			// CPU interface operating mode (read/write)
			BaseLogic::TriState RS[3];			// Selecting a register for the CPU interface
			BaseLogic::TriState n_DBE;			// 0: "Data Bus Enable", enable CPU interface
			BaseLogic::TriState n_RD;
			BaseLogic::TriState n_WR;
			BaseLogic::TriState n_W6_1;			// 0: First write to $2006
			BaseLogic::TriState n_W6_2;			// 0: Second write to $2006
			BaseLogic::TriState n_W5_1;			// 0: First write to $2005
			BaseLogic::TriState n_W5_2;			// 0: Second write to $2005
			BaseLogic::TriState n_R7;			// 0: Read $2007
			BaseLogic::TriState n_W7;			// 0: Write $2007
			BaseLogic::TriState n_W4;			// 0: Write $2004
			BaseLogic::TriState n_W3;			// 0: Write $2003
			BaseLogic::TriState n_R2;			// 0: Read $2002
			BaseLogic::TriState n_W1;			// 0: Write $2001
			BaseLogic::TriState n_W0;			// 0: Write $2000
			BaseLogic::TriState n_R4;			// 0: Read $2004

			BaseLogic::TriState I1_32;			// Increment PPU address 1/32.
			BaseLogic::TriState OBSEL;			// Selecting Pattern Table for sprites
			BaseLogic::TriState BGSEL;			// Selecting Pattern Table for background
			BaseLogic::TriState O8_16;			// Object lines 8/16 (sprite size).
			BaseLogic::TriState n_SLAVE;
			BaseLogic::TriState VBL;			// Used in the VBlank interrupt handling circuitry
			BaseLogic::TriState BnW;			// Disable Color Burst, to generate a monochrome picture
			BaseLogic::TriState n_BGCLIP;		// To generate the CLIP_B control signal
			BaseLogic::TriState n_OBCLIP;		// To generate the CLIP_O control signal
			BaseLogic::TriState BGE;
			BaseLogic::TriState BLACK;			// Active when PPU rendering is disabled (see $2001[3] è $2001[4]).
			BaseLogic::TriState OBE;
			BaseLogic::TriState n_TR;			// "Tint Red". Modifying value for Emphasis
			BaseLogic::TriState n_TG;			// "Tint Green". Modifying value for Emphasis
			BaseLogic::TriState n_TB;			// "Tint Blue". Modifying value for Emphasis

			BaseLogic::TriState H0_Dash;		// H0 signal delayed by one DLatch
			BaseLogic::TriState H0_Dash2;		// H0 signal delayed by two DLatch
			BaseLogic::TriState nH1_Dash;		// H1 signal delayed by one DLatch (in inverse logic)
			BaseLogic::TriState H1_Dash2;		// H1 signal delayed by two DLatch
			BaseLogic::TriState nH2_Dash;		// H2 signal delayed by one DLatch (in inverse logic)
			BaseLogic::TriState H2_Dash2;		// H2 signal delayed by two DLatch
			BaseLogic::TriState H3_Dash2;		// H3 signal delayed by two DLatch
			BaseLogic::TriState H4_Dash2;		// H4 signal delayed by two DLatch
			BaseLogic::TriState H5_Dash2;		// H5 signal delayed by two DLatch
			BaseLogic::TriState EvenOddOut;		// Intermediate signal for the HCounter control circuit.
			BaseLogic::TriState HC;				// "HCounter Clear"
			BaseLogic::TriState VC;				// "VCounter Clear"

			BaseLogic::TriState n_ZCOL0;		// Sprite color. The lower 2 bits are in inverse logic, the higher 2 bits are in direct logic.
			BaseLogic::TriState n_ZCOL1;
			BaseLogic::TriState ZCOL2;
			BaseLogic::TriState ZCOL3;
			BaseLogic::TriState n_ZPRIO;		// 0: Priority of sprite over background
			BaseLogic::TriState n_SPR0HIT;		// To detect a Sprite 0 Hit event
			BaseLogic::TriState EXT_In[4];		// Input color from EXT terminals
			BaseLogic::TriState n_EXT_Out[4];	// The output color for EXT terminals (inverse logic)

			BaseLogic::TriState OB[8];
			BaseLogic::TriState n_SPR0_EV;		// 0: Sprite "0" is found on the current line. To define a Sprite 0 Hit event
			BaseLogic::TriState OFETCH;
			BaseLogic::TriState SPR_OV;			// Sprites on the current line are more than 8 or the main OAM counter is full, copying is stopped
			BaseLogic::TriState OAMCTR2;
			BaseLogic::TriState n_OAM[8];		// OAM Address
			BaseLogic::TriState OAM8;			// Selects an additional (temp) OAM for addressing
			BaseLogic::TriState PD_FIFO;		// To zero the output of the H. Inv circuit
			BaseLogic::TriState OV[8];			// Bit 0-7 of the V sprite value
			BaseLogic::TriState n_WE;			// 0: OAM Write

			BaseLogic::TriState n_CLPB;			// 0: To enable background clipping
			BaseLogic::TriState CLPO;			// To enable sprite clipping
			BaseLogic::TriState n_OBJ_RD_ATTR;		// Sprite H value bits. /OBJ_RD_ATTR also goes into V. Inversion.
			BaseLogic::TriState n_OBJ_RD_X;			// Sprite H value bits
			BaseLogic::TriState n_OBJ_RD_A;			// Sprite H value bits
			BaseLogic::TriState n_OBJ_RD_B;			// Sprite H value bits
			BaseLogic::TriState n_ZH;

			BaseLogic::TriState n_PA_Bot[8];
			BaseLogic::TriState n_PA_Top[6];
			BaseLogic::TriState BGC[4];			// Background color
			BaseLogic::TriState FH[3];			// Fine H value
			BaseLogic::TriState FV[3];			// Fine V value
			BaseLogic::TriState NTV;
			BaseLogic::TriState NTVOut;
			BaseLogic::TriState NTH;
			BaseLogic::TriState NTHOut;
			BaseLogic::TriState TV[5];
			BaseLogic::TriState TH[5];
			BaseLogic::TriState THO[5];
			BaseLogic::TriState n_THO[5];
			BaseLogic::TriState TVO[5];
			BaseLogic::TriState n_TVO[5];
			BaseLogic::TriState FVO[3];
			BaseLogic::TriState n_FVO[3];
			BaseLogic::TriState PAT_ADR[14];	// Pattern address (PAR)

			BaseLogic::TriState n_CB_DB;		// 0: CB -> DB
			BaseLogic::TriState n_BW;			// The outputs of the 4 bit CB responsible for the chrominance are controlled by the /BW control signal.
			BaseLogic::TriState n_DB_CB;		// 0: DB -> CB
			BaseLogic::TriState CGA[5];			// Color RAM Address
			BaseLogic::TriState n_CC[4];		// 4 bits of the chrominance of the current "pixel" (inverted value)
			BaseLogic::TriState n_LL[2];		// 2 bits of the luminance of the current "pixel" (inverted value)

			BaseLogic::TriState RD;				// Output value for /RD pin
			BaseLogic::TriState WR;				// Output value for /WR pin
			BaseLogic::TriState n_ALE;			// Output value for ALE pin
			BaseLogic::TriState TSTEP;			// For PAR Counters control logic
			BaseLogic::TriState DB_PAR;			// DB -> PAR
			BaseLogic::TriState PD_RB;			// PD -> Read Buffer
			BaseLogic::TriState TH_MUX;			// Send the TH Counter value to the MUX input, which will cause the value to go into the palette as Direct Color.
			BaseLogic::TriState XRB;			// 0: Enable RB Output
		} wire{};

		/// <summary>
		/// The most important control signals of the H/V FSM.
		/// </summary>
		struct FsmCommands
		{
			BaseLogic::TriState SEV;			// "Start Sprite Evaluation"
			BaseLogic::TriState CLIP_O;			// "Clip Objects". 1: Do not show the left 8 screen points for sprites. Used to get the CLPO signal that goes into the Obj FIFO.
			BaseLogic::TriState CLIP_B;			// "Clip Background". 1: Do not show the left 8 points of the screen for the background. Used to get the /CLPB signal that goes into the Data Reader.
			BaseLogic::TriState ZHPOS;			// "Clear HPos". Clear the H counters in the sprite FIFO and start the FIFO
			BaseLogic::TriState n_EVAL;			// 0: "Sprite Evaluation in Progress"
			BaseLogic::TriState EEV;			// "End Sprite Evaluation"
			BaseLogic::TriState IOAM2;			// "Init OAM2". Initialize an additional (temp) OAM
			BaseLogic::TriState OBJ_READ;		// Common sprite fetch event, shared by many modules. Selecting a tile for an object (sprite)
			BaseLogic::TriState nVIS;			// "Not Visible". The invisible part of the signal (used in sprite logic)
			BaseLogic::TriState nFNT;			// 0: "Fetch Name Table"
			BaseLogic::TriState FTB;			// "Fetch Tile B"
			BaseLogic::TriState FTA;			// "Fetch Tile A"
			BaseLogic::TriState FAT;			// "Fetch Attribute Table"
			BaseLogic::TriState nFO;			// 0: "Fetch Output Enable"
			BaseLogic::TriState BPORCH;			// "Back Porch"
			BaseLogic::TriState SCCNT;			// "Scroll Counters Control". Update the scrolling registers.
			BaseLogic::TriState nHB;			// "HBlank"
			BaseLogic::TriState BURST;			// Color Burst
			BaseLogic::TriState SYNC;			// Sync pulse
			BaseLogic::TriState n_PICTURE;		// 0: Visible part of the scan-lines
			BaseLogic::TriState RESCL;			// "Reset FF Clear" / "VBlank Clear". VBlank period end event.
			BaseLogic::TriState VSYNC;			// Vertical sync pulse
			BaseLogic::TriState nVSET;			// "VBlank Set". VBlank period start event.
			BaseLogic::TriState VB;				// Active when the invisible part of the video signal is output (used only in H Decoder)
			BaseLogic::TriState BLNK;			// Active when PPU rendering is disabled (by BLACK signal) or during VBlank
			BaseLogic::TriState INT;			// "Interrupt". PPU Interrupt
		} fsm{};

		Revision rev = Revision::Unknown;

		void sim_PCLK();

		BaseLogic::FF Reset_FF;

		BaseLogic::DLatch pclk_1;
		BaseLogic::DLatch pclk_2;
		BaseLogic::DLatch pclk_3;
		BaseLogic::DLatch pclk_4;
		BaseLogic::DLatch pclk_5;	// PAL PPU
		BaseLogic::DLatch pclk_6;	// PAL PPU
		size_t pclk_counter = 0;
		BaseLogic::TriState Prev_PCLK = BaseLogic::TriState::X;

		ControlRegs* regs = nullptr;
		HVCounter* h = nullptr;
		HVCounter* v = nullptr;
		HVDecoder* hv_dec = nullptr;
		FSM* hv_fsm = nullptr;
		CRAM* cram = nullptr;
		VideoOut* vid_out = nullptr;
		Mux* mux = nullptr;
		ObjEval* eval = nullptr;
		OAM* oam = nullptr;
		FIFO* fifo = nullptr;
		VRAM_Control* vram_ctrl = nullptr;
		DataReader* data_reader = nullptr;

		// The internal PPU buses do not use the Bus Conflicts resolver because of the large Capacitance.

		uint8_t DB = 0;				// CPU I/F Data bus
		uint8_t PD = 0;				// Internal PPU Data bus

		void sim_BusInput(uint8_t* ext, uint8_t* data_bus, uint8_t* ad_bus);
		void sim_BusOutput(uint8_t* ext, uint8_t* data_bus, uint8_t* ad_bus, uint8_t* addrHi_bus);

		BaseLogic::DLatch extout_latch[4]{};

	public:
		PPU(Revision rev, bool VideoGen = false);
		~PPU();

		/// <summary>
		/// Simulate one half cycle (state) of the PPU.
		/// </summary>
		/// <param name="inputs">Inputs (see `InputPad`)</param>
		/// <param name="outputs">Outputs (see `OutputPad`)</param>
		/// <param name="ext">Bidirectional EXT color bus</param>
		/// <param name="data_bus">Bidirectional CPU-PPU data bus</param>
		/// <param name="ad_bus">Bidirectional PPU-VRAM data/address bus</param>
		/// <param name="addrHi_bus">This bus carries the rest of the address lines (output)</param>
		/// <param name="vout">The output video signal.</param>
		void sim(BaseLogic::TriState inputs[], BaseLogic::TriState outputs[], uint8_t* ext, uint8_t* data_bus, uint8_t* ad_bus, uint8_t* addrHi_bus, VideoOutSignal& vout);

		size_t GetPCLKCounter();

		void ResetPCLKCounter();

		const char* RevisionToStr(Revision rev);

		BaseLogic::TriState GetDBBit(size_t n);

		void SetDBBit(size_t n, BaseLogic::TriState bit_val);

		BaseLogic::TriState GetPDBit(size_t n);

		void SetPDBit(size_t n, BaseLogic::TriState bit_val);

		size_t GetHCounter();
		size_t GetVCounter();

		/// <summary>
		/// Get the video signal properties of the current PPU revision.
		/// </summary>
		/// <param name="features"></param>
		void GetSignalFeatures(VideoSignalFeatures& features);

		/// <summary>
		/// Switch the video output to RAW Color mode.
		/// </summary>
		/// <param name="enable"></param>
		void SetRAWOutput(bool enable);

		/// <summary>
		/// Convert RAW Color to RGB. A video generator simulation circuit will be activated, which will return a sample corresponding to the current PPU revision.
		/// </summary>
		/// <param name="rawIn"></param>
		/// <param name="rgbOut"></param>
		void ConvertRAWToRGB(VideoOutSignal& rawIn, VideoOutSignal& rgbOut);

		/// <summary>
		/// Set one of the ways to decay OAM cells.
		/// </summary>
		void SetOamDecayBehavior(OAMDecayBehavior behavior);

		/// <summary>
		/// Returns the nature of the PPU - composite or RGB.
		/// </summary>
		bool IsComposite();

		/// <summary>
		/// Noise value (V).
		/// </summary>
		/// <param name="volts">Noise +/- value. 0 to disable.</param>
		void SetCompositeNoise(float volts);
	};
}