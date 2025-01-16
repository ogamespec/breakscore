#pragma once

namespace APUSim
{
	class APU;

#pragma pack(push,1)

	/// <summary>
	/// A software descriptor of the current audio sample.
	/// </summary>
	union AudioOutSignal
	{
		struct AnalogOut
		{
			float a;		// Analog output AUX_A in mV
			float b;		// Analog output AUX_B in mV
		} AUX;

		struct NormalizedOut
		{
			float a;	// Analog output AUX_A, normalized to the interval [0.0; 1.0]
			float b;	// Analog output AUX_B, normalized to the interval [0.0; 1.0]
		} normalized;

		/// <summary>
		/// "Raw" output from the sound generators.
		/// </summary>
		struct RAWOut
		{
			uint8_t sqa;	// Square0 channel digital output 0..15 (4 bit)
			uint8_t sqb;	// Square1 channel digital output 0..15 (4 bit)
			uint8_t tri;	// Triangle channel digital output 0..15 (4 bit)
			uint8_t rnd;	// Noise channel digital output 0..15 (4 bit)
			uint8_t dmc;	// DeltaMod channel digital output 0..127 (7 bit)
		} RAW;
	};

	/// <summary>
	/// The structure describes all the features of the signal and helps organize its rendering.
	/// </summary>
	struct AudioSignalFeatures
	{
		int32_t SampleRate;		// The sampling frequency of the audio signal (samples per "virtual" second). The audio is actually sampled every PHI (core clock) cycle.
		float AUXA_LowLevel;		// Lower signal level for AUX_A (mV)
		float AUXA_HighLevel;		// Upper signal level for AUX_A (mV)
		float AUXB_LowLevel;		// Lower signal level for AUX_B (mV)
		float AUXB_HighLevel;		// Upper signal level for AUX_B (mV)
	};

#pragma pack(pop)

	/// <summary>
	/// All known revisions of official APUs and compatible clones.
	/// </summary>
	enum class Revision
	{
		Custom = -1,		// Fully custom implementation, defined solely by the ChipFeature set
		Unknown = 0,
		RP2A03,				// "Letterless"
		RP2A03G,
		RP2A03H,
		RP2A07,
		UA6527P,
		TA03NP1,
		Max,
	};

	enum class APU_Input
	{
		CLK = 0,
		n_NMI,
		n_IRQ,
		n_RES,
		DBG,
		Max,
	};

	enum class APU_Output
	{
		n_IN0 = 0,
		n_IN1,
		OUT_0,
		OUT_1,
		OUT_2,
		M2,
		RnW,
		Max,
	};

	/// <summary>
	/// The list of distinctive features of the chip implementation. Earlier the features were chosen by revision, 
	/// but this entity looks prettier - you can assemble a customized "chimera" chip from different features.
	/// TBD: Very preliminary version.
	/// </summary>
	enum ChipFeature : uint64_t
	{
		ByRevision = 0x0,			// Automatically determined by the chip revision in the constructor
		QuartzPal = 0x1,
		QuartzNtsc = 0x2,
		QuartzCustom = 0x4,
		DividerPal = 0x8,
		DividerNtsc = 0x10,
		DividerCustom = 0x20,
		MixedSquareDuty = 0x40,
		ClickingTriangle = 0x80,
		NoiseMalfunction = 0x100,
		BCDPresent = 0x200,
		DebugRegistersInsteadHalt = 0x400,
		DebugHaltInsteadRegisters = 0x800,
	};

	// Common elements of APU circuitry

	class RegisterBit
	{
		BaseLogic::DLatch transp_latch{};

	public:
		void sim(BaseLogic::TriState ACLK1, BaseLogic::TriState Enable, BaseLogic::TriState Value);
		BaseLogic::TriState get();
		BaseLogic::TriState nget();
		void set(BaseLogic::TriState val);
	};

	class RegisterBitRes
	{
		BaseLogic::DLatch transp_latch{};

	public:
		void sim(BaseLogic::TriState ACLK1, BaseLogic::TriState Enable, BaseLogic::TriState Value, BaseLogic::TriState Res);
		BaseLogic::TriState get();
		BaseLogic::TriState nget();
		void set(BaseLogic::TriState val);
	};

	class RegisterBitRes2
	{
		BaseLogic::DLatch transp_latch{};

	public:
		void sim(BaseLogic::TriState ACLK1, BaseLogic::TriState Enable, BaseLogic::TriState Value, BaseLogic::TriState Res1, BaseLogic::TriState Res2);
		BaseLogic::TriState get();
		BaseLogic::TriState nget();
		void set(BaseLogic::TriState val);
	};

