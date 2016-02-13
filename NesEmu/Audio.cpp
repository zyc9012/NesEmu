#include "Audio.h"
#include "utils.h"

Audio::Audio()
{
	auto err = Pa_Initialize();
	if (err != paNoError) throw err;

	PaStreamParameters outputParameters;
	outputParameters.device = Pa_GetDefaultOutputDevice();

	outputParameters.channelCount = 1;         /* mono output */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency; //0.050
	outputParameters.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(
        &stream,
        NULL,               /* no input */
        &outputParameters,
		44100,
		FRAMES_PER_BUFFER,
        paClipOff,          /* we won't output out of range samples so don't bother clipping them */
        NULL,               /* no callback, use blocking API */
        NULL);              /* no callback, so no callback userData */
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

void Audio::Push(float out)
{
	buffer[bufferPtr++] = out;
	if (bufferPtr >= FRAMES_PER_BUFFER)
	{
		Pa_WriteStream(stream, buffer, FRAMES_PER_BUFFER);
		bufferPtr = 0;
	}
}

void Audio::Stop()
{
	Pa_StopStream(stream);
}