#include "Ppu.h"
#include "Cpu.h"
#include "Palette.h"
#include "Console.h"
#include "Memory.h"
#include "Image.h"
#include "StateFile.h"

Ppu::Ppu(Console* console)
{
  initPalette();
  this->console = console;
  memory = new PpuMemory(console);
  front = new Image(256, 240);
  back = new Image(256, 240);
  Reset();
}


Ppu::~Ppu()
{
}


bool Ppu::Save(StateFile* f) {
  f->Put(&Cycle);
  f->Put(&ScanLine);
  f->Put(&Frame);
  f->Put(paletteData, 32);
  f->Put(nameTableData, 2048);
  f->Put(oamData, 256);
  f->Put(&v);
  f->Put(&t);
  f->Put(&x);
  f->Put(&w);
  f->Put(&this->f);
  f->Put(&_register);
  f->Put(&nmiOccurred);
  f->Put(&nmiOutput);
  f->Put(&nmiPrevious);
  f->Put(&nmiDelay);
  f->Put(&nameTableByte);
  f->Put(&attributeTableByte);
  f->Put(&lowTileByte);
  f->Put(&highTileByte);
  f->Put(&tileData);
  f->Put(&spriteCount);
  f->Put(spritePatterns, 8);
  f->Put(spritePositions, 8);
  f->Put(spritePriorities, 8);
  f->Put(spriteIndexes, 8);
  f->Put(&flagNameTable);
  f->Put(&flagIncrement);
  f->Put(&flagSpriteTable);
  f->Put(&flagBackgroundTable);
  f->Put(&flagSpriteSize);
  f->Put(&flagMasterSlave);
  f->Put(&flagGrayscale);
  f->Put(&flagShowLeftBackground);
  f->Put(&flagShowLeftSprites);
  f->Put(&flagShowBackground);
  f->Put(&flagShowSprites);
  f->Put(&flagRedTint);
  f->Put(&flagGreenTint);
  f->Put(&flagBlueTint);
  f->Put(&flagSpriteZeroHit);
  f->Put(&flagSpriteOverflow);
  f->Put(&oamAddress);
  f->Put(&bufferedData);
  return true;
}

bool Ppu::Load(StateFile* f) {
  f->Get(&Cycle);
  f->Get(&ScanLine);
  f->Get(&Frame);
  f->Get(paletteData, 32);
  f->Get(nameTableData, 2048);
  f->Get(oamData, 256);
  f->Get(&v);
  f->Get(&t);
  f->Get(&x);
  f->Get(&w);
  f->Get(&this->f);
  f->Get(&_register);
  f->Get(&nmiOccurred);
  f->Get(&nmiOutput);
  f->Get(&nmiPrevious);
  f->Get(&nmiDelay);
  f->Get(&nameTableByte);
  f->Get(&attributeTableByte);
  f->Get(&lowTileByte);
  f->Get(&highTileByte);
  f->Get(&tileData);
  f->Get(&spriteCount);
  f->Get(spritePatterns, 8);
  f->Get(spritePositions, 8);
  f->Get(spritePriorities, 8);
  f->Get(spriteIndexes, 8);
  f->Get(&flagNameTable);
  f->Get(&flagIncrement);
  f->Get(&flagSpriteTable);
  f->Get(&flagBackgroundTable);
  f->Get(&flagSpriteSize);
  f->Get(&flagMasterSlave);
  f->Get(&flagGrayscale);
  f->Get(&flagShowLeftBackground);
  f->Get(&flagShowLeftSprites);
  f->Get(&flagShowBackground);
  f->Get(&flagShowSprites);
  f->Get(&flagRedTint);
  f->Get(&flagGreenTint);
  f->Get(&flagBlueTint);
  f->Get(&flagSpriteZeroHit);
  f->Get(&flagSpriteOverflow);
  f->Get(&oamAddress);
  f->Get(&bufferedData);
  return true;
}

void Ppu::Reset() {
  Cycle = 340;
  ScanLine = 240;
  Frame = 0;
  writeControl(0);
  writeMask(0);
  writeOAMAddress(0);
}

