#include "mapper.h"
#include "mapper2.h"
#include "cartridge.h"
#include "utils.h"
#include "statefile.h"

Mapper2::Mapper2(Cartridge* cartridge)
{
  this->cartridge = cartridge;
  prgBanks = static_cast<int>(cartridge->PRG.size()) / 0x4000;
  prgBank1 = 0;
  prgBank2 = prgBanks - 1;
}


bool Mapper2::Save(StateFile* f) {
  f->Put(&prgBanks);
  f->Put(&prgBank1);
  f->Put(&prgBank2);
  return true;
}

bool Mapper2::Load(StateFile* f) {
  f->Get(&prgBanks);
  f->Get(&prgBank1);
  f->Get(&prgBank2);
  return true;
}

uint8_t Mapper2::Read(uint16_t address) {
  int index;
  if (address < 0x2000)
  {
    return cartridge->CHR[address];
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
    log("unhandled mapper2 read at address: 0x%04X", address);
  }
  return 0;
}

void Mapper2::Write(uint16_t address, uint8_t value) {
  if (address < 0x2000)
    cartridge->CHR[address] = value;
  else if (address >= 0x8000)
    prgBank1 = int(value) % prgBanks;
  else if (address >= 0x6000)
  {
    auto index = int(address) - 0x6000;
    cartridge->SRAM[index] = value;
  }
  else
  {
    log("unhandled mapper2 write at address: 0x%04X", address);
  }
}
