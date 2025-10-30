#ifndef _MAPPER_2_H
#define _MAPPER_2_H

#include <cstdint>
#include "mapper.h"

class Cartridge;
class StateFile;

class Mapper2 : public Mapper
{
public:
  explicit Mapper2(Cartridge* cartridge);
  ~Mapper2() override = default;

  uint8_t Read(uint16_t address) override;
  void Write(uint16_t address, uint8_t value) override;
  void Step() override {}
  bool Save(StateFile*) override;
  bool Load(StateFile*) override;

  Cartridge* cartridge{nullptr};
  int prgBanks{0};
  int prgBank1{0};
  int prgBank2{0};
};

#endif