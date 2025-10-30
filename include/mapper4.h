#ifndef _MAPPER_4_H
#define _MAPPER_4_H

#include <cstdint>
#include <array>
#include "mapper.h"

class Cartridge;
class Console;
class StateFile;

class Mapper4 : public Mapper
{
public:
  Mapper4(Console* console, Cartridge* cartridge);
  ~Mapper4() override = default;

  uint8_t Read(uint16_t address) override;
  void Write(uint16_t address, uint8_t value) override;
  void Step() override;
  bool Save(StateFile*) override;
  bool Load(StateFile*) override;
  void HandleScanLine();
  void writeRegister(uint16_t address, uint8_t value);
  void writeBankSelect(uint8_t value);
  void writeBankData(uint8_t value);
  void writeMirror(uint8_t value);
  void writeProtect([[maybe_unused]] uint8_t value) {}
  void writeIRQLatch(uint8_t value);
  void writeIRQReload([[maybe_unused]] uint8_t value);
  void writeIRQDisable([[maybe_unused]] uint8_t value);
  void writeIRQEnable([[maybe_unused]] uint8_t value);
  int prgBankOffset(int index) const;
  int chrBankOffset(int index) const;
  void updateOffsets();

  Cartridge* cartridge{nullptr};
  Console* console{nullptr};
  uint8_t _register{0};
  std::array<uint8_t, 8> registers{};
  uint8_t prgMode{0};
  uint8_t chrMode{0};
  std::array<int, 4> prgOffsets{};
  std::array<int, 8> chrOffsets{};
  uint8_t reload{0};
  uint8_t counter{0};
  bool irqEnable{false};
};

#endif