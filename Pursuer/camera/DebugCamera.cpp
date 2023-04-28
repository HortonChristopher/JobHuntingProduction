#include "DebugCamera.h"
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;
extern XMFLOAT3 playerMovement = { 0.0f, 0.0f, 0.0f };

extern DeltaTime* deltaTime;

extern float degreeTransfer;
extern bool lockOnActive;

DebugCamera::DebugCamera(int window_width, int window_height, Input* input)
	: Camera(window_width, window_height)
{
	assert(input);

	this->input = input;
	// Adjust to scale relative to screen size
	scaleX = 1.0f / (float)window_width;
	scaleY = 1.0f / (float)window_height;
}

void DebugCamera::Update()
{
	bool dirty = false;
	float angleX = 0.0f;
	float angleY = 0.0f;

	// Get mouse input
	Input::MouseMove mouseMove = input->GetMouseMove();
	//Input::float2 stickMove = input->GetRStickDirection();

	if (title)
	{
		angleY -= XMConvertToRadians(10.0f) * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		dirty = true;
	}
	// Rotate the camera if the right mouse button is pressed
	else if (cutscene)
	{
		angleX += XMConvertToRadians(20.0f) * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		dirty = true;
	}
	else if (lockOn)
	{
		// Calculate the view matrix
  		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

		// Extract the cameraPos position from the view matrix
		XMFLOAT3 cameraPos;
		XMStoreFloat3(&cameraPos, XMVectorNegate(XMVector3TransformCoord(XMLoadFloat3(&target), XMMatrixInverse(nullptr, view))));

		// Calculate the direction vector from the camera's position to the boss position
		XMFLOAT3 directionVector = XMFLOAT3(lockOnEnemyPos.x - playerPos.x, lockOnEnemyPos.y - playerPos.y, lockOnEnemyPos.z - playerPos.z);

		// Normalize the direction vector
		XMVECTOR directionVectorNormalized = XMVector3Normalize(XMLoadFloat3(&directionVector));

		// Calculate the camera's new position based on the direction vector and camera speed
		XMFLOAT3 newPosition = XMFLOAT3(playerPos.x - XMVectorGetX(directionVectorNormalized) * 48.0f, playerPos.y - XMVectorGetY(directionVectorNormalized) * 48.0f + 30.0f, playerPos.z - XMVectorGetZ(directionVectorNormalized) * 48.0f);

		// Calculate the target position based on the player and boss positions
		XMFLOAT3 newTarget = XMFLOAT3((playerPos.x + lockOnEnemyPos.x) * 0.5f, (playerPos.y + lockOnEnemyPos.y) * 0.5f, (playerPos.z + lockOnEnemyPos.z) * 0.5f);

		// Convert cameraPos to cameraEye
		XMVECTOR posV = XMLoadFloat3(&newPosition);
		XMVECTOR targetV = XMLoadFloat3(&target);
		XMVECTOR upV = XMLoadFloat3(&up);

		// Calculate the view matrix
		view = XMMatrixLookAtLH(posV, targetV, upV);

		// Extract the cameraEye position from the view matrix
		XMVECTOR cameraEyeVec = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		cameraEyeVec = XMVector4Transform(cameraEyeVec, XMMatrixInverse(nullptr, view));

		// Convert cameraEyeVec back to a XMFLOAT3
		XMFLOAT3 cameraEye;
		XMStoreFloat3(&cameraEye, cameraEyeVec);

		// Update the camera's position, target, and up vectors
		eye = cameraEye;
		target = lockOnEnemyPos;
		up = up;

		dirty = true;
		viewDirty = true;
	}
	else if (wCutscene)
	{
		switch (wCutscenePart)
		{
		case 0:
			angleY -= XMConvertToRadians(20.0f) * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			dirty = true;
			break;
		case 1:
			angleY += XMConvertToRadians(20.0f) * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			dirty = true;
			break;
		case 2:
			break;
		case 3:
			angleX += XMConvertToRadians(20.0f) * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			dirty = true;
			break;
		}
	}
	else if (cutsceneActive)
	{

	}
	else if (input->PushMouseRight() || input->PushRStickLeft() || input->PushRStickRight() || input->PushRStickDown() || input->PushRStickUp())
	{
		// Camera control with the controller's right stick
		if (input->PushRStickLeft() || input->PushRStickUp() || input->PushRStickRight() || input->PushRStickDown())
		{
			auto vec = input->GetRStickDirection();

			phi += XM_PI / 180.0f * -vec.x * ((float)SettingParameters::GetPadSensitivity() / 3.0f) * SettingParameters::GetReverseX();
			theta += XM_PI / 180.0f * -vec.y * ((float)SettingParameters::GetPadSensitivity() / 3.0f) * SettingParameters::GetReverseY();
			if (theta > 40 * XM_PI / 180.0f)
				theta = 40 * XM_PI / 180.0f;
			else if (theta < -40 * XM_PI / 180.0f)
				theta = -40 * XM_PI / 180.0f;

			if (phi > 360 * XM_PI / 180.0f)
				phi -= 360 * XM_PI / 180.0f;
			else if (phi < 0)
				phi += 360 * XM_PI / 180.0f;

			if (theta < 0)
			{
				distance = 48 * (1 + theta * 1.1f);
			}
			dirty = true;
		}

		// Rotate the camera if the right mouse button is pressed
		if (input->PushMouseRight())
		{
			phi += XM_PI / 180.0f * mouseMove.lX / 7;
			theta += XM_PI / 180.0f * mouseMove.lY / 7;
			if (theta > 40 * XM_PI / 180.0f)
				theta = 40 * XM_PI / 180.0f;
			else if (theta < -40 * XM_PI / 180.0f)
				theta = -40 * XM_PI / 180.0f;

			if (phi > 360 * XM_PI / 180.0f)
				phi -= 360 * XM_PI / 180.0f;
			else if (phi < 0)
				phi += 360 * XM_PI / 180.0f;

			if (theta < 0)
			{
				distance = 48 * (1 + theta * 1.1f);
			}
			dirty = true;
		}

		//  Change the distance with wheel input
		if (mouseMove.lZ != 0) {
			distance -= mouseMove.lZ / 100.0f;
			distance = max(distance, 1.0f);
			dirty = true;
		}

		dirty = true;
	}

	//prevRotation = objectRotation.y;

	if (dirty || viewDirty) {
		// 追加回転分の回転行列を生成
		if (title || cutscene || wCutscene || cutsceneActive)
		{
			XMMATRIX matRotNew = XMMatrixIdentity();
			if (cutscene)
			{
				matRotNew *= XMMatrixRotationX(-angleX);
			}
			else
			{
				matRotNew *= XMMatrixRotationY(-angleY);
			}
			// 累積の回転行列を合成
			// ※回転行列を累積していくと、誤差でスケーリングがかかる危険がある為
			// クォータニオンを使用する方が望ましい
			matRot = matRotNew * matRot;

			// 注視点から視点へのベクトルと、上方向ベクトル
			XMVECTOR vTargetEye = { 0.0f, 0.0f, -distance, 1.0f };
			XMVECTOR vUp = { 0.0f, 1.0f, 0.0f, 0.0f };

			// ベクトルを回転
			vTargetEye = XMVector3Transform(vTargetEye, matRot);
			vUp = XMVector3Transform(vUp, matRot);

			// 注視点からずらした位置に視点座標を決定
			const XMFLOAT3& target = GetTarget();
			SetEye({ target.x + vTargetEye.m128_f32[0], target.y + vTargetEye.m128_f32[1] + 20.0f, target.z + vTargetEye.m128_f32[2] });
			SetUp({ vUp.m128_f32[0], vUp.m128_f32[1], vUp.m128_f32[2] });
		}
		else if (lockOn)
		{
			viewDirty = true;
		}
		else
		{
			float nowTheta = theta;
			if (theta < 0)
				nowTheta = 0;
			eye = Vector3(cos(phi) * cos(nowTheta), sin(nowTheta), sin(phi) * cos(nowTheta)) * distance + target;
			viewDirty = true;
		}
	}

	Camera::Update();
}
