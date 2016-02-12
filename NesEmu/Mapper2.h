#pragma once
#include <stdint.h>
class Mapper;
class Cartridge;

class Mapper2 : public Mapper
{
public:
	Mapper2(Cartridge* cartridge);
	~Mapper2();

	virtual uint8_t Read(uint16_t address) override;
	virtual void Write(uint16_t address, uint8_t value) override;
	virtual void Step() override;
	virtual bool Save() override;
	virtual bool Load() override;

	Cartridge* cartridge;
	int prgBanks;
	int prgBank1;
	int prgBank2;
};

