#include "audio.h"
#include "cartridge.h"
#include "controller.h"
#include "console.h"
#include "mapper.h"
#include "utils.h"
#include "cpu.h"
#include "apu.h"
#include "ppu.h"
#include "ines.h"
#include "palette.h"
#include "statefile.h"
#include <stdexcept>
#include <algorithm>

Console::Console(const std::string& path)
{
  cartridge = LoadNESFile(path);
  if (!cartridge) {
    throw std::runtime_error("Failed to load cartridge");
  }
  RAM.fill(0);
  controller1 = std::make_unique<Controller>();
  controller2 = std::make_unique<Controller>();
  mapper = CreateMapper(this);
  if (!mapper) {
    throw std::runtime_error("Failed to create mapper");
  }
  CPU = std::make_unique<Cpu>(this);
  APU = std::make_unique<Apu>(this);
  PPU = std::make_unique<Ppu>(this);
}

Console::~Console() = default;


void Console::Reset() {
  CPU->Reset();
}

int Console::Step() {
  const auto cpuCycles = CPU->Step();
  const auto ppuCycles = cpuCycles * 3;
  for (int i = 0; i < ppuCycles; i++) {
    PPU->Step();
    mapper->Step();
  }
  for (int i = 0; i < cpuCycles; i++) {
    APU->Step();
  }
  return cpuCycles;
}

int Console::StepFrame() {
  auto cpuCycles = 0;
  const auto frame = PPU->Frame;
  while (frame == PPU->Frame) {
    cpuCycles += Step();
  }
  return cpuCycles;
}

void Console::StepSeconds(double seconds) {
  auto cycles = static_cast<int>(CPUFrequency * seconds);
  while (cycles > 0) {
    cycles -= Step();
  }
}

Image* Console::Buffer() {
  return PPU->front.get();
}

uint32_t Console::BackgroundColor() const {
  return Palette[PPU->readPalette(0) % 64];
}

void Console::SetButtons1(const std::array<bool, 8>& buttons) {
  controller1->SetButtons(buttons);
}

void Console::SetButtons2(const std::array<bool, 8>& buttons) {
  controller2->SetButtons(buttons);
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

bool Console::SaveState(const std::string& filename) {
  try
  {
    StateFile f(filename, StateFileOp::Save);
    Save(&f);
    f.Close();
    log("Saved state: %s", filename.c_str());
  }
  catch (const std::exception&) { return false; }
  return true;
}

bool Console::Save(StateFile* f) {
  f->Put(RAM);
  CPU->Save(f);
  APU->Save(f);
  PPU->Save(f);
  cartridge->Save(f);
  mapper->Save(f);
  return true;
}

bool Console::LoadState(const std::string& filename) {
  try
  {
    StateFile f(filename, StateFileOp::Load);
    Load(&f);
    f.Close();
    log("Loaded state: %s", filename.c_str());
  }
  catch (const std::exception&) { return false; }
  return true;
}

bool Console::Load(StateFile* f) {
  f->Get(RAM);
  CPU->Load(f);
  APU->Load(f);
  PPU->Load(f);
  cartridge->Load(f);
  mapper->Load(f);
  return true;
}
