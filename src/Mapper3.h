#ifndef _MAPPER_3_H
#define _MAPPER_3_H

#include <stdint.h>
class Mapper;
class Cartridge;
class StateFile;

class Mapper3 : public Mapper
{
public:
  Mapper3(Cartridge* cartridge);
  ~Mapper3();

  virtual uint8_t Read(uint16_t address) override;
  virtual void Write(uint16_t address, uint8_t value) override;
  virtual void Step() override;
  virtual bool Save(StateFile*) override;
  virtual bool Load(StateFile*) override;

  Cartridge* cartridge;
  int chrBank;
  int prgBank1;
  int prgBank2;
};

#endif