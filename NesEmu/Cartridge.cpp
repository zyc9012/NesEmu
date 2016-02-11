#include "Cartridge.h"



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

bool Cartridge::Save() {
	//encoder.Encode(cartridge.SRAM);
	//encoder.Encode(cartridge.Mirror);
	//return nil;
	return false;
}

bool Cartridge::Load() {
	//decoder.Decode(&cartridge.SRAM);
	//decoder.Decode(&cartridge.Mirror);
	//return nil;
	return false;
}