	class CounterBit
	{
		BaseLogic::DLatch transp_latch{};
		BaseLogic::DLatch cg_latch{};

	public:
		BaseLogic::TriState sim(
			BaseLogic::TriState Carry,
			BaseLogic::TriState Clear,
			BaseLogic::TriState Load,
			BaseLogic::TriState Step,
			BaseLogic::TriState ACLK1,
			BaseLogic::TriState val);
		BaseLogic::TriState get();
		BaseLogic::TriState nget();
		void set(BaseLogic::TriState val);
	};

	class DownCounterBit
	{
		BaseLogic::DLatch transp_latch{};
		BaseLogic::DLatch cg_latch{};

	public:
		BaseLogic::TriState sim(
			BaseLogic::TriState Carry,
			BaseLogic::TriState Clear,
			BaseLogic::TriState Load,
			BaseLogic::TriState Step,
			BaseLogic::TriState ACLK1,
			BaseLogic::TriState val);
		BaseLogic::TriState get();
		BaseLogic::TriState nget();
		void set(BaseLogic::TriState val);
	};

	class RevCounterBit
	{
		BaseLogic::DLatch transp_latch{};
		BaseLogic::DLatch cg_latch{};

	public:
		BaseLogic::TriState sim(
			BaseLogic::TriState Carry,
			BaseLogic::TriState Dec,
			BaseLogic::TriState Clear,
			BaseLogic::TriState Load,
			BaseLogic::TriState Step,
			BaseLogic::TriState ACLK1,
			BaseLogic::TriState val);
		BaseLogic::TriState get();
		BaseLogic::TriState nget();
		void set(BaseLogic::TriState val);
	};

	// Timing Generator

	class SoftCLK_SRBit
	{
		BaseLogic::DLatch in_latch{};
		BaseLogic::DLatch out_latch{};

	public:
		void sim(BaseLogic::TriState ACLK1, BaseLogic::TriState F1, BaseLogic::TriState F2, BaseLogic::TriState shift_in);

		BaseLogic::TriState get_sout();
		BaseLogic::TriState get_nsout();
	};

	class CLKGen
	{
		APU* apu = nullptr;

		BaseLogic::DLatch phi1_latch{};
		BaseLogic::DLatch phi2_latch{};

		BaseLogic::TriState shift_in = BaseLogic::TriState::X;
		BaseLogic::TriState F1 = BaseLogic::TriState::X;
		BaseLogic::TriState F2 = BaseLogic::TriState::X;
		BaseLogic::TriState Z1 = BaseLogic::TriState::X;
		BaseLogic::TriState Z2 = BaseLogic::TriState::X;
		BaseLogic::TriState mode = BaseLogic::TriState::X;
		BaseLogic::TriState n_mode = BaseLogic::TriState::X;
		BaseLogic::TriState pla[6]{};

		BaseLogic::FF z_ff{};
		BaseLogic::DLatch z1{};
		BaseLogic::DLatch z2{};
		BaseLogic::DLatch md_latch{};
		BaseLogic::DLatch int_status{};
		BaseLogic::FF int_ff{};

		SoftCLK_SRBit lfsr[15]{};

		RegisterBit reg_mode{};
		RegisterBit reg_mask{};

		void sim_ACLK();
		void sim_SoftCLK_Mode();
		void sim_SoftCLK_Control();
		void sim_SoftCLK_PLA();
		void sim_SoftCLK_LFSR();

	public:
		CLKGen(APU* parent);
		~CLKGen();

		void sim();

		BaseLogic::TriState GetINTFF();
	};

	// 6502 Core Binding

	class DIV_SRBit
	{
		BaseLogic::DLatch in_latch{};
		BaseLogic::DLatch out_latch{};

	public:
		void sim(BaseLogic::TriState q, BaseLogic::TriState nq, BaseLogic::TriState rst, BaseLogic::TriState sin);

		BaseLogic::TriState get_sout(BaseLogic::TriState rst);
		BaseLogic::TriState get_nval();
	};

	class CoreBinding
	{
		APU* apu = nullptr;

		BaseLogic::FF CLK_FF{};
		DIV_SRBit div[6]{};

		void sim_DividerBeforeCore();
		void sim_DividerAfterCore();

	public:
		CoreBinding(APU* parent);
		~CoreBinding();

		void sim();
	};

	// OAM DMA

	class DMA
	{
		APU* apu = nullptr;

		static const uint16_t PPU_Addr = 0x2004;

