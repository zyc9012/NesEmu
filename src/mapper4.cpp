#include "mapper.h"
#include "mapper4.h"
#include "ppu.h"
#include "console.h"
#include "cpu.h"
#include "memory.h"
#include "cartridge.h"
#include "utils.h"
#include "statefile.h"

Mapper4::Mapper4(Console* console, Cartridge* cartridge)
{
  this->console = console;
  this->cartridge = cartridge;
  prgOffsets[0] = prgBankOffset(0);
  prgOffsets[1] = prgBankOffset(1);
  prgOffsets[2] = prgBankOffset(-2);
  prgOffsets[3] = prgBankOffset(-1);
}


Mapper4::~Mapper4()
{
}

bool Mapper4::Save(StateFile* f) {
  f->Put(&_register);
  f->Put(registers, 8);
  f->Put(&prgMode);
  f->Put(&chrMode);
  f->Put(prgOffsets, 4);
  f->Put(chrOffsets, 8);
  f->Put(&reload);
  f->Put(&counter);
  f->Put(&irqEnable);
  return true;
}

bool Mapper4::Load(StateFile* f) {
  f->Get(&_register);
  f->Get(registers, 8);
  f->Get(&prgMode);
  f->Get(&chrMode);
  f->Get(prgOffsets, 4);
  f->Get(chrOffsets, 8);
  f->Get(&reload);
  f->Get(&counter);
  f->Get(&irqEnable);
  return true;
}

void Mapper4::Step() {
  auto ppu = console->PPU;
  if (ppu->Cycle != 280) { // TODO: this *should* be 260
    return;
  }
  if (ppu->ScanLine > 239 && ppu->ScanLine < 261) {
    return;
  }
  if (ppu->flagShowBackground == 0 && ppu->flagShowSprites == 0) {
    return;
  }
  HandleScanLine();
}

void Mapper4::HandleScanLine() {
  if (counter == 0) {
    counter = reload;
  }
  else {
    counter--;
    if (counter == 0 && irqEnable) {
      console->CPU->triggerIRQ();
    }
  }
}

uint8_t Mapper4::Read(uint16_t address) {
  if (address < 0x2000) {
    auto bank = address / 0x0400;
    auto offset = address % 0x0400;
    return cartridge->CHR[chrOffsets[bank] + int(offset)];
  }
  else if (address >= 0x8000) {
    address = address - 0x8000;
    auto bank = address / 0x2000;
    auto offset = address % 0x2000;
    return cartridge->PRG[prgOffsets[bank] + int(offset)];
  }
  else if (address >= 0x6000) {
    return cartridge->SRAM[int(address) - 0x6000];
  }
  else {
    log("unhandled mapper4 read at address: 0x%04X", address);
  }
  return 0;
}

void Mapper4::Write(uint16_t address, uint8_t value) {
  if (address < 0x2000) {
    auto bank = address / 0x0400;
    auto offset = address % 0x0400;
    cartridge->CHR[chrOffsets[bank] + int(offset)] = value;
  }
  else if (address >= 0x8000) {
    writeRegister(address, value);
  }
  else if (address >= 0x6000) {
    cartridge->SRAM[int(address) - 0x6000] = value;
  }
  else {
    log("unhandled mapper4 write at address: 0x%04X", address);
  }
}

void Mapper4::writeRegister(uint16_t address, uint8_t value) {
  if (address <= 0x9FFF && address % 2 == 0) {
    writeBankSelect(value);
  }
  else if (address <= 0x9FFF && address % 2 == 1) {
    writeBankData(value);
  }
  else if (address <= 0xBFFF && address % 2 == 0) {
    writeMirror(value);
  }
  else if (address <= 0xBFFF && address % 2 == 1) {
    writeProtect(value);
  }
  else if (address <= 0xDFFF && address % 2 == 0) {
    writeIRQLatch(value);
  }
  else if (address <= 0xDFFF && address % 2 == 1) {
    writeIRQReload(value);
  }
  else if (address <= 0xFFFF && address % 2 == 0) {
    writeIRQDisable(value);
  }
  else if (address <= 0xFFFF && address % 2 == 1) {
    writeIRQEnable(value);
  }
}

