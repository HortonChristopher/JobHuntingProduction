#include "DeltaTime.h"

void DeltaTime::deltaTimeCalc()
{
	if (!firstRun)
	{
		deltaTimeCalculated = deltaStartNow - deltaStartBase;
	}
	else
	{
		firstRun = false;
	}
}

void DeltaTime::deltaTimeNow()
{
	deltaStartNow = std::chrono::high_resolution_clock::now();
}

void DeltaTime::deltaTimePrev()
{
	deltaStartBase = std::chrono::high_resolution_clock::now();
}