#include "utils.h"
#include "Mapper.h"
#include "Mapper2.h"
#include "Mapper4.h"
#include <stdio.h>
#include <stdarg.h>

Mapper* CreateMapper(Console* console)
{
	auto cartridge = console->_Cartridge;
	switch (cartridge->_Mapper)
	{
	case 0:
	case 2:
		return new Mapper2(cartridge);
	case 4:
		return new Mapper4(console, cartridge);
	default:
		log("unsupported mapper: %d", cartridge->_Mapper);
		return nullptr;
	}
}

void log(const char* fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);
	vfprintf(stderr, fmt, argptr);
	va_end(argptr);
	vfprintf(stderr, "\n", argptr);
}