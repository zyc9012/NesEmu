#ifndef _AUDIO_H
#define _AUDIO_H

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#define FRAMES_PER_BUFFER 256

class AudioRecorder;
class Audio
{
public:
  Audio();
  ~Audio();

  void Start();
  void Push(float out);

private:
  float buffer[FRAMES_PER_BUFFER];
  int bufferPtr = 0;
  
  SDL_AudioDeviceID audioDev;

  AudioRecorder* recorder = nullptr;
};



#pragma pack(push, 1)
typedef struct WavHeader {
  char ckId1[4] = { 'R', 'I', 'F', 'F' };
  uint32_t ckSize1 = 4 + 26 + 12 + 8 + 0;
  char waveId[4] = { 'W', 'A', 'V', 'E' };
  char chId2[4] = { 'f', 'm', 't', ' ' };
  uint32_t ckSize2 = 18;
  uint16_t formattag = 0x03;
  uint16_t channels = 1;
  uint32_t samplesPerSec = 44100;
  uint32_t avgBytesPerSec = 44100 * sizeof(float) * 1;
  uint16_t blockAlign = sizeof(float) * 1;
  uint16_t bitsPerSample = 8 * sizeof(float);
  uint16_t cbSize = 0;
  char ckId3[4] = { 'f', 'a', 'c', 't' };
  uint32_t ckSize3 = 4;
  uint32_t sampleLength = 0;
  char ckId4[4] = { 'd', 'a', 't', 'a' };
  uint32_t ckSize4 = 0;
} WavHeader;
#pragma pack(pop)

class AudioRecorder
{
public:
  AudioRecorder(const char* filename);
  ~AudioRecorder();
  void WriteSamples(float* buf, int count);
  void Stop();
private:
  FILE* audioOut;
  WavHeader wavHeader;
  uint32_t totalSampleCount = 0;
};

#endif