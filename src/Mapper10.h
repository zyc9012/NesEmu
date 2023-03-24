#ifndef _MAPPER_10_H
#define _MAPPER_10_H

#include <stdint.h>
class Mapper;
class Cartridge;
class StateFile;

class Mapper10 : public Mapper
{
public:
  Mapper10(Cartridge* cartridge);
  ~Mapper10();

  virtual uint8_t Read(uint16_t address) override;
  virtual void Write(uint16_t address, uint8_t value) override;
  virtual void Step() override;
  virtual bool Save(StateFile*) override;
  virtual bool Load(StateFile*) override;

  Cartridge* cartridge;
  int prgBanks;
  int prgBank1;
  int prgBank2;
  int chrBanks;
  int chrBank1;
  int chrBank2;
	uint8_t	reg[4];
	uint8_t	latchA, latchB;
};

#endif