#include <cstdint>
#include <string>
#include <cstdio>
#include <vector>
#include <memory>
#include "cartridge.h"
#include "ines.h"
#include "utils.h"

std::unique_ptr<Cartridge> LoadNESFile(const std::string& path)
{
  // open file
  FILE *f = fopen(path.c_str(), "rb");
  if (!f) {
    log("Failed to open file: %s", path.c_str());
    return nullptr;
  }
  fseek(f, 0, SEEK_SET);

  // read file header
  iNESFileHeader header;
  if (fread(&header, sizeof(header), 1, f) != 1) {
    log("Failed to read file header");
    fclose(f);
    return nullptr;
  }

  // verify header magic number
  if (header.Magic != iNESFileMagic) {
    log("Invalid .nes file");
    fclose(f);
    return nullptr;
  }

  // mapper type
  const auto mapper1 = header.Control1 >> 4;
  const auto mapper2 = header.Control2 >> 4;
  const auto mapper = static_cast<uint8_t>(mapper1 | (mapper2 << 4));

  // mirroring type
  const auto mirror1 = header.Control1 & 1;
  const auto mirror2 = (header.Control1 >> 3) & 1;
  const auto mirror = static_cast<uint8_t>(mirror1 | (mirror2 << 1));

  // battery-backed RAM
  const auto battery = static_cast<uint8_t>((header.Control1 >> 1) & 1);

  // read trainer if present (unused)
  if ((header.Control1 & 4) == 4) {
    std::vector<uint8_t> trainer(512);
    fread(trainer.data(), 512, 1, f);
  }

  // read prg-rom bank(s)
  const size_t prg_len = static_cast<size_t>(header.NumPRG) * 16384;
  std::vector<uint8_t> prg(prg_len);
  fread(prg.data(), prg_len, 1, f);

  size_t chr_len;
  std::vector<uint8_t> chr;
  // provide chr-rom/ram if not in file
  if (header.NumCHR == 0) {
    chr_len = 8192;
    chr.resize(chr_len, 0);
  }
  // read chr-rom bank(s)
  else {
    chr_len = static_cast<size_t>(header.NumCHR) * 8192;
    chr.resize(chr_len);
    fread(chr.data(), chr_len, 1, f);
  }

  fclose(f);
  
  log("Mapper: %d", mapper);
  log("Mirror: %d", mirror);
  log("Battery: %d", battery);
  log("PRG: %dK", header.NumPRG * 16);
  log("CHR: %dK", header.NumCHR * 8);

  // success
  return std::make_unique<Cartridge>(std::move(prg), std::move(chr), mapper, mirror, battery);
}
