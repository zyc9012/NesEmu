#include "FilterChain.h"
#define M_PI 3.14159265358979323846

float FirstOrderFilter::Step(float x) {
	auto y = B0*x + B1*prevX - A1*prevY;
	prevY = y;
	prevX = x;
	return y;
}

// sampleRate: samples per second
// cutoffFreq: oscillations per second
Filter* LowPassFilter(float sampleRate, float cutoffFreq)  {
	auto c = sampleRate / M_PI / cutoffFreq;
	auto a0i = 1 / (1 + c);
	auto f = new FirstOrderFilter();
	f->B0 = float(a0i);
	f->B1 = float(a0i);
	f->A1 = float((1 - c) * a0i);
	return f;
}

Filter* HighPassFilter(float sampleRate, float cutoffFreq) {
	auto c = sampleRate / M_PI / cutoffFreq;
	auto a0i = 1 / (1 + c);
	auto f = new FirstOrderFilter();
	f->B0 = float(c * a0i);
	f->B1 = float(-c * a0i);
	f->A1 = float((1 - c) * a0i);
	return f;
}

FilterChain::FilterChain(Filter** fc, int fc_len)
{
	this->fc = fc;
	this->fc_len = fc_len;
}

FilterChain::~FilterChain()
{
	for (int i = 0; i < fc_len; i++)
	{
		delete fc[i];
	}
	delete fc;
}

float FilterChain::Step(float x)
{
	if (fc != nullptr)
	{
		for (int i = 0; i < fc_len; i++)
		{
			x = fc[i]->Step(x);
		}
	}
	return x;
}