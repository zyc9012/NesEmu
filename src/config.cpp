#include "config.h"
#include "utils.h"
#include <cstring>
#include <string>

bool ParseCommandLine(Config& config, int argc, char** argv)
{
  bool romSpecified = false;
  for (int i = 1; i < argc;)
  {
    if (argv[i][0] != '-') {
      config.RomFile = std::string(argv[i]);
      romSpecified = true;
    }
    else if (std::strcmp("-s", argv[i]) == 0) {
      i++;
      config.Scale = std::stof(argv[i]);
    }
    else if (std::strcmp("-f", argv[i]) == 0) {
      i++;
      if (std::strcmp("nearest", argv[i]) == 0) config.FilterMode = SDL_SCALEMODE_NEAREST;
      else if (std::strcmp("linear", argv[i]) == 0) config.FilterMode = SDL_SCALEMODE_LINEAR;
      else if (std::strcmp("best", argv[i]) == 0) config.FilterMode = SDL_SCALEMODE_LINEAR; // "best" maps to linear in SDL3
      else log("Unknown filter mode, using \"linear\"");
    }
    i++;
  }

  if (!romSpecified)
  {
    log("Usage");
    log("NesEmu rom_file [-s screen_scale] [-f image_filter]");
    log("options:");
    log("    -s    Scale to the orignal screen size (default 2.0)");
    log("    -f    Image filter, one of [nearest|linear|best] (default linear)");
    return false;
  }

  config.TimePerFrame = 1.0 / static_cast<double>(config.TargetFps);

  return true;
}