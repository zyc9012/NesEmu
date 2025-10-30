#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>
#include <cstdint>
#include <SDL3/SDL.h>

struct Config {
  std::string RomFile;
  std::string StateFileName;
  float Scale{2.0f};
  SDL_ScaleMode FilterMode{SDL_SCALEMODE_LINEAR};
  uint32_t TargetFps{60};
  double TimePerFrame{0.0};
};

bool ParseCommandLine(Config& config, int argc, char** argv);

#endif