void Mapper4::writeBankSelect(uint8_t value) {
  prgMode = (value >> 6) & 1;
  chrMode = (value >> 7) & 1;
  _register = value & 7;
  updateOffsets();
}

void Mapper4::writeBankData(uint8_t value) {
  registers[_register] = value;
  updateOffsets();
}

void Mapper4::writeMirror(uint8_t value) {
  switch (value & 1) {
  case 0:
    cartridge->Mirror = MirrorVertical;
    break;
  case 1:
    cartridge->Mirror = MirrorHorizontal;
    break;
  }
}

void Mapper4::writeProtect(uint8_t value) {
}

void Mapper4::writeIRQLatch(uint8_t value) {
  reload = value;
}

void Mapper4::writeIRQReload(uint8_t value) {
  counter = 0;
}

void Mapper4::writeIRQDisable(uint8_t value) {
  irqEnable = false;
}

void Mapper4::writeIRQEnable(uint8_t value) {
  irqEnable = true;
}

int Mapper4::prgBankOffset(int index) {
  if (index >= 0x80) {
    index -= 0x100;
  }
  index %= cartridge->PRG_len / 0x2000;
  auto offset = index * 0x2000;
  if (offset < 0) {
    offset += cartridge->PRG_len;
  }
  return offset;
}

int Mapper4::chrBankOffset(int index) {
  if (index >= 0x80) {
    index -= 0x100;
  }
  index %= cartridge->CHR_len / 0x0400;
  auto offset = index * 0x0400;
  if (offset < 0) {
    offset += cartridge->CHR_len;
  }
  return offset;
}

void Mapper4::updateOffsets() {
  switch (prgMode) {
  case 0:
    prgOffsets[0] = prgBankOffset(int(registers[6]));
    prgOffsets[1] = prgBankOffset(int(registers[7]));
    prgOffsets[2] = prgBankOffset(-2);
    prgOffsets[3] = prgBankOffset(-1);
    break;
  case 1:
    prgOffsets[0] = prgBankOffset(-2);
    prgOffsets[1] = prgBankOffset(int(registers[7]));
    prgOffsets[2] = prgBankOffset(int(registers[6]));
    prgOffsets[3] = prgBankOffset(-1);
    break;
  }
  switch (chrMode) {
  case 0:
    chrOffsets[0] = chrBankOffset(int(registers[0] & 0xFE));
    chrOffsets[1] = chrBankOffset(int(registers[0] | 0x01));
    chrOffsets[2] = chrBankOffset(int(registers[1] & 0xFE));
    chrOffsets[3] = chrBankOffset(int(registers[1] | 0x01));
    chrOffsets[4] = chrBankOffset(int(registers[2]));
    chrOffsets[5] = chrBankOffset(int(registers[3]));
    chrOffsets[6] = chrBankOffset(int(registers[4]));
    chrOffsets[7] = chrBankOffset(int(registers[5]));
    break;
  case 1:
    chrOffsets[0] = chrBankOffset(int(registers[2]));
    chrOffsets[1] = chrBankOffset(int(registers[3]));
    chrOffsets[2] = chrBankOffset(int(registers[4]));
    chrOffsets[3] = chrBankOffset(int(registers[5]));
    chrOffsets[4] = chrBankOffset(int(registers[0] & 0xFE));
    chrOffsets[5] = chrBankOffset(int(registers[0] | 0x01));
    chrOffsets[6] = chrBankOffset(int(registers[1] & 0xFE));
    chrOffsets[7] = chrBankOffset(int(registers[1] | 0x01));
    break;
  }
}