		CounterBit spr_lo[8]{};
		RegisterBit spr_hi[8]{};

		BaseLogic::DLatch spre_latch{};
		BaseLogic::DLatch nospr_latch{};
		BaseLogic::DLatch dospr_latch{};
		BaseLogic::FF StopDMA{};
		BaseLogic::FF StartDMA{};
		BaseLogic::FF DMADirToggle{};

		BaseLogic::DLatch spr_buf[8]{};

		BaseLogic::TriState SPRE = BaseLogic::TriState::X;		// OAM counter overflow signal. Used to determine if an OAM DMA is complete.
		BaseLogic::TriState SPRS = BaseLogic::TriState::X;		// A signal that enables OAM counter (low 8 bits of the address). The signal is silenced by RUNDMC.

		BaseLogic::TriState NOSPR = BaseLogic::TriState::X;		// When NOSPR is 0 - the OAM DMA circuitry performs its activities to provide the OAM DMA process.
		BaseLogic::TriState DOSPR = BaseLogic::TriState::X;		// Latches the OAM DMA start event
		BaseLogic::TriState sprdma_rdy = BaseLogic::TriState::X;	// aka oamdma_rdy

		void sim_DMA_Address();
		void sim_DMA_Control();

	public:
		DMA(APU* parent);
		~DMA();

		void sim();
		void sim_DMA_Buffer();
		void sim_AddressMux();
	};

	// Differential Pulse-code Modulation (DPCM) Channel

	class DPCM_LFSRBit
	{
		BaseLogic::DLatch in_latch{};
		BaseLogic::DLatch out_latch{};

	public:
		void sim(BaseLogic::TriState ACLK1, BaseLogic::TriState load, BaseLogic::TriState step, BaseLogic::TriState val, BaseLogic::TriState shift_in);
		BaseLogic::TriState get_sout();
	};

	class DPCM_SRBit
	{
		BaseLogic::DLatch in_latch{};
		BaseLogic::DLatch out_latch{};

	public:
		void sim(BaseLogic::TriState ACLK1, BaseLogic::TriState clear, BaseLogic::TriState load, BaseLogic::TriState step, BaseLogic::TriState n_val, BaseLogic::TriState shift_in);
		BaseLogic::TriState get_sout();
	};

	class DpcmChan
	{
		APU* apu = nullptr;

		BaseLogic::TriState LOOPMode = BaseLogic::TriState::X;	// 1: DPCM looped playback
		BaseLogic::TriState n_IRQEN = BaseLogic::TriState::X;	// 0: Enable interrupt from DPCM
		BaseLogic::TriState DSLOAD = BaseLogic::TriState::X;	// Load value into Sample Counter and simultaneously into DPCM Address Counter
		BaseLogic::TriState DSSTEP = BaseLogic::TriState::X;	// Perform Sample Counter decrement and DPCM Address Counter increment simultaneously
		BaseLogic::TriState BLOAD = BaseLogic::TriState::X;		// Load value into Sample Buffer
		BaseLogic::TriState BSTEP = BaseLogic::TriState::X;		// Perform a Sample Buffer bit shift
		BaseLogic::TriState NSTEP = BaseLogic::TriState::X;		// Perform Sample Bit Counter increment
		BaseLogic::TriState DSTEP = BaseLogic::TriState::X;		// Increment/decrement the DPCM Output counter
		BaseLogic::TriState PCM = BaseLogic::TriState::X;		// Load new sample value into Sample Buffer. The signal is active when PHI1 = 0 and the address bus is captured (imitating CPU reading)
		BaseLogic::TriState DOUT = BaseLogic::TriState::X;		// DPCM Out counter has finished counting
		BaseLogic::TriState n_NOUT = BaseLogic::TriState::X;	// 0: Sample Bit Counter has finished counting
		BaseLogic::TriState SOUT = BaseLogic::TriState::X;		// Sample Counter has finished counting
		BaseLogic::TriState DFLOAD = BaseLogic::TriState::X;	// Frequency LFSR finished counting and reloaded itself
		BaseLogic::TriState n_BOUT = BaseLogic::TriState::X;	// The next bit value pushed out of the Sample Buffer shift register (inverted value)
		BaseLogic::TriState Fx[4]{};
		BaseLogic::TriState FR[9]{};
		BaseLogic::TriState Dec1_out[16]{};

