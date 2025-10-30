#ifndef _MEMORY_H
#define _MEMORY_H

#include <cstdint>

class Console;

enum class MirroringMode : uint8_t {
  Horizontal = 0,
  Vertical = 1,
  Single0 = 2,
  Single1 = 3,
  Four = 4,
};

class Memory
{
public:
  virtual ~Memory() = default;
  virtual uint8_t Read(uint16_t address) = 0;
  virtual void Write(uint16_t address, uint8_t value) = 0;
  Console* console{nullptr};
};

// CPU Memory Map

class CpuMemory : public Memory
{
public:
  explicit CpuMemory(Console* console);
  ~CpuMemory() override = default;
  uint8_t Read(uint16_t address) override;
  void Write(uint16_t address, uint8_t value) override;
};

// PPU Memory Map

class PpuMemory : public Memory
{
public:
  explicit PpuMemory(Console* console);
  ~PpuMemory() override = default;
  uint8_t Read(uint16_t address) override;
  void Write(uint16_t address, uint8_t value) override;
};

#endif