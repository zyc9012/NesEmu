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
#include "StateFile.h"

Console::Console(const char* path)
{
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
  }
}

bool Console::SaveState(const char* filename) {
  try
  {
    StateFile f(filename, StateOp_Save);
    Save(&f);
    f.Close();
    log("Saved state: %s", filename);
  }
  catch (const std::exception&) {}
  return true;
}

bool Console::Save(StateFile* f) {
  f->Put(RAM, 2048);
  CPU->Save(f);
  APU->Save(f);
  PPU->Save(f);
  _Cartridge->Save(f);
  _Mapper->Save(f);
  return true;
}

bool Console::LoadState(const char* filename) {
  try
  {
    StateFile f(filename, StateOp_Load);
    Load(&f);
    f.Close();
    log("Loaded state: %s", filename);
  }
  catch (const std::exception&) {}
  return true;
}

bool Console::Load(StateFile* f) {
  f->Get(RAM, 2048);
  CPU->Load(f);
  APU->Load(f);
  PPU->Load(f);
  _Cartridge->Load(f);
  _Mapper->Load(f);
  return true;
}
