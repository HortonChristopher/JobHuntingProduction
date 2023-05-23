#pragma once

#include <DirectXMath.h>

class Object3d;
class BaseCollider;

/// <summary>
/// Structure for obtaining information by querying
/// </summary>
struct QueryHit
{
	// Objects to collide with
	Object3d* object = nullptr;

	// Collision partner collider
	BaseCollider* collider = nullptr;

	// Point of collision
	DirectX::XMVECTOR inter;

	// Rejection vector
	DirectX::XMVECTOR reject;
};

/// <summary>
/// Class that specifies behavior when a query detects an intersection
/// </summary>
class QueryCallback
{
public:
	QueryCallback() = default;

	virtual ~QueryCallback() = default;

	/// <summary>
	/// Call-back at crossing
	/// </summary>
	/// <param name="info">Intersection information</param>
	/// <returns>Returns true to continue query, false to terminate</returns>
	virtual bool OnQueryHit(const QueryHit& info) = 0;
};