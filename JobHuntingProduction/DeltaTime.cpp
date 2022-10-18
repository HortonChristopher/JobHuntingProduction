#include "DeltaTime.h"

void DeltaTime::deltaTimeCalc()
{
	deltaTimeCalculated = deltaStartNow - deltaStartBase;
}

void DeltaTime::deltaTimeNow()
{
	deltaStartNow = std::chrono::high_resolution_clock::now();
}

void DeltaTime::deltaTimePrev()
{
	deltaStartBase = std::chrono::high_resolution_clock::now();
}