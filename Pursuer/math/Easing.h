#pragma once
#include <cmath>

class Easing
{
private:

public: // Functions
	// Linear Interpolation (Lerp)
	static float Lerp(const float& start, const float& end, const float& time);

	// Sin(sine)-based easing
	static float EaseInSin(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseOutSin(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutSin(const float& start, const float& end, const float& maximumTime, const float& currentTime);

	// Quadratic-based easing
	static float EaseInQuad(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseOutQuad(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutQuad(const float& start, const float& end, const float& maximumTime, const float& currentTime);

	// Cubic-based easing
	static float EaseInCubic(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseOutCubic(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutCubic(const float& start, const float& end, const float& maximumTime, const float& currentTime);

	// Quartic-based easing
	static float EaseInQuartic(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseOutQuartic(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutQuartic(const float& start, const float& end, const float& maximumTime, const float& currentTime);

	// Quintic-based easing
	static float EaseInQuintic(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseOutQuintic(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutQuintic(const float& start, const float& end, const float& maximumTime, const float& currentTime);

	// Exponential-based easing
	static float EaseInExpon(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseOutExpon(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutExpon(const float& start, const float& end, const float& maximumTime, const float& currentTime);

	// Circle-based easing
	static float EaseInCirc(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseOutCirc(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutCirc(const float& start, const float& end, const float& maximumTime, const float& currentTime);

	// Back-based easing
	static float EaseInBack(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseOutBack(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutBack(const float& start, const float& end, const float& maximumTime, const float& currentTime);

	// Elastic-based easing
	static float EaseInElastic(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseOutElastic(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutElastic(const float& start, const float& end, const float& maximumTime, const float& currentTime);

	// Bounce-based easing
	static float EaseOutBounce(const float& start, const float& end, const float& maximumTime, const float& currentTime); // EaseIn and EaseInOut both use EaseOutBounce, so it's written first
	static float EaseInBounce(const float& start, const float& end, const float& maximumTime, const float& currentTime);
	static float EaseInOutBounce(const float& start, const float& end, const float& maximumTime, const float& currentTime);
};