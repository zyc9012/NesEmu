#pragma once
#include "Mapper.h"
#include <stdio.h>

Mapper* CreateMapper(Console* console);

void log(const char* fmt, ...);