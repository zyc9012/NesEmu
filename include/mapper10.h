#ifndef _MAPPER_10_H
#define _MAPPER_10_H

#include <cstdint>
#include <array>
#include "mapper.h"

class Cartridge;
class StateFile;

class Mapper10 : public Mapper
{
public:
  explicit Mapper10(Cartridge* cartridge);
  ~Mapper10() override = default;

  uint8_t Read(uint16_t address) override;
  void Write(uint16_t address, uint8_t value) override;
  void Step() override {}
  bool Save(StateFile*) override;
  bool Load(StateFile*) override;

  Cartridge* cartridge{nullptr};
  int prgBanks{0};
  int prgBank1{0};
  int prgBank2{0};
  int chrBanks{0};
  int chrBank1{0};
  int chrBank2{0};
  std::array<uint8_t, 4> reg{};
  uint8_t latchA{0};
  uint8_t latchB{0};
};

#endif