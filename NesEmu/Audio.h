#pragma once
#include <portaudio.h>
#include "BlockingQueue.h"

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

