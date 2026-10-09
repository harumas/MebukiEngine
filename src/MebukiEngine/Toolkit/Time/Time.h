#pragma once
#include <windows.h>

class Time
{
public:
	Time();
	~Time();

	void Tick();
	float GetDeltaTime() const;

private:
	LARGE_INTEGER frequency;
	LARGE_INTEGER lastTime;
	double deltaTime = 0;
};
