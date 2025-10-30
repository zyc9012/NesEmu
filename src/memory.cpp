#include "memory.h"
#include "ppu.h"
#include "apu.h"
#include "controller.h"
#include "mapper.h"
#include "utils.h"
#include "console.h"
#include "cartridge.h"

uint16_t MirrorLookup[][4]{
  { 0, 0, 1, 1 },
  { 0, 1, 0, 1 },
  { 0, 0, 0, 0 },
  { 1, 1, 1, 1 },
  { 0, 1, 2, 3 },
};

uint16_t MirrorAddress(uint8_t mode, uint16_t address)  {
  address = (address - 0x2000) % 0x1000;
  auto table = address / 0x0400;
  auto offset = address % 0x0400;
  return 0x2000 + MirrorLookup[mode][table] * 0x0400 + offset;
}


CpuMemory::CpuMemory(Console* console)
{
  this->console = console;
}


CpuMemory::~CpuMemory()
{
}

uint8_t CpuMemory::Read(uint16_t address)
{
  if (address < 0x2000)
    return console->RAM[address % 0x0800];
  else if (address < 0x4000)
    return console->PPU->readRegister(0x2000 + (address % 8));
  else if (address == 0x4014)
    return console->PPU->readRegister(address);
  else if (address == 0x4015)
    return console->APU->readRegister(address);
  else if (address == 0x4016)
    return console->Controller1->Read();
  else if (address == 0x4017)
    return console->Controller2->Read();
  else if (address < 0x6000)
  {
    // TODO: I/O registers
  }
  else if (address >= 0x6000)
    return console->_Mapper->Read(address);
  else
    log("unhandled cpu memory read at address: 0x%04X\n", address);
  
  return 0;
}

void CpuMemory::Write(uint16_t address, uint8_t value) {
  if (address < 0x2000)
    console->RAM[address % 0x0800] = value;
  else if (address < 0x4000)
    console->PPU->writeRegister(0x2000 + (address % 8), value);
  else if (address < 0x4014)
    console->APU->writeRegister(address, value);
  else if (address == 0x4014)
    console->PPU->writeRegister(address, value);
  else if (address == 0x4015)
    console->APU->writeRegister(address, value);
  else if (address == 0x4016)
  {
    console->Controller1->Write(value);
    console->Controller2->Write(value);
  }
  else if (address == 0x4017)
    console->APU->writeRegister(address, value);
  else if (address < 0x6000)
  {
    // TODO: I/O registers
  }
  else if (address >= 0x6000)
    console->_Mapper->Write(address, value);
  else
    log("unhandled cpu memory write at address: 0x%04X", address);
}



PpuMemory::PpuMemory(Console* console)
{
  this->console = console;
}


PpuMemory::~PpuMemory()
{
}

uint8_t PpuMemory::Read(uint16_t address) {
  address = address % 0x4000;
  if (address < 0x2000)
    return console->_Mapper->Read(address);
  else if (address < 0x3F00)
  {
    auto mode = console->_Cartridge->Mirror;
    return console->PPU->nameTableData[MirrorAddress(mode, address) % 2048];
  }
  else if (address < 0x4000)
    return console->PPU->readPalette(address % 32);
  else
    log("unhandled ppu memory read at address: 0x%04X", address);
  return 0;
}

void PpuMemory::Write(uint16_t address, uint8_t value) {
  address = address % 0x4000;
  if (address < 0x2000)
    console->_Mapper->Write(address, value);
  else if (address < 0x3F00)
  {
    auto mode = console->_Cartridge->Mirror;
    console->PPU->nameTableData[MirrorAddress(mode, address) % 2048] = value;
  }
  else if (address < 0x4000)
    console->PPU->writePalette(address % 32, value);
  else
    log("unhandled ppu memory write at address: 0x%04X", address);
}
