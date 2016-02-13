#include "Apu.h"
#include "iNes.h"
#include "Console.h"
#include "Cpu.h"
#include "utils.h"
#include "FilterChain.h"

double frameCounterRate = CPUFrequency / 240.0;

uint8_t lengthTable[] = {
	10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
	12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30,
};

uint8_t dutyTable[4][8] = {
	{0, 1, 0, 0, 0, 0, 0, 0},
	{0, 1, 1, 0, 0, 0, 0, 0},
	{0, 1, 1, 1, 1, 0, 0, 0},
	{1, 0, 0, 1, 1, 1, 1, 1},
};

uint8_t triangleTable[] = {
	15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
};

uint16_t noiseTable[] = {
	4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068,
};

uint8_t dmcTable[] = {
	214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27,
};

float pulseTable [31];
float tndTable [203];

void initApu() {
	for (int i = 0; i < 31; i++) {
		pulseTable[i] = float(95.52 / (8128.0/i + 100));
	}
	for (int i = 0; i < 203; i++) {
		tndTable[i] = float(163.67 / (24329.0/i + 100));
	}
}

// APU

Apu::Apu(Console* console) {
	initApu();
	this->console = console;
	noise.shiftRegister = 1;
	pulse1.channel = 1;
	pulse2.channel = 2;
	dmc.cpu = console->CPU;
}

Apu::~Apu()
{
}

// void Apu::Save(encoder *gob.Encoder) error {
// 	encoder.Encode(cycle)
// 	encoder.Encode(framePeriod)
// 	encoder.Encode(frameValue)
// 	encoder.Encode(frameIRQ)
// 	pulse1.Save(encoder)
// 	pulse2.Save(encoder)
// 	triangle.Save(encoder)
// 	noise.Save(encoder)
// 	dmc.Save(encoder)
// 	return nil
// }

// void Apu::Load(decoder *gob.Decoder) error {
// 	decoder.Decode(&cycle)
// 	decoder.Decode(&framePeriod)
// 	decoder.Decode(&frameValue)
// 	decoder.Decode(&frameIRQ)
// 	pulse1.Load(decoder)
// 	pulse2.Load(decoder)
// 	triangle.Load(decoder)
// 	noise.Load(decoder)
// 	dmc.Load(decoder)
// 	return nil
// }

void Apu::Step() {
	auto cycle1 = cycle;
	cycle++;
	auto cycle2 = cycle;
	stepTimer();
	auto f1 = int(double(cycle1) / frameCounterRate);
	auto f2 = int(double(cycle2) / frameCounterRate);
	if (f1 != f2) {
		stepFrameCounter();
	}
	auto s1 = int(double(cycle1) / sampleRate);
	auto s2 = int(double(cycle2) / sampleRate);
	if (s1 != s2) {
		sendSample();
	}
}

void Apu::sendSample() {
	auto ot = output();
	//ot = filterChain->Step(ot);
	channel->Push(ot);
}

float Apu::output() {
	auto p1 = pulse1.output();
	auto p2 = pulse2.output();
	auto t = triangle.output();
	auto n = noise.output();
	auto d = dmc.output();
	auto pulseOut = pulseTable[p1+p2];
	auto tndOut = tndTable[3*t+2*n+d];
	return pulseOut + tndOut;
}

// mode 0:    mode 1:       function
// ---------  -----------  -----------------------------
//  - - - f    - - - - -    IRQ (if bit 6 is clear)
//  - l - l    l - l - -    Length counter and sweep
//  e e e e    e e e e -    Envelope and linear counter
void Apu::stepFrameCounter() {
	switch (framePeriod) {
		case 4:
			frameValue = (frameValue + 1) % 4;
			switch (frameValue) {
				case 0:
				case 2:
					stepEnvelope();
					break;
				case 1:
					stepEnvelope();
					stepSweep();
					stepLength();
					break;
				case 3:
					stepEnvelope();
					stepSweep();
					stepLength();
					fireIRQ();
					break;
			}
			break;
		case 5:
			frameValue = (frameValue + 1) % 5;
			switch (frameValue) {
				case 1:
				case 3:
					stepEnvelope();
					break;
				case 0:
				case 2:
					stepEnvelope();
					stepSweep();
					stepLength();
					break;
			}
			break;
		}
}

void Apu::stepTimer() {
	if (cycle%2 == 0) {
		pulse1.stepTimer();
		pulse2.stepTimer();
		noise.stepTimer();
		dmc.stepTimer();
	}
	triangle.stepTimer();
}

