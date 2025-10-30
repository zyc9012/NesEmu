#ifndef _MAPPER_1_H
#define _MAPPER_1_H

#include <cstdint>
#include <array>
#include "mapper.h"

class Cartridge;
class StateFile;

class Mapper1 : public Mapper
{
public:
  explicit Mapper1(Cartridge* cartridge);
  ~Mapper1() override = default;

  uint8_t Read(uint16_t address) override;
  void Write(uint16_t address, uint8_t value) override;
  void Step() override {}
  bool Save(StateFile*) override;
  bool Load(StateFile*) override;
  void loadRegister(uint16_t address, uint8_t value);
  void writeRegister(uint16_t address, uint8_t value);
  void writeControl(uint8_t value);
  void writePRGBank(uint8_t value);
  void writeCHRBank0(uint8_t value);
  void writeCHRBank1(uint8_t value);
  int prgBankOffset(int index) const;
  int chrBankOffset(int index) const;
  void updateOffsets();

  Cartridge* cartridge{nullptr};
  uint8_t shiftRegister{0};
  uint8_t control{0};
  uint8_t prgMode{0};
  uint8_t chrMode{0};
  uint8_t prgBank{0};
  uint8_t chrBank0{0};
  uint8_t chrBank1{0};
  std::array<int, 2> prgOffsets{};
  std::array<int, 2> chrOffsets{};
};

#endif