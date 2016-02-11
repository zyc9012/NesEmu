#include "Image.h"
#include <stdio.h>


Image::Image(int width, int height)
{
	bytes = new uint32_t[width*height];
	this->width = width;
	this->height = height;
}


Image::~Image()
{
	delete bytes;
}

void Image::SetColor(int x, int y, uint32_t color)
{
	bytes[(height-y-1)*width + x] = color;
}