void Apu::stepEnvelope() {
	pulse1.stepEnvelope();
	pulse2.stepEnvelope();
	triangle.stepCounter();
	noise.stepEnvelope();
}

void Apu::stepSweep() {
	pulse1.stepSweep();
	pulse2.stepSweep();
}

void Apu::stepLength() {
	pulse1.stepLength();
	pulse2.stepLength();
	triangle.stepLength();
	noise.stepLength();
}

void Apu::fireIRQ() {
	if (frameIRQ) {
		console->CPU->triggerIRQ();
	}
}

uint8_t Apu::readRegister(uint16_t address) {
	switch (address) {
		case 0x4015:
			return readStatus();
		default:
			log("unhandled apu register read at address: 0x%04X", address);
	}
	return 0;
}

void Apu::writeRegister(uint16_t address, uint8_t value) {
	switch (address) {
	case 0x4000:
		pulse1.writeControl(value);
		break;
	case 0x4001:
		pulse1.writeSweep(value);
		break;
	case 0x4002:
		pulse1.writeTimerLow(value);
		break;
	case 0x4003:
		pulse1.writeTimerHigh(value);
		break;
	case 0x4004:
		pulse2.writeControl(value);
		break;
	case 0x4005:
		pulse2.writeSweep(value);
		break;
	case 0x4006:
		pulse2.writeTimerLow(value);
		break;
	case 0x4007:
		pulse2.writeTimerHigh(value);
		break;
	case 0x4008:
		triangle.writeControl(value);
		break;
	case 0x4009:
	case 0x4010:
		dmc.writeControl(value);
		break;
	case 0x4011:
		dmc.writeValue(value);
		break;
	case 0x4012:
		dmc.writeAddress(value);
		break;
	case 0x4013:
		dmc.writeLength(value);
		break;
	case 0x400A:
		triangle.writeTimerLow(value);
		break;
	case 0x400B:
		triangle.writeTimerHigh(value);
		break;
	case 0x400C:
		noise.writeControl(value);
		break;
	case 0x400D:
	case 0x400E:
		noise.writePeriod(value);
		break;
	case 0x400F:
		noise.writeLength(value);
		break;
	case 0x4015:
		writeControl(value);
		break;
	case 0x4017:
		writeFrameCounter(value);
		break;
	default:
		log("unhandled apu register write at address: 0x%04X", address);
	}
}

uint8_t Apu::readStatus() {
	uint8_t result = 0;
	if (pulse1.lengthValue > 0) {
		result |= 1;
	}
	if (pulse2.lengthValue > 0) {
		result |= 2;
	}
	if (triangle.lengthValue > 0) {
		result |= 4;
	}
	if (noise.lengthValue > 0) {
		result |= 8;
	}
	if (dmc.currentLength > 0) {
		result |= 16;
	}
	return result;
}

void Apu::writeControl(uint8_t value) {
	pulse1.enabled = (value&1) == 1;
	pulse2.enabled = (value&2) == 2;
	triangle.enabled = (value&4) == 4;
	noise.enabled = (value&8) == 8;
	dmc.enabled = (value&16) == 16;
	if (!pulse1.enabled) {
		pulse1.lengthValue = 0;
	}
	if (!pulse2.enabled) {
		pulse2.lengthValue = 0;
	}
	if (!triangle.enabled) {
		triangle.lengthValue = 0;
	}
	if (!noise.enabled) {
		noise.lengthValue = 0;
	}
	if (!dmc.enabled) {
		dmc.currentLength = 0;
	} else {
		if (dmc.currentLength == 0) {
			dmc.restart();
		}
	}
}

void Apu::writeFrameCounter(uint8_t value) {
	framePeriod = 4 + ((value>>7)&1);
	frameIRQ = ((value>>6)&1) == 0;
	// frameValue = 0
	// if framePeriod == 5 {
	// 	stepEnvelope()
	// 	stepSweep()
	// 	stepLength()
	// }
}

// Pulse

// void Pulse::Save(encoder *gob.Encoder) error {
// 	encoder.Encode(enabled)
// 	encoder.Encode(channel)
// 	encoder.Encode(lengthEnabled)
// 	encoder.Encode(lengthValue)
// 	encoder.Encode(timerPeriod)
// 	encoder.Encode(timerValue)
// 	encoder.Encode(dutyMode)
// 	encoder.Encode(dutyValue)
// 	encoder.Encode(sweepReload)
// 	encoder.Encode(sweepEnabled)
// 	encoder.Encode(sweepNegate)
// 	encoder.Encode(sweepShift)
// 	encoder.Encode(sweepPeriod)
// 	encoder.Encode(sweepValue)
// 	encoder.Encode(envelopeEnabled)
// 	encoder.Encode(envelopeLoop)
// 	encoder.Encode(envelopeStart)
// 	encoder.Encode(envelopePeriod)
// 	encoder.Encode(envelopeValue)
// 	encoder.Encode(envelopeVolume)
// 	encoder.Encode(constantVolume)
// 	return nil
// }