uint8_t Ppu::readPalette(uint16_t address)  {
  if ((address >= 16) && (address%4 == 0)) {
    address -= 16;
  }
  return paletteData[address];
}

void Ppu::writePalette(uint16_t address, uint8_t value) {
  if ((address >= 16) && (address%4 == 0)) {
    address -= 16;
  }
  paletteData[address] = value;
}

uint8_t Ppu::readRegister(uint16_t address)  {
  switch (address) {
    case 0x2002:
      return readStatus();
    case 0x2004:
      return readOAMData();
    case 0x2007:
      return readData();
  }
  return 0;
}

void Ppu::writeRegister(uint16_t address, uint8_t value) {
  _register = value;
  switch (address) {
    case 0x2000:
      writeControl(value);
      break;
    case 0x2001:
      writeMask(value);
      break;
    case 0x2003:
      writeOAMAddress(value);
      break;
    case 0x2004:
      writeOAMData(value);
      break;
    case 0x2005:
      writeScroll(value);
      break;
    case 0x2006:
      writeAddress(value);
      break;
    case 0x2007:
      writeData(value);
      break;
    case 0x4014:
      writeDMA(value);
      break;
  }
}

// $2000: PPUCTRL
void Ppu::writeControl(uint8_t value) {
  flagNameTable = (value >> 0) & 3;
  flagIncrement = (value >> 2) & 1;
  flagSpriteTable = (value >> 3) & 1;
  flagBackgroundTable = (value >> 4) & 1;
  flagSpriteSize = (value >> 5) & 1;
  flagMasterSlave = (value >> 6) & 1;
  nmiOutput = ((value>>7)&1) == 1;
  nmiChange();
  // t: ....BA.. ........ = d: ......BA
  t = (t & 0xF3FF) | ((uint16_t(value) & 0x03) << 10);
}

// $2001: PPUMASK
void Ppu::writeMask(uint8_t value) {
  flagGrayscale = (value >> 0) & 1;
  flagShowLeftBackground = (value >> 1) & 1;
  flagShowLeftSprites = (value >> 2) & 1;
  flagShowBackground = (value >> 3) & 1;
  flagShowSprites = (value >> 4) & 1;
  flagRedTint = (value >> 5) & 1;
  flagGreenTint = (value >> 6) & 1;
  flagBlueTint = (value >> 7) & 1;
}

// $2002: PPUSTATUS
uint8_t Ppu::readStatus() {
  auto result = _register & 0x1F;
  result |= flagSpriteOverflow << 5;
  result |= flagSpriteZeroHit << 6;
  if (nmiOccurred) {
    result |= 1 << 7;
  }
  nmiOccurred = false;
  nmiChange();
  // w:                   = 0
  w = 0;
  return result;
}

// $2003: OAMADDR
void Ppu::writeOAMAddress(uint8_t value) {
  oamAddress = value;
}

// $2004: OAMDATA (read)
uint8_t Ppu::readOAMData() {
  auto data = oamData[oamAddress];
  if ((oamAddress & 0x03) == 0x02) {
    data = data & 0xE3;
  }
  return data;
}

// $2004: OAMDATA (write)
void Ppu::writeOAMData(uint8_t value) {
  oamData[oamAddress] = value;
  oamAddress++;
}

// $2005: PPUSCROLL
void Ppu::writeScroll(uint8_t value) {
  if (w == 0) {
    // t: ........ ...HGFED = d: HGFED...
    // x:               CBA = d: .....CBA
    // w:                   = 1
    t = (t & 0xFFE0) | (uint16_t(value) >> 3);
    x = value & 0x07;
    w = 1;
  } else {
    // t: .CBA..HG FED..... = d: HGFEDCBA
    // w:                   = 0
    t = (t & 0x8FFF) | ((uint16_t(value) & 0x07) << 12);
    t = (t & 0xFC1F) | ((uint16_t(value) & 0xF8) << 2);
    w = 0;
  }
}

