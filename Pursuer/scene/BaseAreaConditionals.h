#pragma once

class BaseAreaConditionals
{
public:
	static bool IsScreenNotShaking(bool screenShake)
	{
		if (screenShake)
		{
			return false;
		}

		return true;
	}

	static bool IsCameraNotLockedOnAndPlayerNotDodging(bool lockOn, bool dodge)
	{
		if (!lockOn && !dodge)
		{
			return true;
		}

		return false;
	}

	static bool IsPlayerDodging(bool dodge)
	{
		if (dodge)
		{
			return true;
		}

		return false;
	}

	static bool IsInitializationFinishedAndGameStarting(bool gameStart, bool initializeFinished)
	{
		if (!gameStart && initializeFinished)
		{
			return true;
		}

		return false;
	}

	static bool ShouldFadeInBegin(float startTimer, static float startTimerLimit)
	{
		if (startTimer >= startTimerLimit)
		{
			return true;
		}

		return false;
	}

	static bool IsFadeSpriteAlphaBelowZero(float fadeSpriteAlpha, const float minAlpha)
	{
		if (fadeSpriteAlpha <= minAlpha)
		{
			return true;
		}

		return false;
	}
};