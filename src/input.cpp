#include "input.h"
#include "host.h"
#include "console.h"
#include "ppu.h"
#include "controller.h"

Input::Input(Host* host)
{
  this->host = host;

  int num_joysticks;
  SDL_JoystickID* joysticks = SDL_GetJoysticks(&num_joysticks);
  if (num_joysticks > 0) {
    joystick = SDL_OpenJoystick(joysticks[0]);
  }
  SDL_free(joysticks);
}

Input::~Input()
{
  if (joystick) {
    SDL_CloseJoystick(joystick);
    joystick = NULL;
  }
}

void Input::HandleKey(SDL_KeyboardEvent *key)
{
  auto turbo = (host->console->PPU->Frame % 6) < 3;
  auto buttons = host->console->controller1->GetButtons();
  auto isDown = key->down;

  switch (key->key) {
    case SDLK_Z:
      buttons[static_cast<uint8_t>(ControllerButton::A)] = isDown;
      break;
    case SDLK_A:
      buttons[static_cast<uint8_t>(ControllerButton::A)] = turbo && isDown;
      break;
    case SDLK_X:
      buttons[static_cast<uint8_t>(ControllerButton::B)] = isDown;
      break;
    case SDLK_S:
      buttons[static_cast<uint8_t>(ControllerButton::B)] = turbo && isDown;
      break;
    case SDLK_RSHIFT:
      buttons[static_cast<uint8_t>(ControllerButton::Select)] = isDown;
      break;
    case SDLK_RETURN:
      buttons[static_cast<uint8_t>(ControllerButton::Start)] = isDown;
      break;
    case SDLK_UP:
      buttons[static_cast<uint8_t>(ControllerButton::Up)] = isDown;
      break;
    case SDLK_DOWN:
      buttons[static_cast<uint8_t>(ControllerButton::Down)] = isDown;
      break;
    case SDLK_LEFT:
      buttons[static_cast<uint8_t>(ControllerButton::Left)] = isDown;
      break;
    case SDLK_RIGHT:
      buttons[static_cast<uint8_t>(ControllerButton::Right)] = isDown;
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
  host->console->SetButtons1(buttons);
}

void Input::HandleJoyAxis(SDL_JoyAxisEvent *axis)
{
  auto buttons = host->console->controller1->GetButtons();

  if (axis->value < -3200) {
    if (axis->axis == 0 || axis->axis == 2) {
      buttons[static_cast<uint8_t>(ControllerButton::Left)] = true;
    } else if (axis->axis == 1 || axis->axis == 3) {
      buttons[static_cast<uint8_t>(ControllerButton::Up)] = true;
    }
  } else if (axis->value > 3200) {
    if (axis->axis == 0 || axis->axis == 2) {
      buttons[static_cast<uint8_t>(ControllerButton::Right)] = true;
    } else if (axis->axis == 1 || axis->axis == 3) {
      buttons[static_cast<uint8_t>(ControllerButton::Down)] = true;
    }
  } else {
    buttons[static_cast<uint8_t>(ControllerButton::Left)] = false;
    buttons[static_cast<uint8_t>(ControllerButton::Right)] = false;
    buttons[static_cast<uint8_t>(ControllerButton::Up)] = false;
    buttons[static_cast<uint8_t>(ControllerButton::Down)] = false;
  }
  host->console->SetButtons1(buttons);
}

void Input::HandleJoyHat(SDL_JoyHatEvent *hat)
{
  auto buttons = host->console->controller1->GetButtons();

  switch (hat->value) {
  case SDL_HAT_CENTERED:
    buttons[static_cast<uint8_t>(ControllerButton::Left)] = false;
    buttons[static_cast<uint8_t>(ControllerButton::Right)] = false;
    buttons[static_cast<uint8_t>(ControllerButton::Up)] = false;
    buttons[static_cast<uint8_t>(ControllerButton::Down)] = false;
    break;
  case SDL_HAT_UP:
    buttons[static_cast<uint8_t>(ControllerButton::Up)] = true;
    break;
  case SDL_HAT_RIGHT:
    buttons[static_cast<uint8_t>(ControllerButton::Right)] = true;
    break;
  case SDL_HAT_DOWN:
    buttons[static_cast<uint8_t>(ControllerButton::Down)] = true;
    break;
  case SDL_HAT_LEFT:
    buttons[static_cast<uint8_t>(ControllerButton::Left)] = true;
    break;
  case SDL_HAT_RIGHTUP:
    buttons[static_cast<uint8_t>(ControllerButton::Right)] = true;
    buttons[static_cast<uint8_t>(ControllerButton::Up)] = true;
    break;
  case SDL_HAT_RIGHTDOWN:
    buttons[static_cast<uint8_t>(ControllerButton::Right)] = true;
    buttons[static_cast<uint8_t>(ControllerButton::Down)] = true;
    break;
  case SDL_HAT_LEFTUP:
    buttons[static_cast<uint8_t>(ControllerButton::Left)] = true;
    buttons[static_cast<uint8_t>(ControllerButton::Up)] = true;
    break;
  case SDL_HAT_LEFTDOWN:
    buttons[static_cast<uint8_t>(ControllerButton::Left)] = true;
    buttons[static_cast<uint8_t>(ControllerButton::Down)] = true;
    break;
  }
  host->console->SetButtons1(buttons);
}

void Input::HandleJoyButton(SDL_JoyButtonEvent *button)
{
  auto buttons = host->console->controller1->GetButtons();
  auto isDown = button->down;
  auto turbo = (host->console->PPU->Frame % 6) < 3;

  switch (button->button) {
    case 0:
      buttons[static_cast<uint8_t>(ControllerButton::A)] = isDown;
      break;
    case 1:
      buttons[static_cast<uint8_t>(ControllerButton::B)] = isDown;
      break;
    case 2:
      buttons[static_cast<uint8_t>(ControllerButton::A)] = turbo && isDown;
      break;
    case 3:
      buttons[static_cast<uint8_t>(ControllerButton::B)] = turbo && isDown;
      break;
  }
  host->console->SetButtons1(buttons);
}