// $2006: PPUADDR
void Ppu::writeAddress(uint8_t value) {
  if (w == 0) {
    // t: ..FEDCBA ........ = d: ..FEDCBA
    // t: .X...... ........ = 0
    // w:                   = 1
    t = (t & 0x80FF) | ((uint16_t(value) & 0x3F) << 8);
    w = 1;
  } else {
    // t: ........ HGFEDCBA = d: HGFEDCBA
    // v                    = t
    // w:                   = 0
    t = (t & 0xFF00) | uint16_t(value);
    v = t;
    w = 0;
  }
}

// $2007: PPUDATA (read)
uint8_t Ppu::readData() {
  auto value = memory->Read(v);
  // emulate buffered reads
  if ((v%0x4000) < 0x3F00) {
    auto buffered = bufferedData;
    bufferedData = value;
    value = buffered;
  } else {
    bufferedData = memory->Read(v - 0x1000);
  }
  // increment address
  if (flagIncrement == 0) {
    v += 1;
  } else {
    v += 32;
  }
  return value;
}

// $2007: PPUDATA (write)
void Ppu::writeData(uint8_t value) {
  memory->Write(v, value);
  if (flagIncrement == 0) {
    v += 1;
  } else {
    v += 32;
  }
}

// $4014: OAMDMA
void Ppu::writeDMA(uint8_t value) {
  auto cpu = console->CPU;
  auto address = uint16_t(value) << 8;
  for (int i = 0; i < 256; i++) {
    oamData[oamAddress] = cpu->Read(address);
    oamAddress++;
    address++;
  }
  cpu->stall += 513;
  if ((cpu->Cycles%2) == 1) {
    cpu->stall++;
  }
}

// NTSC Timing Helper Functions

void Ppu::incrementX() {
  // increment hori(v)
  // if coarse X == 31
  if ((v&0x001F) == 31) {
    // coarse X = 0
    v &= 0xFFE0;
    // switch horizontal nametable
    v ^= 0x0400;
  } else {
    // increment coarse X
    v++;
  }
}

void Ppu::incrementY() {
  // increment vert(v)
  // if fine Y < 7
  if ((v&0x7000) != 0x7000) {
    // increment fine Y
    v += 0x1000;
  } else {
    // fine Y = 0
    v &= 0x8FFF;
    // let y = coarse Y
    auto y = (v & 0x03E0) >> 5;
    if (y == 29) {
      // coarse Y = 0
      y = 0;
      // switch vertical nametable
      v ^= 0x0800;
    } else if (y == 31) {
      // coarse Y = 0, nametable not switched
      y = 0;
    } else {
      // increment coarse Y
      y++;
    }
    // put coarse Y back into v
    v = (v & 0xFC1F) | (y << 5);
  }
}

void Ppu::copyX() {
  // hori(v) = hori(t)
  // v: .....F.. ...EDCBA = t: .....F.. ...EDCBA
  v = (v & 0xFBE0) | (t & 0x041F);
}

void Ppu::copyY() {
  // vert(v) = vert(t)
  // v: .IHGF.ED CBA..... = t: .IHGF.ED CBA.....
  v = (v & 0x841F) | (t & 0x7BE0);
}

void Ppu::nmiChange() {
  auto nmi = nmiOutput && nmiOccurred;
  if (nmi && !nmiPrevious) {
    // TODO: this fixes some games but the delay shouldn't have to be so
    // long, so the timings are off somewhere
    nmiDelay = 15;
  }
  nmiPrevious = nmi;
}

void Ppu::setVerticalBlank() {
  auto tmp = front;
  front = back;
  back = tmp;
  nmiOccurred = true;
  nmiChange();
}

void Ppu::clearVerticalBlank() {
  nmiOccurred = false;
  nmiChange();
}

void Ppu::fetchNameTableByte() {
  auto v = this->v;
  auto address = 0x2000 | (v & 0x0FFF);
  nameTableByte = memory->Read(address);
}

void Ppu::fetchAttributeTableByte() {
  auto v = this->v;
  auto address = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
  auto shift = ((v >> 4) & 4) | (v & 2);
  attributeTableByte = ((memory->Read(address) >> shift) & 3) << 2;
}

