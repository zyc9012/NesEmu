#pragma once
#include "Config.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Console;
class Audio;
class Graphics;
class Input;

class Host
{
public:
	Host(Config* config);
	~Host();

	void Run();
	void Step();

	Config* config;
	Console* console;
	Audio* audio;
	Graphics* gfx;
	Input* input;
	GLFWwindow* window;
};

