#pragma once
#include "BlockingQueue.h"
#include <portaudio.h>

class Audio
{
public:
	Audio();
	~Audio();

	void Start();
	void Stop();

	BlockingQueue<float>* channel;

private:
	PaStream* stream;
};

