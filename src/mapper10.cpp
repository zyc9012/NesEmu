#include "mapper.h"
#include "mapper10.h"
#include "cartridge.h"
#include "memory.h"
#include "utils.h"
#include "statefile.h"

Mapper10::Mapper10(Cartridge* cartridge)
{
  this->cartridge = cartridge;
  prgBanks = cartridge->PRG_len / 0x4000;
  prgBank1 = 0;
  prgBank2 = prgBanks - 1;
  chrBanks = cartridge->CHR_len / 0x1000;
  chrBank1 = 0;
  chrBank2 = 0;

  reg[0] = 0;
  reg[1] = 4;
  reg[2] = 0;
  reg[3] = 0;

  latchA = 0xFE;
  latchB = 0xFE;
}


Mapper10::~Mapper10()
{
}


bool Mapper10::Save(StateFile* f) {
  f->Put(&prgBanks);
  f->Put(&prgBank1);
  f->Put(&prgBank2);
  return true;
}

bool Mapper10::Load(StateFile* f) {
  f->Get(&prgBanks);
  f->Get(&prgBank1);
  f->Get(&prgBank2);
  return true;
}

void Mapper10::Step() {
}

uint8_t Mapper10::Read(uint16_t address) {
  int index;

  if ((address & 0x1FF0) == 0x0FD0 && latchA != 0xFD) {
    latchA = 0xFD;
    chrBank1 = reg[0];
  } else if ((address & 0x1FF0) == 0x0FE0 && latchA != 0xFE) {
    latchA = 0xFE;
    chrBank1 = reg[1];
  } else if ((address & 0x1FF0) == 0x1FD0 && latchB != 0xFD) {
    latchB = 0xFD;
    chrBank2 = reg[2];
  } else if ((address & 0x1FF0) == 0x1FE0 && latchB != 0xFE) {
    latchB = 0xFE;
    chrBank2 = reg[3];
  }

  if (address < 0x1000)
  {
    index = chrBank1 * 0x1000 + address;
    return cartridge->CHR[index];
  }
  else if (address < 0x2000)
  {
    index = chrBank2 * 0x1000 + address - 0x1000;
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
    log("unhandled Mapper10 read at address: 0x%04X", address);
  }
  return 0;
}

void Mapper10::Write(uint16_t address, uint8_t value) {
  if (address < 0x2000)
    cartridge->CHR[address] = value;
  else if (address >= 0x8000)
  {
    switch (address & 0xF000) {
    case 0xA000:
      prgBank1 = int(value) % prgBanks;
      break;
    case 0xB000:
      reg[0] = value;
      if (latchA == 0xFD) {
        chrBank1 = int(value);
      }
      break;
    case 0xC000:
      reg[1] = value;
      if (latchA == 0xFE) {
        chrBank1 = int(value);
      }
      break;
    case 0xD000:
      reg[2] = value;
      if (latchB == 0xFD) {
        chrBank2 = int(value);
      }
      break;
    case 0xE000:
      reg[3] = value;
      if (latchB == 0xFE) {
        chrBank2 = int(value);
      }
      break;
    case 0xF000:
      if (value & 0x01)
        cartridge->Mirror = MirrorHorizontal;
      else
        cartridge->Mirror = MirrorVertical;
      break;
    }
  }
  else if (address >= 0x6000)
  {
    auto index = int(address) - 0x6000;
    cartridge->SRAM[index] = value;
  }
  else
  {
    log("unhandled Mapper10 write at address: 0x%04X", address);
  }
}
