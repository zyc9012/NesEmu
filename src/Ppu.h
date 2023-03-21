#ifndef _PPU_H
#define _PPU_H

#include <stdint.h>
class Console;
class Memory;
class Image;
class StateFile;

class Ppu
{
public:
  Ppu(Console* console);
  ~Ppu();

  bool Save(StateFile*);
  bool Load(StateFile*);

  Memory* memory;
  Console* console; // reference to parent object

  int	Cycle;        // 0-340
  int	ScanLine;     // 0-261, 0-239=visible, 240=post, 241-260=vblank, 261=pre
  uint64_t Frame;   // frame counter

  // storage variables
  uint8_t	paletteData[32];
  uint8_t	nameTableData[2048];
  uint8_t	oamData[256];
  Image* front;
  Image* back;

  // PPU registers
  uint16_t v;  // current vram address (15 bit)
  uint16_t t;  // temporary vram address (15 bit)
  uint8_t  x;    // fine x scroll (3 bit)
  uint8_t  w;    // write toggle (1 bit)
  uint8_t  f;    // even/odd frame flag (1 bit)

  uint8_t	_register;

  // NMI flags
  bool	nmiOccurred;
  bool	nmiOutput;
  bool	nmiPrevious;
  uint8_t	nmiDelay;

  // background temporary variables
  uint8_t	nameTableByte;
  uint8_t	attributeTableByte;
  uint8_t	lowTileByte;
  uint8_t	highTileByte;
  uint64_t tileData;

  // sprite temporary variables
  int	spriteCount;
  uint32_t spritePatterns[8];
  uint8_t	spritePositions[8];
  uint8_t	spritePriorities[8];
  uint8_t	spriteIndexes[8];

  // $2000 PPUCTRL
  uint8_t	flagNameTable;        // 0: $2000; 1: $2400; 2: $2800; 3: $2C00
  uint8_t	flagIncrement;        // 0: add 1; 1: add 32
  uint8_t	flagSpriteTable;      // 0: $0000; 1: $1000; ignored in 8x16 mode
  uint8_t	flagBackgroundTable;  // 0: $0000; 1: $1000
  uint8_t	flagSpriteSize;       // 0: 8x8; 1: 8x16
  uint8_t	flagMasterSlave;      // 0: read EXT; 1: write EXT

  // $2001 PPUMASK
  uint8_t	flagGrayscale;           // 0: color; 1: grayscale
  uint8_t	flagShowLeftBackground;  // 0: hide; 1: show
  uint8_t	flagShowLeftSprites;     // 0: hide; 1: show
  uint8_t	flagShowBackground;      // 0: hide; 1: show
  uint8_t	flagShowSprites;         // 0: hide; 1: show
  uint8_t	flagRedTint;             // 0: normal; 1: emphasized
  uint8_t	flagGreenTint;           // 0: normal; 1: emphasized
  uint8_t	flagBlueTint;            // 0: normal; 1: emphasized

  // $2002 PPUSTATUS
  uint8_t	flagSpriteZeroHit;
  uint8_t	flagSpriteOverflow;

  // $2003 OAMADDR
  uint8_t	oamAddress;

  // $2007 PPUDATA
  uint8_t	bufferedData;  // for buffered reads

  void Reset();
  uint8_t readRegister(uint16_t address);
  uint8_t readPalette(uint16_t address);
  void writeRegister(uint16_t address, uint8_t value);
  void writePalette(uint16_t address, uint8_t value);
  void writeControl(uint8_t value);
  void writeMask(uint8_t value);
  uint8_t readStatus();
  void writeOAMAddress(uint8_t value);
  uint8_t readOAMData();
  void writeOAMData(uint8_t value);
  void writeScroll(uint8_t value);
  void writeAddress(uint8_t value);
  uint8_t readData();
  void writeData(uint8_t value);
  void writeDMA(uint8_t value);
  void incrementX();
  void incrementY();
  void copyX();
  void copyY();
  void nmiChange();
  void setVerticalBlank();
  void clearVerticalBlank();
  void fetchNameTableByte();
  void fetchAttributeTableByte();
  void fetchLowTileByte();
  void fetchHighTileByte();
  void storeTileData();
  uint32_t fetchTileData();
  inline uint8_t backgroundPixel();
  inline void spritePixel(uint8_t& index, uint8_t& sprite);
  void renderPixel();
  uint32_t fetchSpritePattern(int i, int row);
  void evaluateSprites();
  void tick();
  void Step();
};

#endif