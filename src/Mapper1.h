#ifndef _MAPPER_1_H
#define _MAPPER_1_H

#include <stdint.h>
class Mapper;
class Cartridge;
class StateFile;

class Mapper1 : public Mapper
{
public:
  Mapper1(Cartridge* cartridge);
  ~Mapper1();

  virtual uint8_t Read(uint16_t address) override;
  virtual void Write(uint16_t address, uint8_t value) override;
  virtual void Step() override;
  virtual bool Save(StateFile*) override;
  virtual bool Load(StateFile*) override;
  void loadRegister(uint16_t address, uint8_t value);
  void writeRegister(uint16_t address, uint8_t value);
  void writeControl(uint8_t value);
  void writePRGBank(uint8_t value);
  void writeCHRBank0(uint8_t value);
  void writeCHRBank1(uint8_t value);
  int prgBankOffset(int index);
  int chrBankOffset(int index);
  void updateOffsets();

  Cartridge* cartridge;
  uint8_t shiftRegister;
  uint8_t control;
  uint8_t prgMode;
  uint8_t chrMode;
  uint8_t prgBank;
  uint8_t chrBank0;
  uint8_t chrBank1;
  int prgOffsets[2];
  int chrOffsets[2];
};

#endif