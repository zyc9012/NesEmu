#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Input.h"
#include "Host.h"
#include "../Console.h"
#include "../Ppu.h"
#include "../Controller.h"

Host* __host;
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_F5:
			__host->console->SaveState(__host->config->StateFileName.c_str()); break;
		case GLFW_KEY_F9:
			__host->console->LoadState(__host->config->StateFileName.c_str()); break;
		}
	}
}

Input::Input(Host* host)
{
	this->host = host;
	__host = host;
	glfwSetKeyCallback(host->window, KeyCallback);
}


Input::~Input()
{
}


bool Input::ReadKey(int key) {
	return glfwGetKey(host->window, key) == GLFW_PRESS;
}

const float* Input::ReadJoyStickAxes() {
	int count;
	return glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
}

const unsigned char* Input::ReadJoyStickButtons() {
	int count;
	return glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
}

void Input::UpdateControllers()
{
	auto joyAxes = ReadJoyStickAxes();
	auto joyButtons = ReadJoyStickButtons();
	auto turbo = (host->console->PPU->Frame % 6) < 3;
	auto buttons = host->console->Controller1->buttons;
	buttons[ButtonA] = ReadKey(GLFW_KEY_Z) || (turbo && ReadKey(GLFW_KEY_A)) || joyButtons[0];
	buttons[ButtonB] = ReadKey(GLFW_KEY_X) || (turbo && ReadKey(GLFW_KEY_S)) || joyButtons[1];
	buttons[ButtonSelect] = ReadKey(GLFW_KEY_RIGHT_SHIFT) || joyButtons[4];
	buttons[ButtonStart] = ReadKey(GLFW_KEY_ENTER) || joyButtons[5];
	buttons[ButtonUp] = ReadKey(GLFW_KEY_I) || (joyAxes[1] < -0.5);
	buttons[ButtonDown] = ReadKey(GLFW_KEY_K) || (joyAxes[1] > 0.5);
	buttons[ButtonLeft] = ReadKey(GLFW_KEY_J) || (joyAxes[0] < -0.5);
	buttons[ButtonRight] = ReadKey(GLFW_KEY_L) || (joyAxes[0] > 0.5);
}