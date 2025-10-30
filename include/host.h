#ifndef _HOST_H
#define _HOST_H

#include "config.h"
#include <SDL3/SDL.h>
#include <memory>

class Console;
class Audio;
class Graphics;
class Input;

class Host
{
public:
  explicit Host(Config* config);
  ~Host();
  
  Host(const Host&) = delete;
  Host& operator=(const Host&) = delete;
  Host(Host&&) = delete;
  Host& operator=(Host&&) = delete;

  void Run();
  void Step();

  Config* config{nullptr};
  std::unique_ptr<Console> console;
  std::unique_ptr<Audio> audio;
  std::unique_ptr<Graphics> gfx;
  std::unique_ptr<Input> input;
  SDL_Window* window{nullptr};
};

#endif