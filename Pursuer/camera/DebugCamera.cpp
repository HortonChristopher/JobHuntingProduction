#include "DebugCamera.h"
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;

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
	Input::float2 stickMove = input->GetRStickDirection();

	if (title)
	{
		angleY -= XMConvertToRadians(10.0f) * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		dirty = true;
	}
	//else if (input->PushKey(DIK_SPACE) || input->PushControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	//{
	//	//angleY -= (XMConvertToRadians(objectRotation.y) - XMConvertToRadians(prevRotation)) * 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	//	if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	//	{
	//		if (objectRotation.y > 0.0f)
	//		{
	//			angleY -= XMConvertToRadians(objectRotation.y + 0.0f);
	//		}
	//		else if (objectRotation.y < 0.0f)
	//		{
	//			angleY += XMConvertToRadians(objectRotation.y + 0.0f);
	//		}

	//		dirty = true;

	//		prevRotation = objectRotation.y;
	//	}
	//	else
	//	{
	//		angleY -= XMConvertToRadians(objectRotation.y - prevRotation);

	//		dirty = true;
	//	}
	//}
	// Rotate the camera if the right mouse button is pressed
	else if (cutscene)
	{
		angleX += XMConvertToRadians(20.0f) * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		dirty = true;
	}
	else if (cutsceneActive)
	{

	}
	else if (input->PushMouseRight() || input->PushRStickLeft() || input->PushRStickRight())
	{
		float dy;
		//float dx;

		if (input->PushMouseRight())
		{
			dy = mouseMove.lX * scaleY;
			//dx = mouseMove.lY * scaleX;
		}
		if (input->PushRStickLeft() || input->PushRStickRight())
		{
			dy = stickMove.x * -scaleY * 10.0f;
		}
		if (input->PushRStickUp() || input->PushRStickDown())
		{
			//dx = stickMove.y * -scaleX * 10.0f;
		}

		//angleX = dx * XM_PI;
		angleY = dy * XM_PI;

		dirty = true;
	}

	prevRotation = objectRotation.y;

	// Translate the camera if the middle mouse button is pressed
	/*if (input->PushMouseMiddle())
	{
		float dx = mouseMove.lX / 100.0f;
		float dy = mouseMove.lY / 100.0f;

		XMVECTOR move = {-dx, +dy, 0, 0};
		move = XMVector3Transform(move, matRot);

		MoveVector(move);
		dirty = true;
	}*/

	// Change the distance with wheel input
	/*if (mouseMove.lZ != 0 && !title) {
		distance -= mouseMove.lZ / 100.0f;
		distance = max(distance, 1.0f);
		dirty = true;
	}*/

	if (dirty || viewDirty) {
		// 追加回転分の回転行列を生成
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

	Camera::Update();
}
