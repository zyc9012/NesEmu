#include "Graphics.h"
#include "Host.h"
#include "../Console.h"
#include "../Image.h"

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

Graphics::Graphics(Host* host)
{
	this->host = host;
	glEnable(GL_TEXTURE_2D);
	lastTime = glfwGetTime();
	CreateTexture();
}


Graphics::~Graphics()
{
}

void Graphics::CreateTexture()
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, host->config->FilterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, host->config->FilterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Graphics::DrawBuffer()
{
	int w, h;
	glfwGetFramebufferSize(host->window, &w, &h);
	float s1 = float(w) / 256;
	float s2 = float(h) / 240;
	float f = float(1 - 0);
	float x = 0, y = 0;
	if (s1 >= s2) {
		x = f * s2 / s1;
		y = f;
	}
	else {
		x = f;
		y = f * s1 / s2;
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex2f(-x, -y);
	glTexCoord2f(1, 1);
	glVertex2f(x, -y);
	glTexCoord2f(1, 0);
	glVertex2f(x, y);
	glTexCoord2f(0, 0);
	glVertex2f(-x, y);
	glEnd();
}

void Graphics::DrawFrame()
{
	Image* img = host->console->Buffer();
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, img->bytes);
	DrawBuffer();
	glBindTexture(GL_TEXTURE_2D, 0);
	glFlush();

	double renderEndTime = glfwGetTime();
	double remainingTime = host->config->TimePerFrame - (renderEndTime - lastTime);

	if (remainingTime > 0) {
		long t = long(remainingTime * 1000);
		SLEEP(t);
	}

	lastTime = glfwGetTime();
}