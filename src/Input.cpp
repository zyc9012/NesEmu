#include "Input.h"
#include "Host.h"
#include "Console.h"
#include "Ppu.h"
#include "Controller.h"

Input::Input(Host* host)
{
  this->host = host;

  if (SDL_NumJoysticks() > 0) {
    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(0);
  }
}

Input::~Input()
{
  if (joystick) {
    SDL_JoystickClose(joystick);
    joystick = NULL;
  }
}

void Input::HandleKey(SDL_KeyboardEvent *key)
{
  auto turbo = (host->console->PPU->Frame % 6) < 3;
  auto buttons = host->console->Controller1->buttons;
  auto isDown = key->state == SDL_PRESSED;

  switch (key->keysym.sym) {
    case SDLK_z:
      buttons[ButtonA] = isDown;
      break;
    case SDLK_a:
      buttons[ButtonA] = turbo && isDown;
      break;
    case SDLK_x:
      buttons[ButtonB] = isDown;
      break;
    case SDLK_s:
      buttons[ButtonB] = turbo && isDown;
      break;
    case SDLK_RSHIFT:
      buttons[ButtonSelect] = isDown;
      break;
    case SDLK_RETURN:
      buttons[ButtonStart] = isDown;
      break;
    case SDLK_UP:
      buttons[ButtonUp] = isDown;
      break;
    case SDLK_DOWN:
      buttons[ButtonDown] = isDown;
      break;
    case SDLK_LEFT:
      buttons[ButtonLeft] = isDown;
      break;
    case SDLK_RIGHT:
      buttons[ButtonRight] = isDown;
      break;
    case SDLK_F5:
      if (isDown) {
        host->console->SaveState(host->config->StateFileName.c_str());
      }
      break;
    case SDLK_F9:
      if (isDown) {
        host->console->LoadState(host->config->StateFileName.c_str());
      }
      break;
    default:
      break;
  }
}

void Input::HandleJoyAxis(SDL_JoyAxisEvent *axis)
{
  auto buttons = host->console->Controller1->buttons;

  if (axis->value < -3200) {
    if (axis->axis == 0 || axis->axis == 2) {
      buttons[ButtonLeft] = true;
    } else if (axis->axis == 1 || axis->axis == 3) {
      buttons[ButtonUp] = true;
    }
  } else if (axis->value > 3200) {
    if (axis->axis == 0 || axis->axis == 2) {
      buttons[ButtonRight] = true;
    } else if (axis->axis == 1 || axis->axis == 3) {
      buttons[ButtonDown] = true;
    }
  } else {
    buttons[ButtonLeft] = false;
    buttons[ButtonRight] = false;
    buttons[ButtonUp] = false;
    buttons[ButtonDown] = false;
  }
}

void Input::HandleJoyHat(SDL_JoyHatEvent *hat)
{
  auto buttons = host->console->Controller1->buttons;

  switch (hat->value) {
  case SDL_HAT_CENTERED:
    buttons[ButtonLeft] = false;
    buttons[ButtonRight] = false;
    buttons[ButtonUp] = false;
    buttons[ButtonDown] = false;
    break;
  case SDL_HAT_UP:
    buttons[ButtonUp] = true;
    break;
  case SDL_HAT_RIGHT:
    buttons[ButtonRight] = true;
    break;
  case SDL_HAT_DOWN:
    buttons[ButtonDown] = true;
    break;
  case SDL_HAT_LEFT:
    buttons[ButtonLeft] = true;
    break;
  case SDL_HAT_RIGHTUP:
    buttons[ButtonRight] = true;
    buttons[ButtonUp] = true;
    break;
  case SDL_HAT_RIGHTDOWN:
    buttons[ButtonRight] = true;
    buttons[ButtonDown] = true;
    break;
  case SDL_HAT_LEFTUP:
    buttons[ButtonLeft] = true;
    buttons[ButtonUp] = true;
    break;
  case SDL_HAT_LEFTDOWN:
    buttons[ButtonLeft] = true;
    buttons[ButtonDown] = true;
    break;
  }
}

void Input::HandleJoyButton(SDL_JoyButtonEvent *button)
{
  auto buttons = host->console->Controller1->buttons;
  auto isDown = button->state == SDL_PRESSED;
  auto turbo = (host->console->PPU->Frame % 6) < 3;

  switch (button->button) {
    case 0:
      buttons[ButtonA] = isDown;
      break;
    case 1:
      buttons[ButtonB] = isDown;
      break;
    case 2:
      buttons[ButtonA] = turbo && isDown;
      break;
    case 3:
      buttons[ButtonB] = turbo && isDown;
      break;
  }
}