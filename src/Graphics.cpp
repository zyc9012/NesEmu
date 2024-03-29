#include "Graphics.h"
#include "Host.h"
#include "Console.h"
#include "Image.h"

Graphics::Graphics(Host* host)
{
  this->host = host;
  renderer = SDL_CreateRenderer(host->window, -1, SDL_RENDERER_ACCELERATED);
  int w, h;
  SDL_GetRendererOutputSize(renderer, &w, &h);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
  SDL_SetTextureScaleMode(texture, host->config->FilterMode);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  if (host->config->Scale != 1) {
    SDL_SetWindowSize(host->window, 256 * host->config->Scale, 240 * host->config->Scale);
  }
}

Graphics::~Graphics()
{
}

void Graphics::DrawFrame()
{
  Image* img = host->console->Buffer();
  void* pixels;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixels, &pitch);
  memcpy(pixels, img->bytes, img->byte_len);
  SDL_UnlockTexture(texture);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}
