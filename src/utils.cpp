#include <stdio.h>
#include <stdarg.h>
#include "utils.h"
#include "Mapper.h"
#include "Mapper1.h"
#include "Mapper2.h"
#include "Mapper4.h"
#include "Mapper10.h"
#include "Cartridge.h"

Mapper* CreateMapper(Console* console)
{
  auto cartridge = console->_Cartridge;
  switch (cartridge->_Mapper)
  {
  case 0:
  case 2:
    return new Mapper2(cartridge);
  case 1:
    return new Mapper1(cartridge);
  case 4:
    return new Mapper4(console, cartridge);
  case 10:
    return new Mapper10(cartridge);
  default:
    log("unsupported mapper: %d", cartridge->_Mapper);
    return nullptr;
  }
}

std::string GetStateFileName(const char* romFile)
{
  std::string s(romFile);
  int c = s.find_last_of('.');
  return s.substr(0, c) + ".state";
}

void log(const char* fmt, ...)
{
  va_list argptr;
  va_start(argptr, fmt);
  vfprintf(stderr, fmt, argptr);
  va_end(argptr);
  vfprintf(stderr, "\n", argptr);
}