#include <memory.h>
#include "Audio.h"
#include "Cartridge.h"
#include "Controller.h"
#include "Console.h"
#include "utils.h"
#include "Cpu.h"
#include "Apu.h"
#include "Ppu.h"
#include "iNes.h"
#include "Palette.h"
#include "FilterChain.h"

Console::Console(const char* path)
{
	initPalette();
	_Cartridge = LoadNESFile(path);
	if (_Cartridge == nullptr) {
		throw nullptr;
	}
	RAM = new uint8_t[2048];
	memset(RAM, 0, 2048);
	Controller1 = new Controller();
	Controller2 = new Controller();
	_Mapper = CreateMapper(this);
	if (_Mapper == nullptr) {
		throw nullptr;
	}
	CPU = new Cpu(this);
	APU = new Apu(this);
	PPU = new Ppu(this);
}


Console::~Console()
{
}


void Console::Reset() {
	CPU->Reset();
}

int Console::Step() {
	auto cpuCycles = CPU->Step();
	auto ppuCycles = cpuCycles * 3;
	for (int i = 0; i < ppuCycles; i++) {
		PPU->Step();
		_Mapper->Step();
	}
	for (int i = 0; i < cpuCycles; i++) {
		APU->Step();
	}
	return cpuCycles;
}

int Console::StepFrame() {
	auto cpuCycles = 0;
	auto frame = PPU->Frame;
	while (frame == PPU->Frame) {
		cpuCycles += Step();
	}
	return cpuCycles;
}

void Console::StepSeconds(double seconds) {
	auto cycles = int(CPUFrequency * seconds);
	while (cycles > 0) {
		cycles -= Step();
	}
}

Image* Console::Buffer() {
	return PPU->front;
}

uint32_t Console::BackgroundColor() {
	return Palette[PPU->readPalette(0)%64];
}

void Console::SetButtons1(bool buttons[8]) {
	Controller1->SetButtons(buttons);
}

void Console::SetButtons2(bool buttons[8]) {
	Controller2->SetButtons(buttons);
}

void Console::SetAudioChannel(Audio* channel) {
	APU->channel = channel;
}

void Console::SetAudioSampleRate(double sampleRate) {
	if (sampleRate != 0) {
		// Convert samples per second to cpu steps per sample
		APU->sampleRate = CPUFrequency / sampleRate;
		// Initialize filters
		Filter** fs = new Filter*[3]{
			HighPassFilter(float(sampleRate), 90),
			HighPassFilter(float(sampleRate), 440),
			LowPassFilter(float(sampleRate), 14000),
		};
		APU->filterChain = new FilterChain(fs, 3);
	} else {
		APU->filterChain = nullptr;
	}
}

bool Console::SaveState(const char* filename) {
	// dir, _ := path.Split(filename)
	// if err := os.MkdirAll(dir, 0755); err != nil {
	// 	return err
	// }
	// file, err := os.Create(filename)
	// if err != nil {
	// 	return err
	// }
	// defer file.Close()
	// encoder := gob.NewEncoder(file)
	// return Save(encoder)
	return false;
}

bool Console::Save() {
	// encoder.Encode(RAM);
	// CPU->Save(encoder);
	// APU->Save(encoder);
	// PPU->Save(encoder);
	// Cartridge.Save(encoder);
	// Mapper.Save(encoder);
	// return encoder.Encode(true);
	return false;
}

bool Console::LoadState(const char* filename) {
	// file, err := os.Open(filename)
	// if err != nil {
	// 	return err
	// }
	// defer file.Close()
	// decoder := gob.NewDecoder(file)
	// return Load(decoder)
	return false;
}

bool Console::Load() {
	// decoder.Decode(&RAM);
	// CPU->Load(decoder);
	// APU->Load(decoder);
	// PPU->Load(decoder);
	// Cartridge.Load(decoder);
	// Mapper.Load(decoder);
	// bool dummy;
	// if err := decoder.Decode(&dummy); err != nil {
	// 	return err
	// }
	// return nil
	return false;
}
