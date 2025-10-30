#ifndef _PALETTE_H
#define _PALETTE_H

#include <cstdint>
#include <array>

inline constexpr size_t PALETTE_SIZE = 64;
extern std::array<uint32_t, PALETTE_SIZE> Palette;
void initPalette();

#endif