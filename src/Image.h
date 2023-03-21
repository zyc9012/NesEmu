#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdint.h>

class Image
{
public:
  Image(int width, int height);
  ~Image();
  void SetColor(int x, int y, uint32_t color);
  int width, height;
  uint32_t* bytes;
  uint32_t byte_len;
};

#endif