		BaseLogic::TriState ED1 = BaseLogic::TriState::X;		// 1: Assert interrupt
		BaseLogic::TriState ED2 = BaseLogic::TriState::X;		// 1: DMC enable ($4015[4])
		BaseLogic::TriState DMC1 = BaseLogic::TriState::X;		// 1: PCM (DMA fetch) done
		BaseLogic::TriState DMC2 = BaseLogic::TriState::X;		// 1: DMC finish
		BaseLogic::TriState CTRL1 = BaseLogic::TriState::X;		// 0: Stop DMA
		BaseLogic::TriState CTRL2 = BaseLogic::TriState::X;		// 0: DMC enable delay

		BaseLogic::TriState ACLK2 = BaseLogic::TriState::X;

		BaseLogic::FF int_ff{};
		BaseLogic::DLatch sout_latch{};
		RegisterBitRes2 ena_ff{};
		BaseLogic::DLatch run_latch1{};
		BaseLogic::DLatch run_latch2{};
		BaseLogic::FF start_ff{};
		BaseLogic::FF rdy_ff{};
		BaseLogic::DLatch en_latch1{};
		BaseLogic::DLatch en_latch2{};
		BaseLogic::DLatch en_latch3{};
		BaseLogic::FF step_ff{};
		BaseLogic::FF stop_ff{};
		BaseLogic::FF pcm_ff{};
		BaseLogic::DLatch dout_latch{};
		BaseLogic::DLatch dstep_latch{};
		BaseLogic::DLatch stop_latch{};
		BaseLogic::DLatch pcm_latch{};
		BaseLogic::DLatch nout_latch{};

		RegisterBit freq_reg[4]{};
		RegisterBit loop_reg{};
		RegisterBit irq_reg{};
		DPCM_LFSRBit lfsr[9]{};
		RegisterBit scnt_reg[8]{};
		DownCounterBit scnt[12]{};
		CounterBit sbcnt[3]{};
		RegisterBit buf_reg[8]{};
		DPCM_SRBit shift_reg[8]{};
		RegisterBit addr_reg[8]{};
		CounterBit addr_lo[8]{};
		CounterBit addr_hi[7]{};
		RevCounterBit out_cnt[6]{};
		RegisterBit out_reg{};

		void sim_ControlReg();
		void sim_IntControl();
		void sim_EnableControl();
		void sim_DMAControl();
		void sim_SampleCounterControl();
		void sim_SampleBufferControl();

		void sim_Decoder1();
		void sim_Decoder2();
		void sim_FreqLFSR();
		void sim_SampleCounterReg();
		void sim_SampleCounter();
		void sim_SampleBitCounter();
		void sim_SampleBuffer();

		void sim_AddressReg();
		void sim_AddressCounter();
		void sim_Output();

		BaseLogic::TriState get_CTRL1();
		BaseLogic::TriState get_CTRL2();
		BaseLogic::TriState get_DMC1();

	public:
		DpcmChan(APU* parent);
		~DpcmChan();

		void sim();
	};

	// Length Counters

	class LengthCounter
	{
		APU* apu = nullptr;

		BaseLogic::DLatch reg_enable_latch{};
		BaseLogic::DLatch ena_latch{};
		BaseLogic::DLatch cout_latch{};
		BaseLogic::FF stat_ff{};
		BaseLogic::DLatch step_latch{};
		BaseLogic::TriState STEP{};

		BaseLogic::DLatch dec_latch[5]{};
		BaseLogic::TriState Dec1_out[32]{};

		BaseLogic::TriState LC[8]{};
		DownCounterBit cnt[8]{};
		BaseLogic::TriState carry_out{};

		void sim_Control(size_t bit_ena, BaseLogic::TriState WriteEn, BaseLogic::TriState& LC_NoCount);
		void sim_Decoder1();
		void sim_Decoder2();
		void sim_Counter(BaseLogic::TriState LC_CarryIn, BaseLogic::TriState WriteEn);

	public:
		LengthCounter(APU* parent);
		~LengthCounter();

		void sim(size_t bit_ena, BaseLogic::TriState WriteEn, BaseLogic::TriState LC_CarryIn, BaseLogic::TriState& LC_NoCount);
	};

	// Envelope Unit
	// Shared between the square channels and the noise generator.

	class EnvelopeUnit
	{
		APU* apu = nullptr;

		RegisterBit envdis_reg{};
		RegisterBit lc_reg{};
		RegisterBit vol_reg[4]{};
		DownCounterBit decay_cnt[4]{};
		DownCounterBit env_cnt[4]{};
		BaseLogic::FF EnvReload{};
		BaseLogic::DLatch erld_latch{};
		BaseLogic::DLatch reload_latch{};
		BaseLogic::DLatch rco_latch{};
		BaseLogic::DLatch eco_latch{};

