#ifndef _INPUT_H
#define _INPUT_H

#include <SDL2/SDL.h>

class Host;

class Input
{
public:
  Input(Host* host);
  ~Input();
  void HandleKey(SDL_KeyboardEvent *key);
  void HandleJoyAxis(SDL_JoyAxisEvent *axis);
  void HandleJoyHat(SDL_JoyHatEvent *hat);
  void HandleJoyButton(SDL_JoyButtonEvent *button);

private:
  Host* host;
  SDL_Joystick* joystick = NULL;
};

#endif