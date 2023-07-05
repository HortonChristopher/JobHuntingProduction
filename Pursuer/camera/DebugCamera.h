#pragma once
#include "Camera.h"
#include "Input.h"
#include "DeltaTime.h"
#include "SettingParameters.h"
#include "Vector.h"

#include <DirectXMath.h>

/// <summary>
/// デバッグ用カメラ
/// </summary>
class DebugCamera :
	public Camera
{
	using XMMATRIX = DirectX::XMMATRIX;
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	/// <param name="input">入力</param>
	DebugCamera(int window_width, int window_height, Input* input);

	// 更新
	void Update() override;

	void SetDistance(float distance) {
		this->distance = distance; viewDirty = true;
	}

	void SetPhi(float phi) {
		this->phi = phi;
	}

	void SetTheta(float theta) {
		this->theta = theta;
	}

	void AddPhi(const float angle)
	{
		phi += angle;
		if (phi > 360 * 3.141592654f / 180.0f)
			phi -= 360 * 3.141592654f / 180.0f;
		else if (phi < 0)
			phi += 360 * 3.141592654f / 180.0f;
		viewDirty = true;
	}

	const float GetPhi()
	{
		return phi;
	}

	const float GetTheta()
	{
		return theta;
	}

	bool IsCameraLockedOn()
	{
		return lockOn;
	}

private:
	// 入力クラスのポインタ
	Input* input;
	// カメラ注視点までの距離
	float distance = 20;
	// スケーリング
	float scaleX = 1.0f;
	float scaleY = 1.0f;

	float prevRotation = 0.0f;

	float rotation = 0.0f;
	float phi = 270.0f * 3.141592654f / 180.0f;
	float theta = 20.0f * 3.141592654f / 180.0f;

	//LONG prevMouseMove = 0.0;
	// 回転行列
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