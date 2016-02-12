#pragma once
#include "Mapper.h"
#include "Console.h"

Mapper* CreateMapper(Console* console);

void log(const char* fmt, ...);