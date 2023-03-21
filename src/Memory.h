#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>
class Console;

enum MirroringModes {
  MirrorHorizontal = 0,
  MirrorVertical = 1,
  MirrorSingle0 = 2,
  MirrorSingle1 = 3,
  MirrorFour = 4,
};

class Memory
{
public:
  virtual uint8_t Read(uint16_t address) = 0;
  virtual void Write(uint16_t address, uint8_t value) = 0;
  Console* console;
};

// CPU Memory Map

class CpuMemory : public Memory
{
public:
  CpuMemory(Console* console);
  ~CpuMemory();
  virtual uint8_t Read(uint16_t address) override;
  virtual void Write(uint16_t address, uint8_t value) override;
};

// PPU Memory Map

class PpuMemory : public Memory
{
public:
  PpuMemory(Console* console);
  ~PpuMemory();
  virtual uint8_t Read(uint16_t address) override;
  virtual void Write(uint16_t address, uint8_t value) override;
};

#endif