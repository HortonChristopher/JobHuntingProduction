#pragma once
#include <chrono>
#include <ratio>

class DeltaTime
{
public:
	void deltaTimeCalc();
	void deltaTimeNow();
	void deltaTimePrev();

private:
	std::chrono::steady_clock::time_point deltaStartBase;
	std::chrono::steady_clock::time_point deltaStartNow;
	bool firstRun = true;

public:
	std::chrono::duration<double, std::micro> deltaTimeCalculated;
};