#include "Host.h"
#include "Graphics.h"
#include "Audio.h"
#include "Input.h"
#include "Console.h"
#include "utils.h"

Host::Host(Config* config)
{
  this->config = config;
  console = new Console(config->RomFile.c_str());
  log("Loaded: %s", config->RomFile.c_str());
  config->StateFileName = GetStateFileName(config->RomFile.c_str());
  console->Reset();

  bool joystick_enabled = true;
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0) {
    log("Failed to initialize SDL with joystick support. Retrying without.");
    joystick_enabled = false;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

  window = SDL_CreateWindow("NesEmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 240, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

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
  Uint64 lastTicks = SDL_GetTicks64();

  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quitRequested = true;
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        input->HandleKey(&event.key);
        break;
      case SDL_JOYAXISMOTION:
        input->HandleJoyAxis(&event.jaxis);
        break;
      case SDL_JOYHATMOTION:
        input->HandleJoyHat(&event.jhat);
        break;
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
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

    Uint64 elapsed = SDL_GetTicks64() - lastTicks;
    if (elapsed < frameTimeLimit) {
      SDL_Delay(frameTimeLimit - elapsed);
    }
    lastTicks = SDL_GetTicks64();
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