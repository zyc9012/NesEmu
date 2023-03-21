#include <stdint.h>
#include <string>
#include <stdio.h>
#include "Cartridge.h"
#include "iNes.h"
#include "utils.h"

uint32_t iNESFileMagic = 0x1a53454e;
uint32_t CPUFrequency = 1789773;

Cartridge* LoadNESFile(const char* path)
{
  // open file
  FILE *f = fopen(path, "rb");
  fseek(f, 0, SEEK_SET);

  // read file header
  iNESFileHeader header;
  fread(&header, sizeof(header), 1, f);

  // verify header magic number
  if (header.Magic != iNESFileMagic) {
    log("Invalid .nes file");
    return nullptr;
  }

  // mapper type
  auto mapper1 = header.Control1 >> 4;
  auto mapper2 = header.Control2 >> 4;
  auto mapper = mapper1 | mapper2 << 4;

  // mirroring type
  auto mirror1 = header.Control1 & 1;
  auto mirror2 = (header.Control1 >> 3) & 1;
  auto mirror = mirror1 | mirror2 << 1;

  // battery-backed RAM
  auto battery = (header.Control1 >> 1) & 1;

  // read trainer if present (unused)
  if ((header.Control1 & 4) == 4) {
    uint8_t* trainer = new uint8_t[512];
    fread(trainer, 512, 1, f);
    delete[] trainer;
  }

  // read prg-rom bank(s)
  int prg_len = int(header.NumPRG) * 16384;
  auto prg = new uint8_t[prg_len];
  fread(prg, prg_len, 1, f);

  int chr_len;
  uint8_t* chr;
  // provide chr-rom/ram if not in file
  if (header.NumCHR == 0) {
    chr_len = 8192;
    chr = new uint8_t[8192];
  }
  // read chr-rom bank(s)
  else {
    chr_len = int(header.NumCHR) * 8192;
    chr = new uint8_t[chr_len];
    fread(chr, chr_len, 1, f);
  }

  fclose(f);
  
  log("Mapper: %d", mapper);
  log("Mirror: %d", mirror);
  log("Battery: %d", battery);
  log("PRG: %dK", header.NumPRG * 16);
  log("CHR: %dK", header.NumCHR * 8);

  // success
  return new Cartridge(prg, prg_len, chr, chr_len, mapper, mirror, battery);
}
