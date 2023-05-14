#pragma once

#include "Camera.h"

class LightCamera : public Camera
{
public:
	LightCamera();

	void SetLightDir(const Vector3& dir);

	void SetDistance(const float dis)
	{
		// Return without doing anything if distance is 0
		if (dis == 0)
			return;
		distance = dis;
	};
private:
	float distance;
	float phi;
	float theta;
};