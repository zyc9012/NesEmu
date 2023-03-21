#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <stdint.h>
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
  void SetAudioChannel(Audio* channel);
  void SetAudioSampleRate(double sampleRate);
  bool SaveState(const char* filename);
  bool Save(StateFile*);
  bool LoadState(const char* filename);
  bool Load(StateFile*);

  Cpu* CPU;
  Apu* APU;
  Ppu* PPU;
  Cartridge* _Cartridge;
  Controller*	Controller1;
  Controller*	Controller2;
  Mapper* _Mapper;
  uint8_t* RAM;
};

#endif