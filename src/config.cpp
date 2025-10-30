#include "config.h"
#include "utils.h"

bool ParseCommandLine(Config& config, int argc, char** argv)
{
  bool romSpecified = false;
  for (int i = 1; i < argc;)
  {
    if (argv[i][0] != '-') {
      config.RomFile = string(argv[i]);
      romSpecified = true;
    }
    else if (strcmp("-s", argv[i]) == 0) {
      i++;
      config.Scale = (float)std::stof(argv[i]);
    }
    else if (strcmp("-f", argv[i]) == 0) {
      i++;
      if (strcmp("nearest", argv[i]) == 0) config.FilterMode = SDL_SCALEMODE_NEAREST;
      else if (strcmp("linear", argv[i]) == 0) config.FilterMode = SDL_SCALEMODE_LINEAR;
      else if (strcmp("best", argv[i]) == 0) config.FilterMode = SDL_SCALEMODE_LINEAR; // "best" maps to linear in SDL3
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

  config.TimePerFrame = 1.0 / config.TargetFps;

  return true;
}