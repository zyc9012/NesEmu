#ifndef _MAPPER_4_H
#define _MAPPER_4_H

#include <stdint.h>
class Mapper;
class Cartridge;
class Console;
class StateFile;

class Mapper4 : public Mapper
{
public:
  Mapper4(Console* console, Cartridge* cartridge);
  ~Mapper4();

  virtual uint8_t Read(uint16_t address) override;
  virtual void Write(uint16_t address, uint8_t value) override;
  virtual void Step() override;
  virtual bool Save(StateFile*) override;
  virtual bool Load(StateFile*) override;
  void HandleScanLine();
  void writeRegister(uint16_t address, uint8_t value);
  void writeBankSelect(uint8_t value);
  void writeBankData(uint8_t value);
  void writeMirror(uint8_t value);
  void writeProtect(uint8_t value);
  void writeIRQLatch(uint8_t value);
  void writeIRQReload(uint8_t value);
  void writeIRQDisable(uint8_t value);
  void writeIRQEnable(uint8_t value);
  int prgBankOffset(int index);
  int chrBankOffset(int index);
  void updateOffsets();

  Cartridge* cartridge;
  Console* console;
  uint8_t _register = 0;
  uint8_t registers[8] = {0,};
  uint8_t prgMode = 0;
  uint8_t chrMode = 0;
  int prgOffsets[4] = {0,};
  int chrOffsets[8] = {0,};
  uint8_t reload = 0;
  uint8_t counter = 0;
  bool irqEnable = false;
};

#endif