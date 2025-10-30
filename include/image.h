#ifndef _IMAGE_H
#define _IMAGE_H

#include <cstdint>
#include <vector>
#include <cstddef>

class Image
{
public:
  Image(int width, int height);
  ~Image() = default;
  
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
  Image(Image&&) = default;
  Image& operator=(Image&&) = default;
  
  void SetColor(int x, int y, uint32_t color);
  
  int GetWidth() const { return width; }
  int GetHeight() const { return height; }
  const uint32_t* GetBytes() const { return bytes.data(); }
  uint32_t* GetBytes() { return bytes.data(); }
  size_t GetByteLen() const { return byte_len; }

private:
  int width{0};
  int height{0};
  std::vector<uint32_t> bytes;
  size_t byte_len{0};
};

#endif