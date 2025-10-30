#include "audio.h"
#include "utils.h"

Audio::Audio()
{
  SDL_AudioSpec spec;
  SDL_zero(spec);
  spec.freq = 44100;
  spec.format = SDL_AUDIO_F32;
  spec.channels = 1;

  audioDev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
  if (audioDev == 0) {
    log("Failed to open audio device: %s", SDL_GetError());
    return;
  }

  // Create an audio stream and bind it to the device
  audioStream = SDL_CreateAudioStream(&spec, &spec);
  if (audioStream == nullptr) {
    log("Failed to create audio stream: %s", SDL_GetError());
    SDL_CloseAudioDevice(audioDev);
    return;
  }

  if (!SDL_BindAudioStream(audioDev, audioStream)) {
    log("Failed to bind audio stream: %s", SDL_GetError());
    SDL_DestroyAudioStream(audioStream);
    SDL_CloseAudioDevice(audioDev);
    return;
  }

  // recorder = new AudioRecorder("../record.wav");
}

Audio::~Audio()
{
  SDL_PauseAudioDevice(audioDev);
  SDL_DestroyAudioStream(audioStream);
  SDL_CloseAudioDevice(audioDev);

  if (recorder != nullptr) recorder->Stop();
}

void Audio::Start()
{
  SDL_ResumeAudioDevice(audioDev);
}

void Audio::Push(float out)
{
  buffer[bufferPtr++] = out;
  if (bufferPtr >= FRAMES_PER_BUFFER)
  {
    while (SDL_GetAudioStreamQueued(audioStream) > 4096) {
      // The queue is considered full, sleep the main thread a little for the audio to catch up
      SDL_Delay(1);
    }
    bufferPtr = 0;
    SDL_PutAudioStreamData(audioStream, buffer, FRAMES_PER_BUFFER * sizeof(float));
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