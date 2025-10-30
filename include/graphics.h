#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <SDL3/SDL.h>

class Host;

class Graphics
{
public:
  explicit Graphics(Host* host);
  ~Graphics();
  
  Graphics(const Graphics&) = delete;
  Graphics& operator=(const Graphics&) = delete;
  Graphics(Graphics&&) = delete;
  Graphics& operator=(Graphics&&) = delete;

  void DrawFrame();

private:
  Host* host{nullptr};
  SDL_Texture* texture{nullptr};
  SDL_Renderer* renderer{nullptr};
};

#endif