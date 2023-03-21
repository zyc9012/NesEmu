#include "Mapper.h"
#include "Mapper1.h"
#include "Memory.h"
#include "Cartridge.h"
#include "utils.h"
#include "StateFile.h"


Mapper1::Mapper1(Cartridge* cartridge)
{
  this->cartridge = cartridge;
  shiftRegister = 0x10;
  prgOffsets[1] = prgBankOffset(-1);
}


Mapper1::~Mapper1()
{
}


bool Mapper1::Save(StateFile* f) {
  f->Put(&shiftRegister);
  f->Put(&control);
  f->Put(&prgMode);
  f->Put(&chrMode);
  f->Put(&prgBank);
  f->Put(&chrBank0);
  f->Put(&chrBank1);
  f->Put(prgOffsets, 2);
  f->Put(chrOffsets, 2);
  return true;
}

bool Mapper1::Load(StateFile* f) {
  f->Get(&shiftRegister);
  f->Get(&control);
  f->Get(&prgMode);
  f->Get(&chrMode);
  f->Get(&prgBank);
  f->Get(&chrBank0);
  f->Get(&chrBank1);
  f->Get(prgOffsets, 2);
  f->Get(chrOffsets, 2);
  return true;
}

void Mapper1::Step() {

}

uint8_t Mapper1::Read(uint16_t address) {
  if (address < 0x2000) {
    auto bank = address / 0x1000;
    auto offset = address % 0x1000;
    return cartridge->CHR[chrOffsets[bank] + int(offset)];
  }
  else if (address >= 0x8000) {
    address = address - 0x8000;
    auto bank = address / 0x4000;
    auto offset = address % 0x4000;
    return cartridge->PRG[prgOffsets[bank] + int(offset)];
  }
  else if (address >= 0x6000) {
    return cartridge->SRAM[int(address) - 0x6000];
  }
  else {
    log("unhandled mapper1 read at address: 0x%04X", address);
  }
  return 0;
}

void Mapper1::Write(uint16_t address, uint8_t value) {
  if (address < 0x2000) {
    auto bank = address / 0x1000;
    auto offset = address % 0x1000;
    cartridge->CHR[chrOffsets[bank] + int(offset)] = value;
  }
  else if (address >= 0x8000) {
    loadRegister(address, value);
  }
  else if (address >= 0x6000) {
    cartridge->SRAM[int(address) - 0x6000] = value;
  }
  else {
    log("unhandled mapper1 write at address: 0x%04X", address);
  }
}

void Mapper1::loadRegister(uint16_t address, uint8_t value) {
  if ((value&0x80) == 0x80) {
    shiftRegister = 0x10;
    writeControl(control | 0x0C);
  } else {
    auto complete = (shiftRegister & 1) == 1;
    shiftRegister >>= 1;
    shiftRegister |= (value & 1) << 4;
    if (complete) {
      writeRegister(address, shiftRegister);
      shiftRegister = 0x10;
    }
  }
}

void Mapper1::writeRegister(uint16_t address, uint8_t value) {
  if (address <= 0x9FFF) {
    writeControl(value);
  }
  else if (address <= 0xBFFF) {
    writeCHRBank0(value);
  }
  else if (address <= 0xDFFF) {
    writeCHRBank1(value);
  }
  else if (address <= 0xFFFF) {
    writePRGBank(value);
  }
}

// Control (internal, $8000-$9FFF)
void Mapper1::writeControl(uint8_t value) {
  control = value;
  chrMode = (value >> 4) & 1;
  prgMode = (value >> 2) & 3;
  auto mirror = value & 3;
  switch (mirror) {
  case 0:
    cartridge->Mirror = MirrorSingle0;
    break;
  case 1:
    cartridge->Mirror = MirrorSingle1;
    break;
  case 2:
    cartridge->Mirror = MirrorVertical;
    break;
  case 3:
    cartridge->Mirror = MirrorHorizontal;
    break;
  }
  updateOffsets();
}

// CHR bank 0 (internal, $A000-$BFFF)
void Mapper1::writeCHRBank0(uint8_t value) {
  chrBank0 = value;
  updateOffsets();
}

// CHR bank 1 (internal, $C000-$DFFF)
void Mapper1::writeCHRBank1(uint8_t value) {
  chrBank1 = value;
  updateOffsets();
}

// PRG bank (internal, $E000-$FFFF)
void Mapper1::writePRGBank(uint8_t value) {
  prgBank = value & 0x0F;
  updateOffsets();
}

int Mapper1::prgBankOffset(int index)  {
  if (index >= 0x80) {
    index -= 0x100;
  }
  index %= cartridge->PRG_len / 0x4000;
  auto offset = index * 0x4000;
  if (offset < 0) {
    offset += cartridge->PRG_len;
  }
  return offset;
}

int Mapper1::chrBankOffset(int index) {
  if (index >= 0x80) {
    index -= 0x100;
  }
  index %= cartridge->CHR_len / 0x1000;
  auto offset = index * 0x1000;
  if (offset < 0) {
    offset += cartridge->CHR_len;
  }
  return offset;
}

// PRG ROM bank mode (0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
//                    2: fix first bank at $8000 and switch 16 KB bank at $C000;
//                    3: fix last bank at $C000 and switch 16 KB bank at $8000)
// CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
void Mapper1::updateOffsets() {
  switch (prgMode) {
  case 0:
  case 1:
    prgOffsets[0] = prgBankOffset(int(prgBank & 0xFE));
    prgOffsets[1] = prgBankOffset(int(prgBank | 0x01));
    break;
  case 2:
    prgOffsets[0] = 0;
    prgOffsets[1] = prgBankOffset(int(prgBank));
    break;
  case 3:
    prgOffsets[0] = prgBankOffset(int(prgBank));
    prgOffsets[1] = prgBankOffset(-1);
    break;
  }
  switch (chrMode) {
  case 0:
    chrOffsets[0] = chrBankOffset(int(chrBank0 & 0xFE));
    chrOffsets[1] = chrBankOffset(int(chrBank0 | 0x01));
    break;
  case 1:
    chrOffsets[0] = chrBankOffset(int(chrBank0));
    chrOffsets[1] = chrBankOffset(int(chrBank1));
    break;
  }
}
