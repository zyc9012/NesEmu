#include "host.h"
#include "graphics.h"
#include "audio.h"
#include "input.h"
#include "console.h"
#include "utils.h"

Host::Host(Config* config)
{
  this->config = config;
  console = new Console(config->RomFile.c_str());
  log("Loaded: %s", config->RomFile.c_str());
  config->StateFileName = GetStateFileName(config->RomFile.c_str());
  console->Reset();

  bool joystick_enabled = true;
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
    log("Failed to initialize SDL with joystick support. Retrying without.");
    joystick_enabled = false;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
      log("Unable to initialize SDL: %s", SDL_GetError());
      return;
    }
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  window = SDL_CreateWindow("NesEmu", 256, 240, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  SDL_GL_SetSwapInterval(1);

  audio = new Audio();
  console->SetAudioChannel(audio);
  console->SetAudioSampleRate(44100);
  audio->Start();

  gfx = new Graphics(this);

  input = new Input(this);
}


Host::~Host()
{
}

void Host::Step()
{
  console->StepSeconds(config->TimePerFrame);
  gfx->DrawFrame();
}

void Host::Run()
{
  bool quitRequested = false;
  Uint64 frameTimeLimit = Uint64(config->TimePerFrame * 1000);
  Uint64 lastTicks = SDL_GetTicks();

  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        quitRequested = true;
        break;
      case SDL_EVENT_KEY_DOWN:
      case SDL_EVENT_KEY_UP:
        input->HandleKey(&event.key);
        break;
      case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        input->HandleJoyAxis(&event.jaxis);
        break;
      case SDL_EVENT_JOYSTICK_HAT_MOTION:
        input->HandleJoyHat(&event.jhat);
        break;
      case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
      case SDL_EVENT_JOYSTICK_BUTTON_UP:
        input->HandleJoyButton(&event.jbutton);
        break;
      default:
        break;
      }
    }

    if (quitRequested) {
      break;
    }

    Step();

    Uint64 elapsed = SDL_GetTicks() - lastTicks;
    if (elapsed < frameTimeLimit) {
      SDL_Delay(frameTimeLimit - elapsed);
    }
    lastTicks = SDL_GetTicks();
  }

  log("Shutting down...");
  delete console;
  delete audio;
  delete gfx;
  delete input;

  SDL_Quit();
}

#undef main
int main(int argc, char** argv)
{
  Config config;
  if (!ParseCommandLine(config, argc, argv)) {
    return 1;
  }
  Host host(&config);
  host.Run();
  return 0;
}