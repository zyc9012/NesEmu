#pragma once

class Host;

class Input
{
public:
	Input(Host* host);
	~Input();
	void UpdateControllers();

private:
	bool ReadKey(int);
	const float* ReadJoyStickAxes();
	const unsigned char* ReadJoyStickButtons();

	Host* host;
};

