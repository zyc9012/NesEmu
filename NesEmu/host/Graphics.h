#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Host;

class Graphics
{
public:
	Graphics(Host* host);
	~Graphics();

	void DrawFrame();

private:
	void CreateTexture();
	void DrawBuffer();

	Host* host;
	GLuint texture;
	double lastTime;
};