void Ppu::fetchLowTileByte() {
  auto fineY = (v >> 12) & 7;
  auto table = flagBackgroundTable;
  auto tile = nameTableByte;
  auto address = 0x1000*uint16_t(table) + uint16_t(tile)*16 + fineY;
  lowTileByte = memory->Read(address);
}

void Ppu::fetchHighTileByte() {
  auto fineY = (v >> 12) & 7;
  auto table = flagBackgroundTable;
  auto tile = nameTableByte;
  auto address = 0x1000*uint16_t(table) + uint16_t(tile)*16 + fineY;
  highTileByte = memory->Read(address + 8);
}

void Ppu::storeTileData() {
  uint32_t data = 0;
  for (int i = 0; i < 8; i++) {
    auto a = attributeTableByte;
    auto p1 = (lowTileByte & 0x80) >> 7;
    auto p2 = (highTileByte & 0x80) >> 6;
    lowTileByte <<= 1;
    highTileByte <<= 1;
    data <<= 4;
    data |= uint32_t(a | p1 | p2);
  }
  tileData |= uint64_t(data);
}

uint32_t Ppu::fetchTileData() {
  return uint32_t(tileData >> 32);
}

uint8_t Ppu::backgroundPixel() {
  if (flagShowBackground == 0) {
    return 0;
  }
  auto data = fetchTileData() >> ((7 - x) * 4);
  return uint8_t(data & 0x0F);
}

void Ppu::spritePixel(uint8_t& index, uint8_t& sprite)  {
  if (flagShowSprites == 0) {
    index = 0;
    sprite = 0;
    return;
  }
  for (int i = 0; i < spriteCount; i++) {
    auto offset = (Cycle - 1) - int(spritePositions[i]);
    if (offset < 0 || offset > 7) {
      continue;
    }
    offset = 7 - offset;
    auto color = uint8_t((spritePatterns[i] >> uint8_t(offset * 4)) & 0x0F);
    if (color%4 == 0) {
      continue;
    }
    index = uint8_t(i);
    sprite = color;
    return;
  }
  index = 0;
  sprite = 0;
}

void Ppu::renderPixel() {
  auto x = Cycle - 1;
  auto y = ScanLine;
  auto background = backgroundPixel();
  uint8_t i, sprite;
  spritePixel(i, sprite);
  if (x < 8 && flagShowLeftBackground == 0) {
    background = 0;
  }
  if (x < 8 && flagShowLeftSprites == 0) {
    sprite = 0;
  }
  auto b = ((background%4) != 0);
  auto s = ((sprite%4) != 0);
  uint8_t color = 0;
  if (!b && !s) {
    color = 0;
  } else if (!b && s) {
    color = sprite | 0x10;
  } else if (b && !s) {
    color = background;
  } else {
    if (spriteIndexes[i] == 0 && x < 255) {
      flagSpriteZeroHit = 1;
    }
    if (spritePriorities[i] == 0) {
      color = sprite | 0x10;
    } else {
      color = background;
    }
  }
  auto c = Palette[readPalette(uint16_t(color))%64];
  back->SetColor(x, y, c);
}

uint32_t Ppu::fetchSpritePattern(int i, int row) {
  auto tile = oamData[i*4+1];
  auto attributes = oamData[i*4+2];
  uint16_t address = 0;
  if (flagSpriteSize == 0) {
    if ((attributes&0x80) == 0x80) {
      row = 7 - row;
    }
    auto table = flagSpriteTable;
    address = 0x1000*uint16_t(table) + uint16_t(tile)*16 + uint16_t(row);
  } else {
    if ((attributes&0x80) == 0x80) {
      row = 15 - row;
    }
    auto table = tile & 1;
    tile &= 0xFE;
    if (row > 7) {
      tile++;
      row -= 8;
    }
    address = 0x1000*uint16_t(table) + uint16_t(tile)*16 + uint16_t(row);
  }
  auto a = (attributes & 3) << 2;
  lowTileByte = memory->Read(address);
  highTileByte = memory->Read(address + 8);
  uint32_t data = 0;
  for (i = 0; i < 8; i++) {
    uint8_t p1, p2;
    if ((attributes&0x40) == 0x40) {
      p1 = (lowTileByte & 1) << 0;
      p2 = (highTileByte & 1) << 1;
      lowTileByte >>= 1;
      highTileByte >>= 1;
    } else {
      p1 = (lowTileByte & 0x80) >> 7;
      p2 = (highTileByte & 0x80) >> 6;
      lowTileByte <<= 1;
      highTileByte <<= 1;
    }
    data <<= 4;
    data |= uint32_t(a | p1 | p2);
  }
  return data;
}

