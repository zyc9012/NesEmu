#ifndef _AUDIO_H
#define _AUDIO_H

#include <cstdio>
#include <cstdint>
#include <array>
#include <memory>
#include <string>
#include <SDL3/SDL.h>

inline constexpr int FRAMES_PER_BUFFER = 256;

class AudioRecorder;
class Audio
{
public:
  Audio();
  ~Audio();
  
  Audio(const Audio&) = delete;
  Audio& operator=(const Audio&) = delete;
  Audio(Audio&&) = delete;
  Audio& operator=(Audio&&) = delete;

  void Start();
  void Push(float out);
  void Close();
private:
  std::array<float, FRAMES_PER_BUFFER> buffer{};
  int bufferPtr{0};
  
  SDL_AudioDeviceID audioDev{0};
  SDL_AudioStream* audioStream{nullptr};

  std::unique_ptr<AudioRecorder> recorder;
};



#pragma pack(push, 1)
struct WavHeader {
  std::array<char, 4> ckId1{'R', 'I', 'F', 'F'};
  uint32_t ckSize1{4 + 26 + 12 + 8 + 0};
  std::array<char, 4> waveId{'W', 'A', 'V', 'E'};
  std::array<char, 4> chId2{'f', 'm', 't', ' '};
  uint32_t ckSize2{18};
  uint16_t formattag{0x03};
  uint16_t channels{1};
  uint32_t samplesPerSec{44100};
  uint32_t avgBytesPerSec{44100 * sizeof(float) * 1};
  uint16_t blockAlign{static_cast<uint16_t>(sizeof(float) * 1)};
  uint16_t bitsPerSample{static_cast<uint16_t>(8 * sizeof(float))};
  uint16_t cbSize{0};
  std::array<char, 4> ckId3{'f', 'a', 'c', 't'};
  uint32_t ckSize3{4};
  uint32_t sampleLength{0};
  std::array<char, 4> ckId4{'d', 'a', 't', 'a'};
  uint32_t ckSize4{0};
};
#pragma pack(pop)

class AudioRecorder
{
public:
  explicit AudioRecorder(const std::string& filename);
  ~AudioRecorder();
  
  AudioRecorder(const AudioRecorder&) = delete;
  AudioRecorder& operator=(const AudioRecorder&) = delete;
  AudioRecorder(AudioRecorder&&) = delete;
  AudioRecorder& operator=(AudioRecorder&&) = delete;
  
  void WriteSamples(const float* buf, int count);
  void Stop();
private:
  FILE* audioOut{nullptr};
  WavHeader wavHeader;
  uint32_t totalSampleCount{0};
};

#endif