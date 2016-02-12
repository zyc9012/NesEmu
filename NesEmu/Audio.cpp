#include "Audio.h"

int AudioCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	/* Cast data passed through stream to our structure. */
	BlockingQueue<float> *channel = *(BlockingQueue<float>**)userData;
	float *out = (float*)outputBuffer;
	unsigned int i;

	for (i = 0; i<framesPerBuffer; i++)
	{
		*out++ = channel->pop();
	}

	if (!channel->empty())
	{
		channel->clear();
	}
	return 0;
}

Audio::Audio()
{
	auto err = Pa_Initialize();
	if (err != paNoError) throw err;

	/* Open an audio I/O stream. */
	err = Pa_OpenDefaultStream(&stream,
		0,          /* no input channels */
		1,          /* mono output */
		paFloat32,  /* 32 bit floating point output */
		44100,
		paFramesPerBufferUnspecified,
		/* frames per buffer, i.e. the number
		of sample frames that PortAudio will
		request from the callback. Many apps
		may want to use
		paFramesPerBufferUnspecified, which
		tells PortAudio to pick the best,
		possibly changing, buffer size.*/
		AudioCallback, /* this is your callback function */
		&channel); /*This is a pointer that will be passed to
			your callback*/

}

Audio::~Audio()
{
	Stop();
	Pa_CloseStream(stream);
	Pa_Terminate();
}

void Audio::Start()
{
	Pa_StartStream(stream);
}

void Audio::Stop()
{
	Pa_StopStream(stream);
}