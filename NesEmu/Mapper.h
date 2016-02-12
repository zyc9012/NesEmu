#pragma once
#include <stdint.h>

class Mapper
{
public:
	//Mapper();
	//~Mapper();

	virtual uint8_t Read(uint16_t address) = 0;
	virtual void Write(uint16_t address, uint8_t value) = 0;
	virtual void Step() = 0;
	virtual bool Save() = 0;
	virtual bool Load() = 0;
};