	public:
		EnvelopeUnit(APU* parent);
		~EnvelopeUnit();

		void sim(BaseLogic::TriState V[4], BaseLogic::TriState WR_Reg, BaseLogic::TriState WR_LC);
		BaseLogic::TriState get_LC();
	};

	// Noise Channel

	class FreqLFSRBit
	{
		BaseLogic::DLatch in_latch{};
		BaseLogic::DLatch out_latch{};

	public:
		void sim(BaseLogic::TriState ACLK1, BaseLogic::TriState load, BaseLogic::TriState step, BaseLogic::TriState val, BaseLogic::TriState shift_in);
		BaseLogic::TriState get_sout();
	};

	class RandomLFSRBit
	{
		RegisterBit in_reg{};
		BaseLogic::DLatch out_latch{};

	public:
		void sim(BaseLogic::TriState ACLK1, BaseLogic::TriState load, BaseLogic::TriState shift_in);
		BaseLogic::TriState get_sout();
	};

	class NoiseChan
	{
		friend APU;

		APU* apu = nullptr;

		BaseLogic::TriState NNF[11]{};
		BaseLogic::TriState RSTEP = BaseLogic::TriState::X;
		BaseLogic::TriState RNDOUT = BaseLogic::TriState::X;
		BaseLogic::TriState Vol[4]{};
		BaseLogic::TriState Dec1_out[16]{};

		RegisterBitRes freq_reg[4]{};
		FreqLFSRBit freq_lfsr[11]{};
		RegisterBit rmod_reg{};
		RandomLFSRBit rnd_lfsr[15]{};
		EnvelopeUnit* env_unit = nullptr;

		void sim_FreqReg();
		void sim_Decoder1();
		void sim_Decoder1_Calc(BaseLogic::TriState* F, BaseLogic::TriState* nF);
		void sim_Decoder2();
		void sim_FreqLFSR();
		void sim_RandomLFSR();

	public:
		NoiseChan(APU* parent);
		~NoiseChan();

		void sim();
		BaseLogic::TriState get_LC();
	};

	// Square Channels

	/// <summary>
	/// Input #carry connection option for the adder.
	/// In the real circuit there is no `AdderCarryMode` signal. For Square0 the input n_carry is connected directly to VDD and for Square1 it is connected to INC.
	/// But we cheat a little bit here for convenience by making the connection using multiplexer.
	/// </summary>
	enum SquareChanCarryIn
	{
		Unknown = 0,
		Vdd,
		Inc,
	};

	class FreqRegBit
	{
		BaseLogic::DLatch transp_latch{};
		BaseLogic::DLatch sum_latch{};

	public:
		void sim(BaseLogic::TriState ACLK3, BaseLogic::TriState ACLK1, BaseLogic::TriState WR, BaseLogic::TriState DB_in, BaseLogic::TriState ADDOUT, BaseLogic::TriState n_sum);
		BaseLogic::TriState get_nFx(BaseLogic::TriState ADDOUT);
		BaseLogic::TriState get_Fx(BaseLogic::TriState ADDOUT);
		BaseLogic::TriState get();
		void set(BaseLogic::TriState value);
	};

	class AdderBit
	{
	public:
		void sim(BaseLogic::TriState F, BaseLogic::TriState nF, BaseLogic::TriState S, BaseLogic::TriState nS, BaseLogic::TriState C, BaseLogic::TriState nC,
			BaseLogic::TriState& cout, BaseLogic::TriState& n_cout, BaseLogic::TriState& n_sum);
	};

	class SquareChan
	{
		friend APU;

		APU* apu = nullptr;
		SquareChanCarryIn cin_type = SquareChanCarryIn::Unknown;

		BaseLogic::TriState n_sum[11]{};
		BaseLogic::TriState S[11]{};
		BaseLogic::TriState SR[3]{};
		BaseLogic::TriState BS[12]{};
		BaseLogic::TriState DEC = BaseLogic::TriState::X;
		BaseLogic::TriState INC = BaseLogic::TriState::X;
		BaseLogic::TriState n_COUT = BaseLogic::TriState::X;
		BaseLogic::TriState SW_UVF = BaseLogic::TriState::X;	// Sweep underflow
		BaseLogic::TriState FCO = BaseLogic::TriState::X;
		BaseLogic::TriState FLOAD = BaseLogic::TriState::X;
		BaseLogic::TriState DO_SWEEP = BaseLogic::TriState::X;
		BaseLogic::TriState SW_OVF = BaseLogic::TriState::X;	// Sweep overflow
		BaseLogic::TriState DUTY = BaseLogic::TriState::X;
		BaseLogic::TriState Vol[4]{};

