#pragma once

#include <DirectXMath.h>
#include <d3d12.h>

#include "Vector.h"

using namespace DirectX;

/// <summary>
/// Basic Camera Functions
/// </summary>
class Camera
{
public:
	Camera();

	Camera(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up, float arg_near = 0.1f, float arg_far = 1000.0f);

	~Camera();

	virtual void Update();

	void UpdateViewMatrix();

	inline const Vector3& GetEye() { return eye; }
	inline const Vector3& GetTarget() { return target; }
	inline const Vector3& GetUp() { return up; }

	inline const XMMATRIX& GetViewMatrix() { return matView; }
	inline const XMMATRIX& GetProjectionMatrix() { return matProjection; }

	inline const XMMATRIX& GetViewMatrixProjection() 
	{
		matViewProjection = matView * matProjection;
		return matViewProjection;
	}

	inline const XMMATRIX& GetBillboardMatrix() { return matBillboard; }
	inline const XMMATRIX& GetBillboardMatrixY() { return matBillboardY; }

	inline const float GetNear() { return nearDist; }
	inline const float GetFar() { return farDist; }
	inline const float GetPhi() { return phi; }

	inline const float GetTheta()
	{
		if (theta < 0)
		{
			return 0;
		}
		return theta;
	}

	void SetViewMatrix(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up);

	void MoveVector(const XMVECTOR& moveEye = { }, const XMVECTOR& moveTarget = { }, const XMVECTOR& moveUp = { });

	void SetEye(const XMFLOAT3& eye) { this->eye = eye; viewDirty = true; }
	void SetTarget(const XMFLOAT3& target) { this->target = target; viewDirty = true; }
	void SetUp(const XMFLOAT3& up) { this->up = up; viewDirty = true; }
protected:
	XMMATRIX matView; // View transformation matrix

	float angle = 0.0f;
	float nearDist = 0.1f;
	float farDist = 1000.0f;

	float phi;
	float theta;

	Vector3 eye;
	Vector3 target;
	Vector3 up;

	// View matrix dirty flag
	bool viewDirty = false;

	// Billboard matrix
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();

	// Billboard matrix around Y axis
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();

	// Projection matrix
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();

	// View projection matrix
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
};

