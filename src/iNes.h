#ifndef _INES_H
#define _INES_H

#include <stdint.h>

extern uint32_t iNESFileMagic;
extern uint32_t CPUFrequency;

class Cartridge;

typedef struct iNESFileHeader
{
  uint32_t Magic;       // iNES magic number
  uint8_t	NumPRG;       // number of PRG-ROM banks (16KB each)
  uint8_t	NumCHR;       // number of CHR-ROM banks (8KB each)
  uint8_t	Control1;     // control bits
  uint8_t	Control2;     // control bits
  uint8_t	NumRAM;       // PRG-RAM size (x 8KB)
  uint8_t	unused[7];    // unused padding
} iNESFileHeader;


Cartridge* LoadNESFile(const char* path);

#endif