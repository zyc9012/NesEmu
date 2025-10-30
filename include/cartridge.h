#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

#include <cstdint>
#include <vector>
#include <array>

class StateFile;

class Cartridge
{
public:
  Cartridge(std::vector<uint8_t>&& prg, std::vector<uint8_t>&& chr, uint8_t mapper, uint8_t mirror, uint8_t battery);
  ~Cartridge() = default;
  
  Cartridge(const Cartridge&) = delete;
  Cartridge& operator=(const Cartridge&) = delete;
  Cartridge(Cartridge&&) = default;
  Cartridge& operator=(Cartridge&&) = default;

  bool Save(StateFile*);
  bool Load(StateFile*);

  std::vector<uint8_t> PRG;       // PRG-ROM banks
  std::vector<uint8_t> CHR;       // CHR-ROM banks
  std::array<uint8_t, 0x2000> SRAM{};  // Save RAM (8KB)
  uint8_t mapper{0};              // mapper type
  uint8_t mirror{0};              // mirroring mode
  uint8_t battery{0};             // battery present
};

#endif