// void Pulse::Load(decoder *gob.Decoder) error {
// 	decoder.Decode(&enabled)
// 	decoder.Decode(&channel)
// 	decoder.Decode(&lengthEnabled)
// 	decoder.Decode(&lengthValue)
// 	decoder.Decode(&timerPeriod)
// 	decoder.Decode(&timerValue)
// 	decoder.Decode(&dutyMode)
// 	decoder.Decode(&dutyValue)
// 	decoder.Decode(&sweepReload)
// 	decoder.Decode(&sweepEnabled)
// 	decoder.Decode(&sweepNegate)
// 	decoder.Decode(&sweepShift)
// 	decoder.Decode(&sweepPeriod)
// 	decoder.Decode(&sweepValue)
// 	decoder.Decode(&envelopeEnabled)
// 	decoder.Decode(&envelopeLoop)
// 	decoder.Decode(&envelopeStart)
// 	decoder.Decode(&envelopePeriod)
// 	decoder.Decode(&envelopeValue)
// 	decoder.Decode(&envelopeVolume)
// 	decoder.Decode(&constantVolume)
// 	return nil
// }

void Pulse::writeControl(uint8_t value) {
	dutyMode = (value >> 6) & 3;
	lengthEnabled = ((value>>5)&1) == 0;
	envelopeLoop = ((value>>5)&1) == 1;
	envelopeEnabled = ((value>>4)&1) == 0;
	envelopePeriod = value & 15;
	constantVolume = value & 15;
	envelopeStart = true;
}

void Pulse::writeSweep(uint8_t value) {
	sweepEnabled = ((value>>7)&1) == 1;
	sweepPeriod = (value >> 4) & 7;
	sweepNegate = ((value>>3)&1) == 1;
	sweepShift = value & 7;
	sweepReload = true;
}

void Pulse::writeTimerLow(uint8_t value) {
	timerPeriod = (timerPeriod & 0xFF00) | uint16_t(value);
}

void Pulse::writeTimerHigh(uint8_t value) {
	lengthValue = lengthTable[value>>3];
	timerPeriod = (timerPeriod & 0x00FF) | (uint16_t(value&7) << 8);
	envelopeStart = true;
	dutyValue = 0;
}

void Pulse::stepTimer() {
	if (timerValue == 0) {
		timerValue = timerPeriod;
		dutyValue = (dutyValue + 1) % 8;
	} else {
		timerValue--;
	}
}

void Pulse::stepEnvelope() {
	if (envelopeStart) {
		envelopeVolume = 15;
		envelopeValue = envelopePeriod;
		envelopeStart = false;
	} else if (envelopeValue > 0) {
		envelopeValue--;
	} else {
		if (envelopeVolume > 0) {
			envelopeVolume--;
		} else if (envelopeLoop) {
			envelopeVolume = 15;
		}
		envelopeValue = envelopePeriod;
	}
}

void Pulse::stepSweep() {
	if (sweepReload) {
		if (sweepEnabled && sweepValue == 0) {
			sweep();
		}
		sweepValue = sweepPeriod;
		sweepReload = false;
	} else if (sweepValue > 0) {
		sweepValue--;
	} else {
		if (sweepEnabled) {
			sweep();
		}
		sweepValue = sweepPeriod;
	}
}

void Pulse::stepLength() {
	if (lengthEnabled && lengthValue > 0) {
		lengthValue--;
	}
}

void Pulse::sweep() {
	auto delta = timerPeriod >> sweepShift;
	if (sweepNegate) {
		timerPeriod -= delta;
		if (channel == 1) {
			timerPeriod--;
		}
	} else {
		timerPeriod += delta;
	}
}

uint8_t Pulse::output() {
	if (!enabled) {
		return 0;
	}
	if (lengthValue == 0) {
		return 0;
	}
	if (dutyTable[dutyMode][dutyValue] == 0) {
		return 0;
	}
	if (timerPeriod < 8 || timerPeriod > 0x7FF) {
		return 0;
	}
	// if !sweepNegate && timerPeriod+(timerPeriod>>sweepShift) > 0x7FF {
	// 	return 0
	// }
	if (envelopeEnabled) {
		return envelopeVolume;
	} else {
		return constantVolume;
	}
}

