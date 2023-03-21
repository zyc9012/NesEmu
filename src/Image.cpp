#include "Image.h"
#include <stdio.h>


Image::Image(int width, int height)
{
  bytes = new uint32_t[width*height];
  this->width = width;
  this->height = height;
  this->byte_len = width * height * sizeof(uint32_t);
}


Image::~Image()
{
  delete bytes;
}

void Image::SetColor(int x, int y, uint32_t color)
{
  bytes[y*width + x] = color;
}
