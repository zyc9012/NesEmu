#include "Host.h"
#include "Graphics.h"
#include "Audio.h"
#include "Input.h"
#include "../Console.h"
#include "../utils.h"

#ifdef _WIN32
#include <windows.h>
#define SLEEP(n) Sleep(n)
#else
#include <time.h>
#include <string.h>
#define SLEEP(n) timespec __t;\
                 __t.tv_sec = 0;\
                 __t.tv_nsec = n;\
                 nanosleep(&__t, NULL);
#endif


Host::Host(Config* config)
{
	this->config = config;
	console = new Console(config->RomFile.c_str());
	log("Loaded: %s", config->RomFile.c_str());
	config->StateFileName = GetStateFileName(config->RomFile.c_str());
	console->Reset();

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, 0);
	window = glfwCreateWindow(int(256 * config->Scale), int(240 * config->Scale), "Nes Emulator", NULL, NULL);
	glfwMakeContextCurrent(window);

	audio = new Audio();
	console->SetAudioChannel(audio);
	console->SetAudioSampleRate(44100);
	audio->Start();

	gfx = new Graphics(this);

	input = new Input(this);
}


Host::~Host()
{
	log("Shutting down...");
	delete console;
	delete audio;
	delete gfx;
	delete input;
	glfwTerminate();
}

void Host::Step()
{
	input->UpdateControllers();

	console->StepSeconds(config->TimePerFrame);

	gfx->DrawFrame();
}

void Host::Run()
{
	while (!glfwWindowShouldClose(window))
	{
		Step();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}


int main(int argc, char** argv)
{
	Config config;
	if (!ParseCommandLine(config, argc, argv)) {
		return 400;
	}
	Host host(&config);
	host.Run();
	return 0;
}