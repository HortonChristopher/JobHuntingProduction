#include "DebugCamera.h"
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

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
	float angleX = 0;
	float angleY = 0;

	// Get mouse input
	Input::MouseMove mouseMove = input->GetMouseMove();
	Input::float2 stickMove = input->GetRStickDirection();

	// Rotate the camera if the right mouse button is pressed
	if (input->PushMouseRight() || input->PushRStickLeft() || input->PushRStickRight())
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

	/*if (input->PushKey(DIK_W) || input->PushLStickUp())
	{
		float dz = 1.0f;

		XMVECTOR move = { 0, 0, +dz };
		move = XMVector3Transform(move, matRot);

		MoveVector(move);
		dirty = true;
	}

	if (input->PushKey(DIK_S) || input->PushLStickDown())
	{
		float dz = 1.0f;

		XMVECTOR move = { 0, 0, -dz };
		move = XMVector3Transform(move, matRot);

		MoveVector(move);
		dirty = true;
	}*/

	//if (input->PushKey(DIK_A))
	//{
	//	float dy = 2.0f;

	//	angleY = dy * XM_PI;
	//	dirty = true;
	//}

	//if (input->PushKey(DIK_D))
	//{
	//	float dy = 2.0f;

	//	angleY = -dy * XM_PI;
	//	dirty = true;
	//}

	// Change the distance with wheel input
	if (mouseMove.lZ != 0) {
		distance -= mouseMove.lZ / 100.0f;
		distance = max(distance, 1.0f);
		dirty = true;
	}

	if (dirty || viewDirty) {
		// 追加回転分の回転行列を生成
		XMMATRIX matRotNew = XMMatrixIdentity();
		matRotNew *= XMMatrixRotationX(-angleX);
		matRotNew *= XMMatrixRotationY(-angleY);
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
