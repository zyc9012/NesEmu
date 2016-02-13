#pragma once
#include <portaudio.h>
#define FRAMES_PER_BUFFER 1024

class Audio
{
public:
	Audio();
	~Audio();

	void Start();
	void Stop();
	void Push(float out);

private:
	PaStream* stream;
	float buffer[FRAMES_PER_BUFFER];
	int bufferPtr = 0;
};