		RegisterBit dir_reg{};
		FreqRegBit freq_reg[11]{};
		RegisterBit sr_reg[3]{};
		AdderBit adder[11]{};
		BaseLogic::DLatch fco_latch{};
		DownCounterBit freq_cnt[11]{};
		RegisterBit swdis_reg{};
		BaseLogic::DLatch reload_latch{};
		BaseLogic::DLatch sco_latch{};
		BaseLogic::FF reload_ff{};
		RegisterBit sweep_reg[3]{};
		DownCounterBit sweep_cnt[3]{};
		RegisterBit duty_reg[2]{};
		DownCounterBit duty_cnt[3]{};
		BaseLogic::DLatch sqo_latch{};

		EnvelopeUnit* env_unit = nullptr;

		void sim_FreqReg(BaseLogic::TriState WR2, BaseLogic::TriState WR3);
		void sim_ShiftReg(BaseLogic::TriState WR1);
		void sim_BarrelShifter();
		void sim_Adder();
		void sim_FreqCounter();
		void sim_Sweep(BaseLogic::TriState WR1, BaseLogic::TriState NOSQ);
		void sim_Duty(BaseLogic::TriState WR0, BaseLogic::TriState WR3);
		void sim_Output(BaseLogic::TriState NOSQ, BaseLogic::TriState* SQ_Out);

	public:
		SquareChan(APU* parent, SquareChanCarryIn carry_routing);
		~SquareChan();

		void sim(BaseLogic::TriState WR0, BaseLogic::TriState WR1, BaseLogic::TriState WR2, BaseLogic::TriState WR3, BaseLogic::TriState NOSQ, BaseLogic::TriState* SQ_Out);
		BaseLogic::TriState get_LC();
	};

	// Triangle Channel

	class TriangleChan
	{
		APU* apu = nullptr;

		BaseLogic::TriState TCO = BaseLogic::TriState::Z;
		BaseLogic::TriState FOUT = BaseLogic::TriState::Z;
		BaseLogic::TriState n_FOUT = BaseLogic::TriState::X;
		BaseLogic::TriState LOAD = BaseLogic::TriState::X;
		BaseLogic::TriState STEP = BaseLogic::TriState::X;
		BaseLogic::TriState TSTEP = BaseLogic::TriState::X;

		RegisterBit lc_reg{};
		BaseLogic::FF Reload_FF{};
		BaseLogic::DLatch reload_latch1{};
		BaseLogic::DLatch reload_latch2{};
		BaseLogic::DLatch tco_latch{};

		RegisterBit lin_reg[7]{};
		DownCounterBit lin_cnt[7]{};
		RegisterBit freq_reg[11]{};
		DownCounterBit freq_cnt[11]{};
		BaseLogic::DLatch fout_latch{};
		CounterBit out_cnt[5]{};

		void sim_Control();
		void sim_LinearReg();
		void sim_LinearCounter();
		void sim_FreqReg();
		void sim_FreqCounter();
		void sim_Output();

	public:
		TriangleChan(APU* parent);
		~TriangleChan();

		void sim();
		BaseLogic::TriState get_LC();
	};

	// Register Decoder

	class RegsDecoder
	{
		APU* apu = nullptr;

		BaseLogic::TriState pla[29]{};		// Decoder

		BaseLogic::TriState nREGWR = BaseLogic::TriState::X;
		BaseLogic::TriState nREGRD = BaseLogic::TriState::X;

		BaseLogic::DLatch lock_latch{};

		void sim_DebugLock();
		void sim_Predecode();
		void sim_Decoder();
		void sim_RegOps();

	public:
		RegsDecoder(APU* parent);
		~RegsDecoder();

		void sim();
		void sim_DebugRegisters();
	};

	// Simulation of APU chip terminals and everything related to them.

	class BIDIR
	{
		BaseLogic::DLatch in_latch{};		// pad -> terminal circuit
		BaseLogic::DLatch out_latch{};		// terminal circuit -> pad

	public:
		void sim(BaseLogic::TriState pad_in, BaseLogic::TriState to_pad,
			BaseLogic::TriState& from_pad, BaseLogic::TriState& pad_out,
			BaseLogic::TriState rd, BaseLogic::TriState wr);

		void sim_Input(BaseLogic::TriState pad_in, BaseLogic::TriState& from_pad, BaseLogic::TriState rd);
		void sim_Output(BaseLogic::TriState to_pad, BaseLogic::TriState& pad_out, BaseLogic::TriState wr);

