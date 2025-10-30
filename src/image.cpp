#include "image.h"

Image::Image(int width, int height)
  : width(width), height(height), byte_len(width * height * sizeof(uint32_t))
{
  bytes.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
}

void Image::SetColor(int x, int y, uint32_t color)
{
  bytes[static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x)] = color;
}
