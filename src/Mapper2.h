#ifndef _MAPPER_2_H
#define _MAPPER_2_H

#include <stdint.h>
class Mapper;
class Cartridge;
class StateFile;

class Mapper2 : public Mapper
{
public:
  Mapper2(Cartridge* cartridge);
  ~Mapper2();

  virtual uint8_t Read(uint16_t address) override;
  virtual void Write(uint16_t address, uint8_t value) override;
  virtual void Step() override;
  virtual bool Save(StateFile*) override;
  virtual bool Load(StateFile*) override;

  Cartridge* cartridge;
  int prgBanks;
  int prgBank1;
  int prgBank2;
};

#endif