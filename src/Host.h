#ifndef _HOST_H
#define _HOST_H

#include "Config.h"
#include "SDL2/SDL.h"

class Console;
class Audio;
class Graphics;
class Input;

class Host
{
public:
  Host(Config* config);
  ~Host();

  void Run();
  void Step();

  Config* config;
  Console* console;
  Audio* audio;
  Graphics* gfx;
  Input* input;
  SDL_Window* window;
};

#endif