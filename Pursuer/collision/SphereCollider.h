#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include <DirectXMath.h>

/// <summary>
/// Ball collision detection object
/// </summary>
class SphereCollider : public BaseCollider, public Sphere
{
private: //Alias
	// Use DirectX::
	using XMVECTOR = DirectX::XMVECTOR;
public:
	SphereCollider(XMVECTOR offset = { 0,0,0,0 }, float radius = 1.0f) : offset(offset), radius(radius)
	{
		// Set the spherical shape
		shapeType = COLLISIONSHAPE_SPHERE;
	}

	/// <summary>
	/// Update
	/// </summary>
	void Update() override;

	inline const Vector3& GetMax()
	{
		Vector3 tmpMax = Vector3(Sphere::center.m128_f32[0] + radius, Sphere::center.m128_f32[1] + radius, Sphere::center.m128_f32[2] + radius);

		return tmpMax;
	}

	inline const Vector3& GetMin()
	{
		Vector3 tmpMin = Vector3(Sphere::center.m128_f32[0] - radius, Sphere::center.m128_f32[1] - radius, Sphere::center.m128_f32[2] - radius);

		return tmpMin;
	}

	inline void SetRadius(float radius) { this->radius = radius; }

	inline const XMVECTOR& GetOffset() { return offset; }

	inline void SetOffset(const XMVECTOR& offset) { this->offset = offset; }

	inline float GetRadius() { return radius; }
private:
	// Offset from object center
	XMVECTOR offset;
	// Radius
	float radius;
};