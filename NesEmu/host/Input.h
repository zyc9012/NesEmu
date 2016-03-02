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
	bool hasJoy1;
	bool hasJoy2;
	int counter = 0;
};

