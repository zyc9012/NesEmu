#pragma once
#include <stdint.h>
#include "Audio.h"
class Console;
class Cpu;
class FilterChain;

class Pulse
{
public:
	Pulse() {}
	~Pulse() {}

	void writeControl(uint8_t value);
	void writeSweep(uint8_t value);
	void writeTimerLow(uint8_t value);
	void writeTimerHigh(uint8_t value);
	void stepTimer();
	void stepEnvelope();
	void stepSweep();
	void stepLength();
	void sweep();
	uint8_t output();

	bool enabled;
	uint8_t channel;
	bool lengthEnabled;
	uint8_t lengthValue;
	uint16_t timerPeriod;
	uint16_t timerValue;
	uint8_t dutyMode;
	uint8_t dutyValue;
	bool sweepReload;
	bool sweepEnabled;
	bool sweepNegate;
	uint8_t sweepShift;
	uint8_t sweepPeriod;
	uint8_t sweepValue;
	bool envelopeEnabled;
	bool envelopeLoop;
	bool envelopeStart;
	uint8_t envelopePeriod;
	uint8_t envelopeValue;
	uint8_t envelopeVolume;
	uint8_t constantVolume;
};

class Triangle
{
public:
	Triangle() {}
	~Triangle() {}

	void writeControl(uint8_t value);
	void writeTimerLow(uint8_t value);
	void writeTimerHigh(uint8_t value);
	void stepTimer();
	void stepCounter();
	void stepLength();
	uint8_t output();

	bool enabled;
	bool lengthEnabled;
	uint8_t lengthValue;
	uint16_t timerPeriod;
	uint16_t timerValue;
	uint8_t dutyValue;
	uint8_t counterPeriod;
	uint8_t counterValue;
	bool counterReload;
};

class Noise {
public:
	Noise() {}
	~Noise() {}

	void writeControl(uint8_t value);
	void stepTimer();
	void writePeriod(uint8_t value);
	void writeLength(uint8_t value);
	void stepLength();
	void stepEnvelope();
	uint8_t output();

	bool enabled;
	bool mode;
	uint16_t shiftRegister;
	bool lengthEnabled;
	uint8_t lengthValue;
	uint16_t timerPeriod;
	uint16_t timerValue;
	bool envelopeEnabled;
	bool envelopeLoop;
	bool envelopeStart;
	uint8_t envelopePeriod;
	uint8_t envelopeValue;
	uint8_t envelopeVolume;
	uint8_t constantVolume;
};

class DMC {
public:
	DMC() {}
	~DMC() {}

	void writeControl(uint8_t value);
	void writeValue(uint8_t value);
	void writeAddress(uint8_t value);
	void writeLength(uint8_t value);
	void restart();
	void stepTimer();
	void stepReader();
	void stepShifter();
	uint8_t output();

	Cpu* cpu;
	bool enabled;
	uint8_t value;
	uint16_t sampleAddress;
	uint16_t sampleLength;
	uint16_t currentAddress;
	uint16_t currentLength;
	uint8_t shiftRegister;
	uint8_t bitCount;
	uint8_t tickPeriod;
	uint8_t tickValue;
	bool loop;
	bool irq;
};

class Apu
{
public:
	Apu(Console* console);
	~Apu();

	void Step();
	void sendSample();
	float output();
	void stepFrameCounter();
	void stepTimer();
	void stepEnvelope();
	void stepSweep();
	void stepLength();
	void fireIRQ();
	uint8_t readRegister(uint16_t address);
	void writeRegister(uint16_t address, uint8_t value);
	uint8_t readStatus();
	void writeControl(uint8_t value);
	void writeFrameCounter(uint8_t value);

	Console* console;
	Audio* channel;
	double sampleRate;
	Pulse pulse1;
	Pulse pulse2;
	Triangle triangle;
	Noise noise;
	DMC dmc;
	uint64_t cycle;
	uint8_t framePeriod;
	uint8_t frameValue;
	bool frameIRQ;
	FilterChain* filterChain;
};

