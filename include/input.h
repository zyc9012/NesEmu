#ifndef _INPUT_H
#define _INPUT_H

#include <SDL3/SDL.h>

class Host;

class Input
{
public:
  explicit Input(Host* host);
  ~Input();
  
  Input(const Input&) = delete;
  Input& operator=(const Input&) = delete;
  Input(Input&&) = delete;
  Input& operator=(Input&&) = delete;
  
  void HandleKey(SDL_KeyboardEvent* key);
  void HandleJoyAxis(SDL_JoyAxisEvent* axis);
  void HandleJoyHat(SDL_JoyHatEvent* hat);
  void HandleJoyButton(SDL_JoyButtonEvent* button);

private:
  Host* host{nullptr};
  SDL_Joystick* joystick{nullptr};
};

#endif