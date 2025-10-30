#include "cartridge.h"
#include "statefile.h"

Cartridge::Cartridge(std::vector<uint8_t>&& prg, std::vector<uint8_t>&& chr, uint8_t mapper, uint8_t mirror, uint8_t battery)
  : PRG(std::move(prg)), CHR(std::move(chr)), mapper(mapper), mirror(mirror), battery(battery)
{
  SRAM.fill(0);
}

bool Cartridge::Save(StateFile* f) {
  f->Put(PRG);
  f->Put(CHR);
  f->Put(SRAM);
  f->Put(&mirror);
  return true;
}

bool Cartridge::Load(StateFile* f) {
  f->Get(PRG);
  f->Get(CHR);
  f->Get(SRAM);
  f->Get(&mirror);
  return true;
}