		BaseLogic::TriState get_in();
		BaseLogic::TriState get_out();
		void set_in(BaseLogic::TriState val);
		void set_out(BaseLogic::TriState val);
	};

	class Pads
	{
		APU* apu = nullptr;

		BIDIR n_irq{};
		BIDIR n_nmi{};
		BIDIR data_bus[8]{};
		BIDIR n_in[2]{};
		BIDIR out[3]{};

		BaseLogic::TriState OUT_Signal[3]{};
		RegisterBit out_reg[3]{};
		BaseLogic::DLatch out_latch[3]{};

		BaseLogic::TriState unused = BaseLogic::TriState::Z;

		void sim_OutReg();

	public:
		Pads(APU* parent);
		~Pads();

		void sim_InputPads(BaseLogic::TriState inputs[]);
		void sim_OutputPads(BaseLogic::TriState outputs[], uint16_t* addr);

		void sim_DataBusInput(uint8_t* data);
		void sim_DataBusOutput(uint8_t* data);
	};

	// Obtaining the analog value of the AUX A/B signals from the digital outputs of the generators.

	class DAC
	{
		APU* apu = nullptr;

		bool raw_mode = false;
		bool norm_mode = false;

		float auxa_mv[0x100]{};
		float auxa_norm[0x100]{};

		float auxb_mv[32768]{};
		float auxb_norm[32768]{};

		const float IntRes = 39000.f;		// Internal single MOSFET resistance
		const float IntUnloaded = 999999.f;	// Non-loaded DAC internal resistance
		const float ExtRes = 100.f;		// On-board pull-down resistor to GND
		const float Vdd = 5.0f;

		float AUX_A_Resistance(size_t sqa, size_t sqb);
		float AUX_B_Resistance(size_t tri, size_t rnd, size_t dmc);

		void gen_DACTabs();

		void sim_DACA(AudioOutSignal& aout);
		void sim_DACB(AudioOutSignal& aout);

	public:
		DAC(APU* parent);
		~DAC();

		void sim(AudioOutSignal& AUX);

		void SetRAWOutput(bool enable);

		void SetNormalizedOutput(bool enable);
	};

	class APU
	{
		friend CoreBinding;
		friend CLKGen;
		friend RegsDecoder;
		friend LengthCounter;
		friend EnvelopeUnit;
		friend DpcmChan;
		friend NoiseChan;
		friend SquareChan;
		friend TriangleChan;
		friend DMA;
		friend Pads;
		friend DAC;

		/// <summary>
		/// Internal auxiliary and intermediate connections.
		/// </summary>
		struct InternalWires
		{
			BaseLogic::TriState n_CLK;
			BaseLogic::TriState PHI0;
			BaseLogic::TriState PHI1;
			BaseLogic::TriState PHI2;
			BaseLogic::TriState RDY;			// To core
			BaseLogic::TriState RDY2;			// Default 1 (2A03)
			BaseLogic::TriState nACLK2;
			BaseLogic::TriState ACLK1;
			BaseLogic::TriState RES;
			BaseLogic::TriState n_M2;
			BaseLogic::TriState n_NMI;
			BaseLogic::TriState n_IRQ;
			BaseLogic::TriState INT;
			BaseLogic::TriState n_LFO1;
			BaseLogic::TriState n_LFO2;
			BaseLogic::TriState RnW;			// From core
			BaseLogic::TriState SPR_CPU;
			BaseLogic::TriState SPR_PPU;
			BaseLogic::TriState RW;				// To pad
			BaseLogic::TriState RD;				// To DataBus pads
			BaseLogic::TriState WR;				// To DataBus pads
			BaseLogic::TriState SYNC;			// From core

			BaseLogic::TriState n_DMC_AB;
			BaseLogic::TriState RUNDMC;
			BaseLogic::TriState DMCINT;
			BaseLogic::TriState DMCRDY;

			// RegOps
			BaseLogic::TriState n_R4015;
			BaseLogic::TriState n_R4016;
			BaseLogic::TriState n_R4017;
			BaseLogic::TriState n_R4018;
			BaseLogic::TriState n_R4019;
			BaseLogic::TriState n_R401A;
			BaseLogic::TriState W4000;
			BaseLogic::TriState W4001;
			BaseLogic::TriState W4002;
			BaseLogic::TriState W4003;
			BaseLogic::TriState W4004;
			BaseLogic::TriState W4005;
			BaseLogic::TriState W4006;
			BaseLogic::TriState W4007;
			BaseLogic::TriState W4008;
			BaseLogic::TriState W400A;
			BaseLogic::TriState W400B;
			BaseLogic::TriState W400C;
			BaseLogic::TriState W400E;
			BaseLogic::TriState W400F;
			BaseLogic::TriState W4010;
			BaseLogic::TriState W4011;
			BaseLogic::TriState W4012;
			BaseLogic::TriState W4013;
			BaseLogic::TriState W4014;
			BaseLogic::TriState W4015;
			BaseLogic::TriState W4016;
			BaseLogic::TriState W4017;
			BaseLogic::TriState W401A;

