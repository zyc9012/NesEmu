#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <cstdint>
#include <memory>
#include <array>
#include <string>

class Audio;
class Cpu;
class Ppu;
class Apu;
class Controller;
class Mapper;
class Image;
class Cartridge;
class StateFile;

class Console
{
public:
  explicit Console(const std::string& path);
  ~Console();
  
  Console(const Console&) = delete;
  Console& operator=(const Console&) = delete;
  Console(Console&&) = delete;
  Console& operator=(Console&&) = delete;

  void Reset();
  int Step();
  int StepFrame();
  void StepSeconds(double seconds);
  Image* Buffer();
  uint32_t BackgroundColor() const;
  void SetButtons1(const std::array<bool, 8>& buttons);
  void SetButtons2(const std::array<bool, 8>& buttons);
  void SetAudioChannel(Audio* channel);
  void SetAudioSampleRate(double sampleRate);
  bool SaveState(const std::string& filename);
  bool Save(StateFile*);
  bool LoadState(const std::string& filename);
  bool Load(StateFile*);

  std::unique_ptr<Cpu> CPU;
  std::unique_ptr<Apu> APU;
  std::unique_ptr<Ppu> PPU;
  std::unique_ptr<Cartridge> cartridge;
  std::unique_ptr<Controller> controller1;
  std::unique_ptr<Controller> controller2;
  std::unique_ptr<Mapper> mapper;
  std::array<uint8_t, 2048> RAM{};
};

#endif