// Triangle



// void Triangle::Save(encoder *gob.Encoder) error {
// 	encoder.Encode(enabled)
// 	encoder.Encode(lengthEnabled)
// 	encoder.Encode(lengthValue)
// 	encoder.Encode(timerPeriod)
// 	encoder.Encode(timerValue)
// 	encoder.Encode(dutyValue)
// 	encoder.Encode(counterPeriod)
// 	encoder.Encode(counterValue)
// 	encoder.Encode(counterReload)
// 	return nil
// }

// void Triangle::Load(decoder *gob.Decoder) error {
// 	decoder.Decode(&enabled)
// 	decoder.Decode(&lengthEnabled)
// 	decoder.Decode(&lengthValue)
// 	decoder.Decode(&timerPeriod)
// 	decoder.Decode(&timerValue)
// 	decoder.Decode(&dutyValue)
// 	decoder.Decode(&counterPeriod)
// 	decoder.Decode(&counterValue)
// 	decoder.Decode(&counterReload)
// 	return nil
// }

void Triangle::writeControl(uint8_t value) {
	lengthEnabled = ((value>>7)&1) == 0;
	counterPeriod = value & 0x7F;
}

void Triangle::writeTimerLow(uint8_t value) {
	timerPeriod = (timerPeriod & 0xFF00) | uint16_t(value);
}

void Triangle::writeTimerHigh(uint8_t value) {
	lengthValue = lengthTable[value>>3];
	timerPeriod = (timerPeriod & 0x00FF) | (uint16_t(value&7) << 8);
	timerValue = timerPeriod;
	counterReload = true;
}

void Triangle::stepTimer() {
	if (timerValue == 0) {
		timerValue = timerPeriod;
		if (lengthValue > 0 && counterValue > 0) {
			dutyValue = (dutyValue + 1) % 32;
		}
	} else {
		timerValue--;
	}
}

void Triangle::stepLength() {
	if (lengthEnabled && lengthValue > 0) {
		lengthValue--;
	}
}

void Triangle::stepCounter() {
	if (counterReload) {
		counterValue = counterPeriod;
	} else if (counterValue > 0) {
		counterValue--;
	}
	if (lengthEnabled) {
		counterReload = false;
	}
}

uint8_t Triangle::output() {
	if (!enabled) {
		return 0;
	}
	if (lengthValue == 0) {
		return 0;
	}
	if (counterValue == 0) {
		return 0;
	}
	return triangleTable[dutyValue];
}

// Noise

// void Noise::Save(encoder *gob.Encoder) error {
// 	encoder.Encode(enabled)
// 	encoder.Encode(mode)
// 	encoder.Encode(shiftRegister)
// 	encoder.Encode(lengthEnabled)
// 	encoder.Encode(lengthValue)
// 	encoder.Encode(timerPeriod)
// 	encoder.Encode(timerValue)
// 	encoder.Encode(envelopeEnabled)
// 	encoder.Encode(envelopeLoop)
// 	encoder.Encode(envelopeStart)
// 	encoder.Encode(envelopePeriod)
// 	encoder.Encode(envelopeValue)
// 	encoder.Encode(envelopeVolume)
// 	encoder.Encode(constantVolume)
// 	return nil
// }

// void Noise::Load(decoder *gob.Decoder) error {
// 	decoder.Decode(&enabled)
// 	decoder.Decode(&mode)
// 	decoder.Decode(&shiftRegister)
// 	decoder.Decode(&lengthEnabled)
// 	decoder.Decode(&lengthValue)
// 	decoder.Decode(&timerPeriod)
// 	decoder.Decode(&timerValue)
// 	decoder.Decode(&envelopeEnabled)
// 	decoder.Decode(&envelopeLoop)
// 	decoder.Decode(&envelopeStart)
// 	decoder.Decode(&envelopePeriod)
// 	decoder.Decode(&envelopeValue)
// 	decoder.Decode(&envelopeVolume)
// 	decoder.Decode(&constantVolume)
// 	return nil
// }

void Noise::writeControl(uint8_t value) {
	lengthEnabled = ((value>>5)&1) == 0;
	envelopeLoop = ((value>>5)&1) == 1;
	envelopeEnabled = ((value>>4)&1) == 0;
	envelopePeriod = value & 15;
	constantVolume = value & 15;
	envelopeStart = true;
}

