#pragma once

#include "BaseCollider.h"

#include <DirectXMath.h>

class Object3d;

/// <summary>
/// Structure for obtaining information by raycast
/// </summary>
struct RaycastHit
{
	// Collision partner object
	Object3d* object = nullptr;

	// Collider
	BaseCollider* collider = nullptr;

	// Collision point
	DirectX::XMVECTOR inter;

	// Distance to the collision point
	float distance = 0.0f;
};