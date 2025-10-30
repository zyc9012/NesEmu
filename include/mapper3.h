#ifndef _MAPPER_3_H
#define _MAPPER_3_H

#include <cstdint>
#include "mapper.h"

class Cartridge;
class StateFile;

class Mapper3 : public Mapper
{
public:
  explicit Mapper3(Cartridge* cartridge);
  ~Mapper3() override = default;

  uint8_t Read(uint16_t address) override;
  void Write(uint16_t address, uint8_t value) override;
  void Step() override {}
  bool Save(StateFile*) override;
  bool Load(StateFile*) override;

  Cartridge* cartridge{nullptr};
  int chrBank{0};
  int prgBank1{0};
  int prgBank2{0};
};

#endif