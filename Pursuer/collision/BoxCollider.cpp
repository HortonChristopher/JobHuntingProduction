#include "BoxCollider.h"

void BoxCollider::Update()
{
	// Obtains the posture matrix of the attached object
	const XMMATRIX& matWorld = object->GetMatWorld();
	// Obtaining the center of an object
	objCenter = matWorld.r[3];
	// Degree of object rotation
	auto matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));

	// Degree of collider rotation
	auto colMatRot = XMMatrixIdentity();
	colMatRot *= XMMatrixRotationZ(XMConvertToRadians(colliderRot.z));
	colMatRot *= XMMatrixRotationX(XMConvertToRadians(colliderRot.x));
	colMatRot *= XMMatrixRotationY(XMConvertToRadians(colliderRot.y));

	Box::scale = scale;
	// Calculation of the center position of the collider
	CalculateCenter(matRot);
	// Calculation of MAX and MIN for colliders
	CalculateMinMaxPoint(colMatRot, matRot);
	// Calculation in the plane direction
	CalculateDirect(colMatRot, matRot);
}

void BoxCollider::CalculateCenter(const XMMATRIX& matRot)
{
	const XMMATRIX& matWorld = object->GetMatWorld();

	auto rotOffset = offset;

	// Rotate according to the object's degree of rotation
	rotOffset = XMVector3TransformNormal(rotOffset, matRot);

	Box::center = matWorld.r[3] + rotOffset;
}

void BoxCollider::CalculateMinMaxPoint(const XMMATRIX& colMatRot, const XMMATRIX& matRot)
{
	// Calculate MAX and MIN in local coordinate system before rotation and translation from scale information
	XMVECTOR minOffset = -XMVECTOR{ scale.x, scale.y, scale.z, 0 };
	XMVECTOR maxOffset = XMVECTOR{ scale.x, scale.y, scale.z, 0 };

	Box::rotation = object->GetRotation();
	// Rotates to match the degree of rotation of the collider
	minOffset = XMVector3TransformNormal(minOffset, colMatRot);
	maxOffset = XMVector3TransformNormal(maxOffset, colMatRot);

	// Add offset from the object's center position
	minOffset += offset;
	maxOffset += offset;

	// Rotate according to the object's degree of rotation
	minOffset = XMVector3TransformNormal(minOffset, matRot);
	maxOffset = XMVector3TransformNormal(maxOffset, matRot);

	// Convert to world coordinate system
	Box::minPosition = objCenter + minOffset;
	Box::maxPosition = objCenter + maxOffset;
}

void BoxCollider::CalculateDirect(const XMMATRIX& colMatRot, const XMMATRIX& matRot)
{
	// Each axial direction before rotation
	XMVECTOR x = { 1,0,0,0 };
	XMVECTOR y = { 0,1,0,0 };
	XMVECTOR z = { 0,0,1,0 };

	// Rotates to match the degree of rotation of the collider
	x = XMVector3TransformNormal(x, colMatRot);
	y = XMVector3TransformNormal(y, colMatRot);
	z = XMVector3TransformNormal(z, colMatRot);

	// Rotate according to the object's degree of rotation
	x = XMVector3TransformNormal(x, matRot);
	y = XMVector3TransformNormal(y, matRot);
	z = XMVector3TransformNormal(z, matRot);

	// Normalize and store
	normaDirect[0] = Vector3(x).Normalize().ConvertXMVECTOR();
	normaDirect[1] = Vector3(y).Normalize().ConvertXMVECTOR();
	normaDirect[2] = Vector3(z).Normalize().ConvertXMVECTOR();
}