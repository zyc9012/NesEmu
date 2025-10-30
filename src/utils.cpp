#include <cstdio>
#include <cstdarg>
#include <memory>
#include "utils.h"
#include "mapper.h"
#include "mapper1.h"
#include "mapper2.h"
#include "mapper3.h"
#include "mapper4.h"
#include "mapper10.h"
#include "cartridge.h"
#include "console.h"

std::unique_ptr<Mapper> CreateMapper(Console* console)
{
  auto* cartridge = console->cartridge.get();
  switch (cartridge->mapper)
  {
  case 0:
  case 2:
    return std::make_unique<Mapper2>(cartridge);
  case 1:
    return std::make_unique<Mapper1>(cartridge);
  case 3:
    return std::make_unique<Mapper3>(cartridge);
  case 4:
    return std::make_unique<Mapper4>(console, cartridge);
  case 10:
    return std::make_unique<Mapper10>(cartridge);
  default:
    log("unsupported mapper: %d", cartridge->mapper);
    return nullptr;
  }
}

std::string GetStateFileName(const std::string& romFile)
{
  const auto pos = romFile.find_last_of('.');
  if (pos != std::string::npos) {
    return romFile.substr(0, pos) + ".state";
  }
  return romFile + ".state";
}

void log(const char* fmt, ...)
{
  va_list argptr;
  va_start(argptr, fmt);
  std::vfprintf(stderr, fmt, argptr);
  va_end(argptr);
  std::fprintf(stderr, "\n");
}