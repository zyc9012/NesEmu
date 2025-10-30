#include "mapper.h"
#include "mapper3.h"
#include "cartridge.h"
#include "utils.h"
#include "statefile.h"

Mapper3::Mapper3(Cartridge* cartridge)
{
  this->cartridge = cartridge;
  auto prgBanks = cartridge->PRG_len / 0x4000;
  prgBank1 = 0;
  prgBank2 = prgBanks - 1;
}


Mapper3::~Mapper3()
{
}


bool Mapper3::Save(StateFile* f) {
  f->Put(&chrBank);
  f->Put(&prgBank1);
  f->Put(&prgBank2);
  return true;
}

bool Mapper3::Load(StateFile* f) {
  f->Get(&chrBank);
  f->Get(&prgBank1);
  f->Get(&prgBank2);
  return true;
}

void Mapper3::Step() {
}

uint8_t Mapper3::Read(uint16_t address) {
  int index;
  if (address < 0x2000)
  {
    index = chrBank * 0x2000 + int(address);
    return cartridge->CHR[index];
  }
  else if (address >= 0xC000)
  {
    index = prgBank2 * 0x4000 + int(address - 0xC000);
    return cartridge->PRG[index];
  }
  else if (address >= 0x8000)
  {
    index = prgBank1 * 0x4000 + int(address - 0x8000);
    return cartridge->PRG[index];
  }
  else if (address >= 0x6000)
  {
    index = int(address) - 0x6000;
    return cartridge->SRAM[index];
  }
  else
  {
    log("unhandled Mapper3 read at address: 0x%04X", address);
  }
  return 0;
}

void Mapper3::Write(uint16_t address, uint8_t value) {
  if (address < 0x2000) {
    auto index = chrBank * 0x2000 + int(address);
    cartridge->CHR[index] = value;
  }
  else if (address >= 0x8000)
  {
    chrBank  = int(value & 3);
  }
  else if (address >= 0x6000)
  {
    auto index = int(address) - 0x6000;
    cartridge->SRAM[index] = value;
  }
  else
  {
    log("unhandled Mapper3 write at address: 0x%04X", address);
  }
}