void Ppu::evaluateSprites() {
  int h;
  if (flagSpriteSize == 0) {
    h = 8;
  } else {
    h = 16;
  }
  auto count = 0;
  for (int i = 0; i < 64; i++) {
    auto y = oamData[i*4+0];
    auto a = oamData[i*4+2];
    auto x = oamData[i*4+3];
    auto row = ScanLine - int(y);
    if (row < 0 || row >= h) {
      continue;
    }
    if (count < 8) {
      spritePatterns[count] = fetchSpritePattern(i, row);
      spritePositions[count] = x;
      spritePriorities[count] = (a >> 5) & 1;
      spriteIndexes[count] = uint8_t(i);
    }
    count++;
  }
  if (count > 8) {
    count = 8;
    flagSpriteOverflow = 1;
  }
  spriteCount = count;
}

// tick updates Cycle, ScanLine and Frame counters
void Ppu::tick() {
  if (nmiDelay > 0) {
    nmiDelay--;
    if (nmiDelay == 0 && nmiOutput && nmiOccurred) {
      console->CPU->triggerNMI();
    }
  }

  if (flagShowBackground != 0 || flagShowSprites != 0) {
    if (f == 1 && ScanLine == 261 && Cycle == 339) {
      Cycle = 0;
      ScanLine = 0;
      Frame++;
      f ^= 1;
      return;
    }
  }
  Cycle++;
  if (Cycle > 340) {
    Cycle = 0;
    ScanLine++;
    if (ScanLine > 261) {
      ScanLine = 0;
      Frame++;
      f ^= 1;
    }
  }
}

// Step executes a single PPU cycle
void Ppu::Step() {
  tick();

  auto renderingEnabled = flagShowBackground != 0 || flagShowSprites != 0;
  auto preLine = ScanLine == 261;
  auto visibleLine = ScanLine < 240;
  // postLine = ScanLine == 240
  auto renderLine = preLine || visibleLine;
  auto preFetchCycle = Cycle >= 321 && Cycle <= 336;
  auto visibleCycle = Cycle >= 1 && Cycle <= 256;
  auto fetchCycle = preFetchCycle || visibleCycle;

  // background logic
  if (renderingEnabled) {
    if (visibleLine && visibleCycle) {
      renderPixel();
    }
    if (renderLine && fetchCycle) {
      tileData <<= 4;
      switch (Cycle % 8) {
        case 1:
          fetchNameTableByte();
          break;
        case 3:
          fetchAttributeTableByte();
          break;
        case 5:
          fetchLowTileByte();
          break;
        case 7:
          fetchHighTileByte();
          break;
        case 0:
          storeTileData();
          break;
      }
    }
    if (preLine && Cycle >= 280 && Cycle <= 304) {
      copyY();
    }
    if (renderLine) {
      if (fetchCycle && Cycle%8 == 0) {
        incrementX();
      }
      if (Cycle == 256) {
        incrementY();
      }
      if (Cycle == 257) {
        copyX();
      }
    }
  }

  // sprite logic
  if (renderingEnabled) {
    if (Cycle == 257) {
      if (visibleLine) {
        evaluateSprites();
      } else {
        spriteCount = 0;
      }
    }
  }

  // vblank logic
  if (ScanLine == 241 && Cycle == 1) {
    setVerticalBlank();
  }
  if (preLine && Cycle == 1) {
    clearVerticalBlank();
    flagSpriteZeroHit = 0;
    flagSpriteOverflow = 0;
  }
}
