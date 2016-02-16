#include "Cartridge.h"
#include "StateFile.h"


Cartridge::Cartridge(uint8_t* prg, int prg_len, uint8_t* chr, int chr_len, uint8_t mapper, uint8_t mirror, uint8_t battery)
{
	PRG = prg;
	PRG_len = prg_len;
	CHR = chr;
	CHR_len = chr_len;
	SRAM = new uint8_t[0x2000];
	_Mapper = mapper;
	Mirror = mirror;
	Battery = battery;
}


Cartridge::~Cartridge()
{
}

bool Cartridge::Save(StateFile* f) {
	f->Put(SRAM, 0x2000);
	f->Put(&Mirror);
	return true;
}

bool Cartridge::Load(StateFile* f) {
	f->Get(SRAM, 0x2000);
	f->Get(&Mirror);
	return true;
}
