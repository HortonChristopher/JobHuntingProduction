#pragma once

#include "Camera.h"
#include "Input.h"
#include "DeltaTime.h"
#include "SettingParameters.h"
#include "Vector.h"
#include "GameWindow.h"

#include <DirectXMath.h>
#include <cstdio>
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

class GameWindow;

/// <summary>
/// Camera for debugging
/// </summary>
class DebugCamera : public Camera
{
private:
	using XMMATRIX = DirectX::XMMATRIX;

public:
	DebugCamera();

	// Update
	void Update();

	void SetDistance(float distance) 
	{
		this->distance = distance; viewDirty = true;
	}

	void SetPhi(float phi) 
	{
		this->phi = phi;
	}

	void SetTheta(float theta) 
	{
		this->theta = theta;
	}

	void AddPhi(const float angle)
	{
		phi += angle;
		if (phi > 360 * 3.141592654f / 180.0f)
		{
			phi -= 360 * 3.141592654f / 180.0f;
		}
		else if (phi < 0)
		{
			phi += 360 * 3.141592654f / 180.0f;
		}
		viewDirty = true;
	}

	const float GetPhi()
	{
		return phi;
	}

private:
	// Pointer to input class
	Input* input;

	// Distance to camera gazing point
	float distance = 20;

	// Scaling
	float scaleX = 1.0f;
	float scaleY = 1.0f;

	float prevRotation = 0.0f;

	float rotation = 0.0f;
	float phi = 270.0f * 3.141592654f / 180.0f;
	float theta = 20.0f * 3.141592654f / 180.0f;

	// Rotation matrix
	XMMATRIX matRot = DirectX::XMMatrixIdentity();
public:
	bool title = false;
	bool cutscene = false;
	bool wCutscene = false;
	int wCutscenePart = 0;
	bool cutsceneActive = false;
	XMFLOAT3 lockOnEnemyPos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 playerPos = { 0.0f, 0.0f, 0.0f };
	float playerRadius = 0.0f;
	bool lockOn = false;
	bool resetting = false;
	float resetPhi = 0.0f;
};