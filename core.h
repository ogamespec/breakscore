// The top-level API for managed applications.

#pragma once

extern "C"
{
	/// <summary>
	/// Creates a motherboard instance with all the hardware (APU/PPU/cartridge connector). The cartridge defaults to the Ejected state.
	/// </summary>
	/// <param name="boardName">Revision name of the motherboard. If the board is not supported, a `BogusBoard` will be created.</param>
	/// <param name="apu">APU revision</param>
	/// <param name="ppu">PPU revision</param>
	/// <param name="p1">The form factor of the cartridge connector.</param>
	void CreateBoard(char* boardName, char* apu, char* ppu, char* p1);

	/// <summary>
	/// Destroys the motherboard instance and all the resources it occupies.
	/// </summary>
	void DestroyBoard();

	/// <summary>
	/// Insert the cartridge. Cartridge Factory will try to fit .nes ROM into the form factor of the current motherboard's cartridge connector. In addition, a mapper instance will be created if it is supported.
	/// </summary>
	/// <param name="nesImage">.nes ROM image.</param>
	/// <param name="size">Image size (bytes)</param>
	int InsertCartridge(uint8_t* nesImage, size_t size);

	/// <summary>
	/// Free the cartridge connector from the ROM/mapper and all resources.
	/// </summary>
	void EjectCartridge();

	/// <summary>
	/// Simulate 1 half cycle of the board. The simulation of the signal edge is not supported, this is overkill.
	/// </summary>
	void Step();

	/// <summary>
	/// Make the board /RES pins = 0 for a few CLK half cycles so that the APU/PPU resets all of its internal circuits.
	/// </summary>
	void Reset();

	/// <summary>
	/// The parent application can check that the board is in the reset process and ignore the audio/video signal for that time.
	/// </summary>
	/// <returns></returns>
	bool InResetState();

	/// <summary>
	/// Get the values of the ACLK cycle counter.
	/// </summary>
	/// <returns></returns>
	size_t GetACLKCounter();

	/// <summary>
	/// Get the value of the 6502 core cycle counter (PHI Counter)
	/// </summary>
	/// <returns></returns>
	size_t GetPHICounter();

	/// <summary>
	/// Get the current resulting AUX value in normalized [0.0; 1.0] format.
	/// </summary>
	/// <returns></returns>
	void SampleAudioSignal(float* sample);

	/// <summary>
	/// Get audio signal settings that help with its rendering on the consumer side.
	/// </summary>
	/// <param name="features"></param>
	void GetApuSignalFeatures(APUSim::AudioSignalFeatures* features);

	/// <summary>
	/// Get the "pixel" counter. Keep in mind that pixels refers to an abstract entity representing the visible or invisible part of the video signal.
	/// </summary>
	/// <returns></returns>
	size_t GetPCLKCounter();

	/// <summary>
	/// Get 1 sample of the video signal.
	/// </summary>
	/// <param name="sample"></param>
	void SampleVideoSignal(PPUSim::VideoOutSignal* sample);

	/// <summary>
	/// Get the direct value from the PPU H counter.
	/// </summary>
	/// <returns></returns>	
	size_t GetHCounter();

	/// <summary>
	/// Get the direct value from the PPU V counter.
	/// </summary>
	/// <returns></returns>
	size_t GetVCounter();

	/// <summary>
	/// Get video signal settings that help with its rendering on the consumer side.
	/// </summary>
	/// <param name="features"></param>
	void GetPpuSignalFeatures(PPUSim::VideoSignalFeatures* features);

	/// <summary>
	/// Convert the raw color to RGB. Can be used for palette generation or PPU video output in RAW mode.
	/// The SYNC level (RAW.Sync) check must be done from the outside.
	/// </summary>
	void ConvertRAWToRGB(uint16_t raw, uint8_t* r, uint8_t* g, uint8_t* b);

	/// <summary>
	/// Use RAW color output. 
	/// RAW color refers to the Chroma/Luma combination that comes to the video generator and the Emphasis bit combination.
	/// </summary>
	/// <param name="enable"></param>
	void SetRAWColorMode(bool enable);

	/// <summary>
	/// Set one of the ways to decay OAM cells.
	/// </summary>
	void SetOamDecayBehavior(PPUSim::OAMDecayBehavior behavior);

	/// <summary>
	/// Set the noise for the composite video signal (in volts).
	/// </summary>
	/// <param name="volts"></param>
	void SetNoiseLevel(float volts);

	/// <summary>
	/// Create an IO instance of the device with the specified DeviceID. Return handle
	/// </summary>
	size_t IOCreateInstance(uint32_t device_id);

	/// <summary>
	/// Delete the device instance by handle.
	/// </summary>
	void IODisposeInstance(size_t handle);

	/// <summary>
	/// Connect the device to the port.
	/// </summary>
	void IOAttach(size_t port, size_t handle);

	/// <summary>
	/// Disconnect the device from the port.
	/// </summary>
	void IODetach(size_t port, size_t handle);

	/// <summary>
	/// Set the IOState state of the device. For example, the state of the controller buttons.
	/// </summary>
	void IOSetState(size_t handle, size_t io_state, uint32_t value);

	/// <summary>
	/// Get the state of the specified device IOState.
	/// </summary>
	uint32_t IOGetState(size_t handle, size_t io_state);

	/// <summary>
	/// Get the number of IO device states. For example, the number of buttons of the controller.
	/// </summary>
	size_t IOGetNumStates(size_t handle);

	/// <summary>
	/// Return the IOState name of the device.
	/// </summary>
	void IOGetStateName(size_t handle, size_t io_state, char* name, size_t name_size);
};