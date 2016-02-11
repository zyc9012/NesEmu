#pragma once
#include <stdint.h>
#include "BlockingQueue.h"
class Cpu;
class Ppu;
class Apu;
class Controller;
class Mapper;
class Image;
#include "Cartridge.h"

class Console
{
public:
	Console(const char* path);
	~Console();

	void Reset();
	int Step();
	int StepFrame();
	void StepSeconds(double seconds);
	Image* Buffer();
	uint32_t BackgroundColor();
	void SetButtons1(bool buttons[8]);
	void SetButtons2(bool buttons[8]);
	void SetAudioChannel(BlockingQueue<float>* channel); // TODO chan
	void SetAudioSampleRate(double sampleRate);
	bool SaveState(const char* filename);
	bool Save();
	bool LoadState(const char* filename);
	bool Load();

	Cpu* CPU;
	Apu* APU;
	Ppu* PPU;
	Cartridge* _Cartridge;
	Controller*	Controller1;
	Controller*	Controller2;
	Mapper* _Mapper;
	uint8_t* RAM;
};
