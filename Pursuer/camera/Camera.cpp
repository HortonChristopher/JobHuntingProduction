#include "Camera.h"

Camera::Camera()
{
	float aspectRatio = (float)1920 / 1080;

	eye = XMFLOAT3(0, 0, -10);
	target = XMFLOAT3(0, 0, 0);
	up = XMFLOAT3(0, 1, 0);

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		aspectRatio,
		nearDist, farDist
	);

	UpdateViewMatrix();
}

Camera::Camera(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up, float arg_near, float arg_far) :eye(eye), target(target), up(up), nearDist(arg_near), farDist(arg_far)
{
	float aspectRatio = (float)1920 / 1080;

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f),
		aspectRatio,
		nearDist, farDist
	);

	UpdateViewMatrix();
}


Camera::~Camera()
{
}

void Camera::Update()
{
	if (viewDirty) {
		// View matrix update
		UpdateViewMatrix();
		viewDirty = false;
	}
}

void Camera::UpdateViewMatrix()
{
	// Viewpoint coordinates
	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	// Target coordinates
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	// Upward coordinates
	XMVECTOR upVector = XMLoadFloat3(&up);

	// Camera Z-axis (line of sight direction)
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);

	// Asserted because 0 vector cannot be oriented
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));

	// Normalize vector
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	// X-axis (right direction) of the camera
	XMVECTOR cameraAxisX;
	// X-axis is obtained by the outer product of the upward towards Z-axis
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	// Normalize vector
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	// Y-axis (upward) of the camera
	XMVECTOR cameraAxisY;
	// Y-axis is obtained by the outer product of Z-axis towards X-axis
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	// Camera rotation matrix
	XMMATRIX matCameraRot;
	// Camera coordinate system to world coordinate system transformation matrix
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
	// Calculate the inverse matrix (inverse rotation) by transposition
	matView = XMMatrixTranspose(matCameraRot);

	// Coordinates of viewpoint multiplied by -1
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	// Vector from the camera position to the world origin (camera coordinate system)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);
	// Combine them into one vector.
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	// Set the translation component in the view matrix
	matView.r[3] = translation;

#pragma region Compute omnidirectional billboard matrix
	// Billboard matrix
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion

#pragma region Calculation of billboard matrix around Y-axis
	// Camera X-axis, Y-axis, Z-axis
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	// X axis is common
	ybillCameraAxisX = cameraAxisX;
	// Y axis is the Y axis of the world coordinate system
	ybillCameraAxisY = XMVector3Normalize(upVector);
	// Z-axis is obtained by the outer product of X-axis to Y-axis
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	// Billboard matrix around Y axis
	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion
}

void Camera::SetViewMatrix(const XMFLOAT3& eye, const XMFLOAT3& target, const XMFLOAT3& up)
{
	this->eye = eye;
	this->target = target;
	this->up = up;

	viewDirty = true;
}

void Camera::MoveVector(const XMVECTOR& moveEye, const XMVECTOR& moveTarget, const XMVECTOR& moveUp)
{
	eye.x += moveEye.m128_f32[0];
	eye.y += moveEye.m128_f32[1];
	eye.z += moveEye.m128_f32[2];

	target.x += moveTarget.m128_f32[0];
	target.y += moveTarget.m128_f32[1];
	target.z += moveTarget.m128_f32[2];

	up.x += moveUp.m128_f32[0];
	up.y += moveUp.m128_f32[1];
	up.z += moveUp.m128_f32[2];

	viewDirty = true;
}
