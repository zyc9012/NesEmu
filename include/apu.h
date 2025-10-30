#ifndef _APU_H
#define _APU_H

#include <cstdint>
#include "audio.h"

class Console;
class Cpu;
class FilterChain;
class StateFile;

class Pulse
{
public:
  Pulse() = default;
  ~Pulse() = default;

  bool Save(StateFile*);
  bool Load(StateFile*);

  void writeControl(uint8_t value);
  void writeSweep(uint8_t value);
  void writeTimerLow(uint8_t value);
  void writeTimerHigh(uint8_t value);
  void stepTimer();
  void stepEnvelope();
  void stepSweep();
  void stepLength();
  void sweep();
  uint8_t output() const;

  bool enabled{false};
  uint8_t channel{0};
  bool lengthEnabled{false};
  uint8_t lengthValue{0};
  uint16_t timerPeriod{0};
  uint16_t timerValue{0};
  uint8_t dutyMode{0};
  uint8_t dutyValue{0};
  bool sweepReload{false};
  bool sweepEnabled{false};
  bool sweepNegate{false};
  uint8_t sweepShift{0};
  uint8_t sweepPeriod{0};
  uint8_t sweepValue{0};
  bool envelopeEnabled{false};
  bool envelopeLoop{false};
  bool envelopeStart{false};
  uint8_t envelopePeriod{0};
  uint8_t envelopeValue{0};
  uint8_t envelopeVolume{0};
  uint8_t constantVolume{0};
};

class Triangle
{
public:
  Triangle() = default;
  ~Triangle() = default;

  bool Save(StateFile*);
  bool Load(StateFile*);

  void writeControl(uint8_t value);
  void writeTimerLow(uint8_t value);
  void writeTimerHigh(uint8_t value);
  void stepTimer();
  void stepCounter();
  void stepLength();
  uint8_t output() const;

  bool enabled{false};
  bool lengthEnabled{false};
  uint8_t lengthValue{0};
  uint16_t timerPeriod{0};
  uint16_t timerValue{0};
  uint8_t dutyValue{0};
  uint8_t counterPeriod{0};
  uint8_t counterValue{0};
  bool counterReload{false};
};

class Noise {
public:
  Noise() = default;
  ~Noise() = default;

  bool Save(StateFile*);
  bool Load(StateFile*);

  void writeControl(uint8_t value);
  void stepTimer();
  void writePeriod(uint8_t value);
  void writeLength(uint8_t value);
  void stepLength();
  void stepEnvelope();
  uint8_t output() const;

  bool enabled{false};
  bool mode{false};
  uint16_t shiftRegister{0};
  bool lengthEnabled{false};
  uint8_t lengthValue{0};
  uint16_t timerPeriod{0};
  uint16_t timerValue{0};
  bool envelopeEnabled{false};
  bool envelopeLoop{false};
  bool envelopeStart{false};
  uint8_t envelopePeriod{0};
  uint8_t envelopeValue{0};
  uint8_t envelopeVolume{0};
  uint8_t constantVolume{0};
};

class DMC {
public:
  DMC() = default;
  ~DMC() = default;

  bool Save(StateFile*);
  bool Load(StateFile*);

  void writeControl(uint8_t value);
  void writeValue(uint8_t value);
  void writeAddress(uint8_t value);
  void writeLength(uint8_t value);
  void restart();
  void stepTimer();
  void stepReader();
  void stepShifter();
  uint8_t output() const;

  Cpu* cpu{nullptr};
  bool enabled{false};
  uint8_t value{0};
  uint16_t sampleAddress{0};
  uint16_t sampleLength{0};
  uint16_t currentAddress{0};
  uint16_t currentLength{0};
  uint8_t shiftRegister{0};
  uint8_t bitCount{0};
  uint8_t tickPeriod{0};
  uint8_t tickValue{0};
  bool loop{false};
  bool irq{false};
};

class Apu
{
public:
  explicit Apu(Console* console);
  ~Apu();
  
  Apu(const Apu&) = delete;
  Apu& operator=(const Apu&) = delete;
  Apu(Apu&&) = delete;
  Apu& operator=(Apu&&) = delete;

  bool Save(StateFile*);
  bool Load(StateFile*);

  void Step();
  void sendSample();
  float output() const;
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

  Console* console{nullptr};
  Audio* channel{nullptr};
  double sampleRate{0.0};
  Pulse pulse1;
  Pulse pulse2;
  Triangle triangle;
  Noise noise;
  DMC dmc;
  uint64_t cycle{0};
  uint8_t framePeriod{0};
  uint8_t frameValue{0};
  bool frameIRQ{false};
};

#endif