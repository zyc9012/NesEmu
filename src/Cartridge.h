#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

#include <stdint.h>
class StateFile;

class Cartridge
{
public:
  Cartridge(uint8_t* prg, int prg_len, uint8_t* chr, int chr_len, uint8_t mapper, uint8_t mirror, uint8_t battery);
  ~Cartridge();

  bool Save(StateFile*);
  bool Load(StateFile*);

  uint8_t* PRG;       // PRG-ROM banks
  int PRG_len;
  uint8_t* CHR;       // CHR-ROM banks
  int CHR_len;
  uint8_t* SRAM;      // Save RAM
  uint8_t _Mapper;    // mapper type
  uint8_t Mirror;     // mirroring mode
  uint8_t Battery;    // battery present
};

#endif