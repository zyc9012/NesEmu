#pragma once
#include <string>
#include <stdint.h>
#include <GL/glew.h>
using namespace std;

typedef struct Config {
	string RomFile;
	string StateFileName;
	float Scale = 2;
	uint16_t FilterMode = GL_NEAREST;
	uint32_t TargetFps = 60;
	double TimePerFrame;
} Config;

bool ParseCommandLine(Config& config, int argc, char** argv);