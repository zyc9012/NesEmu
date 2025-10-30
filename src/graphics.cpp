#include "graphics.h"
#include "host.h"
#include "console.h"
#include "image.h"

Graphics::Graphics(Host* host)
{
  this->host = host;
  renderer = SDL_CreateRenderer(host->window, nullptr);
  int w, h;
  SDL_GetRenderOutputSize(renderer, &w, &h);
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
  memcpy(pixels, img->GetBytes(), img->GetByteLen());
  SDL_UnlockTexture(texture);
  SDL_RenderTexture(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}