			BaseLogic::TriState SQA_LC;
			BaseLogic::TriState SQB_LC;
			BaseLogic::TriState TRI_LC;
			BaseLogic::TriState RND_LC;
			BaseLogic::TriState NOSQA;
			BaseLogic::TriState NOSQB;
			BaseLogic::TriState NOTRI;
			BaseLogic::TriState NORND;

			// Auxiliary signals associated with the Test mode, which seems to be present only on 2A03.
			BaseLogic::TriState DBG;			// from pad
			BaseLogic::TriState n_DBGRD;		// from regs decoder
			BaseLogic::TriState LOCK;
		} wire{};

		Revision rev = Revision::Unknown;
		ChipFeature fx = ChipFeature::ByRevision;

		// Instances of internal APU modules, including the core

		M6502Core::M6502* core = nullptr;
		CoreBinding* core_int = nullptr;
		CLKGen* clkgen = nullptr;
		RegsDecoder* regs = nullptr;
		LengthCounter* lc[4]{};
		DpcmChan* dpcm = nullptr;
		NoiseChan* noise = nullptr;
		SquareChan* square[2]{};
		TriangleChan* tri = nullptr;
		DMA* dma = nullptr;
		Pads* pads = nullptr;
		DAC* dac = nullptr;

		// Internal buses.

		uint8_t DB = 0;
		bool DB_Dirty = false;

		uint16_t DMC_Addr{};
		uint16_t SPR_Addr{};
		uint16_t CPU_Addr{};		// Core to mux & regs predecoder
		uint16_t Ax{};				// Mux to pads & regs decoder

		// DAC Inputs

		BaseLogic::TriState SQA_Out[4]{};
		BaseLogic::TriState SQB_Out[4]{};
		BaseLogic::TriState TRI_Out[4]{};
		BaseLogic::TriState RND_Out[4]{};
		BaseLogic::TriState DMC_Out[8]{};		// msb is not used. This is done for the convenience of packing the value in byte.

		BaseLogic::TriState GetDBBit(size_t n);
		void SetDBBit(size_t n, BaseLogic::TriState bit_val);

		size_t aclk_counter = 0;
		size_t phi_counter = 0;

		BaseLogic::TriState PrevPHI_Core = BaseLogic::TriState::X;	// to optimize
		BaseLogic::TriState PrevPHI_SoundGen = BaseLogic::TriState::X;	// to optimize

		void sim_CoreIntegration();
		void sim_SoundGenerators();

	public:
		APU(M6502Core::M6502* core, Revision rev, ChipFeature features = ChipFeature::ByRevision);
		~APU();

		/// <summary>
		/// Simulate one half cycle
		/// </summary>
		void sim(BaseLogic::TriState inputs[], BaseLogic::TriState outputs[], uint8_t* data, uint16_t* addr, AudioOutSignal& AUX);

		/// <summary>
		/// Turn on the digital output, instead of the analog DAC levels.
		/// </summary>
		/// <param name="enable"></param>
		void SetRAWOutput(bool enable);

		/// <summary>
		/// Turn on the normalized analog level output from the DAC.
		/// </summary>
		/// <param name="enable"></param>
		void SetNormalizedOutput(bool enable);

		/// <summary>
		/// Get the value of the ACLK cycle counter (PHI/2)
		/// </summary>
		/// <returns></returns>
		size_t GetACLKCounter();

		/// <summary>
		/// Reset the ACLK cycle counter
		/// </summary>
		void ResetACLKCounter();

		/// <summary>
		/// Get the value of the 6502 core cycle counter.
		/// </summary>
		/// <returns></returns>
		size_t GetPHICounter();

		/// <summary>
		/// Reset the 6502 core cycle counter.
		/// </summary>
		void ResetPHICounter();

		/// <summary>
		/// Get the audio signal properties of the current APU revision.
		/// </summary>
		/// <param name="features"></param>
		void GetSignalFeatures(AudioSignalFeatures& features);

		BaseLogic::TriState GetPHI2();
	};
}