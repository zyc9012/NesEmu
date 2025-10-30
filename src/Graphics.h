#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <SDL3/SDL.h>

class Host;

class Graphics
{
public:
  Graphics(Host* host);
  ~Graphics();

  void DrawFrame();

private:
  Host* host;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
};

#endif