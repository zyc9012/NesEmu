#ifndef _INES_H
#define _INES_H

#include <cstdint>
#include <array>
#include <string>
#include <memory>

inline constexpr uint32_t iNESFileMagic = 0x1a53454e;
inline constexpr uint32_t CPUFrequency = 1789773;

class Cartridge;

struct iNESFileHeader
{
  uint32_t Magic;                 // iNES magic number
  uint8_t NumPRG;                 // number of PRG-ROM banks (16KB each)
  uint8_t NumCHR;                 // number of CHR-ROM banks (8KB each)
  uint8_t Control1;               // control bits
  uint8_t Control2;               // control bits
  uint8_t NumRAM;                 // PRG-RAM size (x 8KB)
  std::array<uint8_t, 7> unused;  // unused padding
};

std::unique_ptr<Cartridge> LoadNESFile(const std::string& path);

#endif