#pragma once

#include <array>
#include <DirectXMath.h>

#include "Camera.h"
#include "Vector.h"

/// <summary>
/// Collision Detection Primitive
/// </summary>

/// <summary>
/// Ball/Sphere
/// </summary>
struct Sphere
{
	// Center Coordinates
	DirectX::XMVECTOR center = { 0,0,0,1 };
	// Radius
	float radius = 1.0f;
};

/// <summary>
/// Plane
/// </summary>
struct Plane
{
	// Normal Vector
	DirectX::XMVECTOR normal = { 0, 1, 0, 0 };

	// Distance from the origin
	float distance = 0.0f;
};

/// <summary>
/// Triangle with traced line (clockwise is the surface)
/// </summary>
class Triangle
{
public:
	// 3 vertex coordinates
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;

	// Normal Vector
	DirectX::XMVECTOR normal;

	/// <summary>
	/// Normal calculation
	/// </summary>
	void ComputeNormal();
};

/// <summary>
/// Ray (semi-linear)
/// </summary>
struct Ray
{
	// Start point coordinates
	DirectX::XMVECTOR start = { 0,0,0,1 };

	// Direction
	DirectX::XMVECTOR dir = { 1,0,0,0 };
};

// Capsule
struct Capsule
{
	DirectX::XMVECTOR startPosition = {};
	DirectX::XMVECTOR endPosition = {};

	// Radius
	float radius = 1.0f;
};

// Box
struct Box
{
	// Centerpoint
	DirectX::XMVECTOR center = {};

	std::array<DirectX::XMVECTOR, 3> normaDirect;

	// Size
	Vector3 scale = { 1,1,1 };

	Vector3 minPosition = {};
	Vector3 maxPosition = {};

	Vector3 rotation = {};
};