#pragma once

class Filter
{
public:
	virtual float Step(float x) = 0;
	virtual ~Filter() {}
};

class FirstOrderFilter : public Filter
{
public:
	FirstOrderFilter() {}
	~FirstOrderFilter() {}

	virtual float Step(float x) override;

	float B0;
	float B1;
	float A1;
	float prevX;
	float prevY;
};

Filter* LowPassFilter(float sampleRate, float cutoffFreq);
Filter* HighPassFilter(float sampleRate, float cutoffFreq);

class FilterChain
{
public:
	FilterChain(Filter** fc, int fc_len);
	~FilterChain();

	Filter** fc;
	int fc_len;
	float Step(float x);
};
