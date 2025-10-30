#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <memory>

class Mapper;
class Console;

std::unique_ptr<Mapper> CreateMapper(Console* console);
std::string GetStateFileName(const std::string& romFile);

void log(const char* fmt, ...);

#endif