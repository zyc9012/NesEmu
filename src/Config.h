#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>
#include <stdint.h>
#include <SDL3/SDL.h>
using namespace std;

typedef struct Config {
  string RomFile;
  string StateFileName;
  float Scale = 2;
  SDL_ScaleMode FilterMode = SDL_SCALEMODE_LINEAR;
  uint32_t TargetFps = 60;
  double TimePerFrame;
} Config;

bool ParseCommandLine(Config& config, int argc, char** argv);

#endif