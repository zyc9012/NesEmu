#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include "Mapper.h"
#include "Console.h"

Mapper* CreateMapper(Console* console);
std::string GetStateFileName(const char* romFile);
void log(const char* fmt, ...);

#endif