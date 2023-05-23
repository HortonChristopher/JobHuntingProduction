#pragma once

#include <DirectXMath.h>

class BasicOBJ;
class BaseCollider;
/// <summary>
/// Collision information
/// </summary>
struct CollisionInfo
{
public:
	CollisionInfo(Object3d* object, BaseCollider* collider, const DirectX::XMVECTOR& inter, const DirectX::XMVECTOR& reject = {}) {
		this->object = object;
		this->collider = collider;
		this->inter = inter;
		this->reject = reject;
	}

	// Objects to collide with
	BasicOBJ* object = nullptr;

	// Collision partner collider
	BaseCollider* collider = nullptr;

	// Point of collision
	DirectX::XMVECTOR inter;

	// Rejection vector
	DirectX::XMVECTOR reject;
};

