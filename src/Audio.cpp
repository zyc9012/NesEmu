#include "Audio.h"
#include "utils.h"

Audio::Audio()
{
  SDL_AudioSpec req, real;
  memset(&req, 0, sizeof(req));
  req.freq = 44100;
  req.format = AUDIO_F32;
  req.channels = 1;
  req.samples = FRAMES_PER_BUFFER;
  req.callback = nullptr;
  req.userdata = nullptr;

  audioDev = SDL_OpenAudioDevice(nullptr, 0, &req, &real, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
  if (audioDev <= 0) {
    log("Failed to open audio device: %s", SDL_GetError());
    return;
  }

  if (real.samples != req.samples) // Notify, but still use it
    log("Output audio samples: %d (requested: %d)", real.samples, req.samples);
  if (real.format != req.format || real.channels != req.channels) {
    log("Sound buffer format does not match requested format.");
    log("Output audio freq: %d (requested: %d)", real.freq, req.freq);
    log("Output audio format: %d (requested: %d)", real.format, req.format);
    log("Output audio channels: %d (requested: %d)", real.channels, req.channels);
    log("Provided output format does not match requirement, turning audio off");
    SDL_CloseAudioDevice(audioDev);
    return;
  }

  // recorder = new AudioRecorder("../record.wav");
}

Audio::~Audio()
{
  SDL_PauseAudioDevice(audioDev, 1);
  SDL_CloseAudioDevice(audioDev);

  if (recorder != nullptr) recorder->Stop();
}

void Audio::Start()
{
  SDL_PauseAudioDevice(audioDev, 0);
}

void Audio::Push(float out)
{
  buffer[bufferPtr++] = out;
  if (bufferPtr >= FRAMES_PER_BUFFER)
  {
    bufferPtr = 0;
    SDL_QueueAudio(audioDev, buffer, FRAMES_PER_BUFFER * sizeof(float));
  }
}


AudioRecorder::AudioRecorder(const char* filename)
{
  audioOut = fopen(filename, "wb");
  fwrite(&wavHeader, sizeof(wavHeader), 1, audioOut);
}

AudioRecorder::~AudioRecorder()
{
}

void AudioRecorder::WriteSamples(float* buf, int count)
{
  fwrite(buf, sizeof(float)*count, 1, audioOut);
  totalSampleCount += count;
}

void AudioRecorder::Stop()
{
  wavHeader.ckSize1 += (totalSampleCount * sizeof(float));
  wavHeader.sampleLength = totalSampleCount;
  wavHeader.ckSize4 = (totalSampleCount * sizeof(float));
  fseek(audioOut, 0, SEEK_SET);
  fwrite(&wavHeader, sizeof(wavHeader), 1, audioOut);
  fclose(audioOut);
}