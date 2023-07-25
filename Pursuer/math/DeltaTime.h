#pragma once
#include <chrono>
#include <ratio>

class DeltaTime
{
public:
	void deltaTimeCalc();
	void deltaTimeNow();
	void deltaTimePrev();

	float DeltaTimeDividedByMiliseconds()
	{
		return deltaTimeCalculated.count() / 1000000.0f;
	}

private:
	std::chrono::steady_clock::time_point deltaStartBase;
	std::chrono::steady_clock::time_point deltaStartNow;

public:
	std::chrono::duration<float, std::micro> deltaTimeCalculated;
};