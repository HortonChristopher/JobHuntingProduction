#include "Easing.h"

#define M_PI 3.14159265358979323846f // PI

float Easing::Lerp(const float& start, const float& end, const float& time)
{
	return start * (1.0f - time) + end * time;
}

float Easing::EaseInSin(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, 1 - cosf((M_PI * t) / 2));
}

float Easing::EaseOutSin(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, sinf((M_PI * t) / 2));
}

float Easing::EaseInOutSin(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, -(cosf(M_PI * t) - 1) / 2);
}

float Easing::EaseInQuad(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t * t);
}

float Easing::EaseOutQuad(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, 1 - (1 - t) * (1 - t));
}

float Easing::EaseInOutQuad(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t < 0.5 ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2);
}

float Easing::EaseInCubic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t * t * t);
}

float Easing::EaseOutCubic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, 1 - powf(1 - t, 3));
}

float Easing::EaseInOutCubic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t < 0.5 ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2);
}

float Easing::EaseInQuartic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t * t * t * t);
}

float Easing::EaseOutQuartic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, 1 - powf(1 - t, 4));
}

float Easing::EaseInOutQuartic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t < 0.5 ? 8 * t * t * t * t : 1 - powf(-2 * t + 2, 4) / 2);
}

float Easing::EaseInQuintic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t * t * t * t * t);
}

float Easing::EaseOutQuintic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, 1 - powf(1 - t, 5));
}

float Easing::EaseInOutQuintic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t < 0.5 ? 16 * t * t * t * t * t : 1 - powf(-2 * t + 2, 5) / 2);
}

float Easing::EaseInExpon(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t == 0 ? 0 : powf(2, 10 * t - 10));
}

float Easing::EaseOutExpon(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t == 1 ? 1 : 1 - powf(2, -10 * t));
}

float Easing::EaseInOutExpon(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t == 0 ? 0 : t == 1 ? 1 : t < 0.5 ? powf(2, 20 * t - 10) / 2 : (2 - powf(2, -20 * t + 10)) / 2);
}

float Easing::EaseInCirc(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, 1 - sqrtf(1 - powf(t, 2)));
}

float Easing::EaseOutCirc(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, sqrtf(1 - powf(t - 1, 2)));
}

float Easing::EaseInOutCirc(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	return Lerp(start, end, t < 0.5 ? (1 - sqrtf(1 - powf(2 * t, 2))) / 2 : (sqrtf(1 - powf(-2 * t + 2, 2)) + 1) / 2);
}

float Easing::EaseInBack(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	const float c1 = 1.70158f;
	const float c2 = c1 + 1;

	return Lerp(start, end, c2 * t * t * t - c1 * t * t);
}

float Easing::EaseOutBack(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	const float c1 = 1.70158f;
	const float c2 = c1 + 1;

	return Lerp(start, end, 1 + c2 * powf(t - 1, 3) + c1 * powf(t - 1, 2));
}

float Easing::EaseInOutBack(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return Lerp(start, end, t < 0.5 ? (powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2 : (powf(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2);
}

float Easing::EaseInElastic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	const float c4 = (2 * M_PI) / 3;

	return Lerp(start, end, t == 0 ? 0 : t == 1 ? 1 : -powf(2, 10 * t - 10) * sinf((t * 10 - 10.75f) * c4));
}

float Easing::EaseOutElastic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	const float c4 = (2 * M_PI) / 3;

	return Lerp(start, end, t == 0 ? 0 : t == 1 ? 1 : powf(2, -10 * t) * sinf((t * 10 - 0.75f) * c4) + 1);
}

float Easing::EaseInOutElastic(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	const float c5 = (2 * M_PI) / 4.5f;

	return Lerp(start, end, t == 0 ? 0 : t == 1 ? 1 : t < 0.5f ? -(powf(2, 20 * t - 10) * sinf((20 * t - 11.125f) * c5)) / 2 : (powf(2, -20 * t + 10) * sinf((20 * t - 11.125f) * c5)) / 2 + 1);
}

float Easing::EaseOutBounce(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (t < 1 / d1)
	{
		return Lerp(start, end, n1 * t * t);
	}
	else if (t < 2 / d1)
	{
		return Lerp(start, end, n1 * (t -= 1.5f / d1) * t + 0.75f);
	}
	else if (t < 2.5f / d1)
	{
		return Lerp(start, end, n1 * (t -= 2.25f / d1) * t + 0.9375f);
	}
	else
	{
		return Lerp(start, end, n1 * (t -= 2.625f / d1) * t + 0.984375f);
	}
}

float Easing::EaseInBounce(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;

	return Lerp(start, end, 1 - EaseOutBounce(start, end, maximumTime, 1 - t));
}

float Easing::EaseInOutBounce(const float& start, const float& end, const float& maximumTime, const float& currentTime)
{
	float t = currentTime;
	t /= maximumTime;

	return Lerp(start, end, t < 0.5f ? (1 - EaseOutBounce(start, end, maximumTime, 1 - 2 * t)) / 2 : (1 + EaseOutBounce(start, end, maximumTime, 2 * t - 1)) / 2);
}