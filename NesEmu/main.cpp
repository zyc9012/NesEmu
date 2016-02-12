#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <windows.h>
#include <stdio.h>
#include "Image.h"
#include "Console.h"
#include "BlockingQueue.h"
#include "Audio.h"
#include "Controller.h"
#include "utils.h"
#include "Ppu.h"

const unsigned int TARGET_FPS = 30;
const double TIME_PER_FRAME = 1.0 / TARGET_FPS;
double lastTime;

GLuint texture;
Console* console;
BlockingQueue<float>* channel;

void CreateTexture()
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SetTexture(Image* img)
{
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, img->bytes);
}

void DrawBuffer(GLFWwindow* window)
{
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	float s1 = float(w) / 256;
	float s2 = float(h) / 240;
	float f = float(1 - 0);
	float x = 0, y = 0;
	if (s1 >= s2) {
		x = f * s2 / s1;
		y = f;
	} else {
		x = f;
		y = f * s1 / s2;
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex2f(-x, y);
	glTexCoord2f(1, 1);
	glVertex2f(x, y);
	glTexCoord2f(1, 0);
	glVertex2f(x, -y);
	glTexCoord2f(0, 0);
	glVertex2f(-x, -y);
	glEnd();
}

bool ReadKey(GLFWwindow* window, int key) {
	return glfwGetKey(window, key) == GLFW_PRESS;
}

void ReadKeys(GLFWwindow* window, bool turbo, bool* buttons){
	buttons[ButtonA] = ReadKey(window, GLFW_KEY_Z) || (turbo && ReadKey(window, GLFW_KEY_A));
	buttons[ButtonB] = ReadKey(window, GLFW_KEY_X) || (turbo && ReadKey(window, GLFW_KEY_S));
	buttons[ButtonSelect] = ReadKey(window, GLFW_KEY_RIGHT_SHIFT);
	buttons[ButtonStart] = ReadKey(window, GLFW_KEY_ENTER);
	buttons[ButtonUp] = ReadKey(window, GLFW_KEY_I);
	buttons[ButtonDown] = ReadKey(window, GLFW_KEY_K);
	buttons[ButtonLeft] = ReadKey(window, GLFW_KEY_J);
	buttons[ButtonRight] = ReadKey(window, GLFW_KEY_L);
}

void UpdateControllers(GLFWwindow* window)
{
	auto turbo = (console->PPU->Frame % 6) < 3;
	ReadKeys(window, turbo, console->Controller1->buttons);
}

void Step(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT);

	UpdateControllers(window);

	console->StepSeconds(TIME_PER_FRAME);

	// render
	Image* img = console->Buffer();
	//glDrawPixels(img->width, img->height, GL_RGBA, GL_UNSIGNED_BYTE, img->bytes);

	glBindTexture(GL_TEXTURE_2D, texture);
	SetTexture(img);
	DrawBuffer(window);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFlush();

	double renderEndTime = glfwGetTime();
	double remainingTime = TIME_PER_FRAME - (renderEndTime - lastTime);

	if (remainingTime > 0) {
		Sleep(long(remainingTime * 1000));
	}

	lastTime = glfwGetTime();

}

int main(int argc, char** argv)
{
	float scale = 2;
	char* romFile = nullptr;

	// parse command line
	for (int i = 0; i < argc;)
	{
		if (strcmp("-rom", argv[i]) == 0) {
			i++;
			romFile = argv[i];
		}
		else if (strcmp("-scale", argv[i]) == 0) {
			i++;
			scale = (float)atof(argv[i]);
		}
		i++;
	}

	if (romFile == nullptr)
	{
		log("Please specify rom file with -rom");
		return 0;
	}

	console = new Console(romFile);
	log("Loaded: %s", romFile);
	console->Reset();

	channel = new BlockingQueue<float>;
	Audio audio;
	audio.channel = channel;
	console->SetAudioChannel(channel);
	console->SetAudioSampleRate(44100);
	audio.Start();

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, 0);
	GLFWwindow* window = glfwCreateWindow(int(256 * scale), int(240 * scale), "Nes Emulator", NULL, NULL);
	glfwMakeContextCurrent(window);
	glEnable(GL_TEXTURE_2D);
	lastTime = glfwGetTime();

	CreateTexture();
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		Step(window);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	log("Shutting down...");
	audio.Stop();
	glfwTerminate();

	return 0;
}