void Noise::writePeriod(uint8_t value) {
	mode = (value&0x80) == 0x80;
	timerPeriod = noiseTable[value&0x0F];
}

void Noise::writeLength(uint8_t value) {
	lengthValue = lengthTable[value>>3];
	envelopeStart = true;
}

void Noise::stepTimer() {
	if (timerValue == 0) {
		timerValue = timerPeriod;
		uint8_t shift;
		if (mode) {
			shift = 6;
		} else {
			shift = 1;
		}
		auto b1 = shiftRegister & 1;
		auto b2 = (shiftRegister >> shift) & 1;
		shiftRegister >>= 1;
		shiftRegister |= (b1 ^ b2) << 14;
	} else {
		timerValue--;
	}
}

void Noise::stepEnvelope() {
	if (envelopeStart) {
		envelopeVolume = 15;
		envelopeValue = envelopePeriod;
		envelopeStart = false;
	} else if (envelopeValue > 0) {
		envelopeValue--;
	} else {
		if (envelopeVolume > 0) {
			envelopeVolume--;
		} else if (envelopeLoop) {
			envelopeVolume = 15;
		}
		envelopeValue = envelopePeriod;
	}
}

void Noise::stepLength() {
	if (lengthEnabled && lengthValue > 0) {
		lengthValue--;
	}
}

uint8_t Noise::output() {
	if (!enabled) {
		return 0;
	}
	if (lengthValue == 0) {
		return 0;
	}
	if ((shiftRegister&1) == 1) {
		return 0;
	}
	if (envelopeEnabled) {
		return envelopeVolume;
	} else {
		return constantVolume;
	}
}

// DMC


// void DMC::Save(encoder *gob.Encoder) error {
// 	encoder.Encode(enabled)
// 	encoder.Encode(value)
// 	encoder.Encode(sampleAddress)
// 	encoder.Encode(sampleLength)
// 	encoder.Encode(currentAddress)
// 	encoder.Encode(currentLength)
// 	encoder.Encode(shiftRegister)
// 	encoder.Encode(bitCount)
// 	encoder.Encode(tickPeriod)
// 	encoder.Encode(tickValue)
// 	encoder.Encode(loop)
// 	encoder.Encode(irq)
// 	return nil
// }

// void DMC::Load(decoder *gob.Decoder) error {
// 	decoder.Decode(&enabled)
// 	decoder.Decode(&value)
// 	decoder.Decode(&sampleAddress)
// 	decoder.Decode(&sampleLength)
// 	decoder.Decode(&currentAddress)
// 	decoder.Decode(&currentLength)
// 	decoder.Decode(&shiftRegister)
// 	decoder.Decode(&bitCount)
// 	decoder.Decode(&tickPeriod)
// 	decoder.Decode(&tickValue)
// 	decoder.Decode(&loop)
// 	decoder.Decode(&irq)
// 	return nil
// }

void DMC::writeControl(uint8_t value) {
	irq = (value&0x80) == 0x80;
	loop = (value&0x40) == 0x40;
	tickPeriod = dmcTable[value&0x0F];
}

void DMC::writeValue(uint8_t value) {
	value = value & 0x7F;
}

void DMC::writeAddress(uint8_t value) {
	// Sample address = %11AAAAAA.AA000000
	sampleAddress = 0xC000 | (uint16_t(value) << 6);
}

void DMC::writeLength(uint8_t value) {
	// Sample length = %0000LLLL.LLLL0001
	sampleLength = (uint16_t(value) << 4) | 1;
}

void DMC::restart() {
	currentAddress = sampleAddress;
	currentLength = sampleLength;
}

void DMC::stepTimer() {
	if (!enabled) {
		return;
	}
	stepReader();
	if (tickValue == 0) {
		tickValue = tickPeriod;
		stepShifter();
	} else {
		tickValue--;
	}
}

void DMC::stepReader() {
	if (currentLength > 0 && bitCount == 0) {
		cpu->stall += 4;
		shiftRegister = cpu->Read(currentAddress);
		bitCount = 8;
		currentAddress++;
		if (currentAddress == 0) {
			currentAddress = 0x8000;
		}
		currentLength--;
		if (currentLength == 0 && loop) {
			restart();
		}
	}
}

void DMC::stepShifter() {
	if (bitCount == 0) {
		return;
	}
	if ((shiftRegister&1) == 1) {
		if (value <= 125) {
			value += 2;
		}
	} else {
		if (value >= 2) {
			value -= 2;
		}
	}
	shiftRegister >>= 1;
	bitCount--;
}

uint8_t DMC::output() {
	return value;
}
