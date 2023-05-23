#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include "Model.h"
#include "Collision.h"
#include "Mesh.h"

#include <DirectXMath.h>

using namespace DirectX;

/// <summary>
/// Mesh collision detection object
/// </summary>
class MeshCollider : public BaseCollider
{
public:
	MeshCollider()
	{
		// Set mesh shape
		shapeType = COLLISIONSHAPE_MESH;
		maxPoint = {};
		minPoint = {};
	}

	/// <summary>
	/// Build an array of triangles
	/// </summary>
	void ConstructTriangles(Model* model);

	/// <summary>
	/// Update
	/// </summary>
	void Update() override;

	inline const Vector3& GetMax()
	{
		auto point = XMVector3Transform(maxPoint, object->GetMatWorld());

		return Vector3(point.m128_f32[0], point.m128_f32[1], point.m128_f32[2]);
	}

	inline const Vector3& GetMin()
	{
		auto point = XMVector3Transform(minPoint, object->GetMatWorld());

		return Vector3(point.m128_f32[0], point.m128_f32[1], point.m128_f32[2]);
	}

	/// <summary>
	/// Collision detection with a sphere
	/// </summary>
	/// <param name="sphere">球</param>
	/// <param name="inter">交点（出力用）</param>
	/// <returns>交差しているか否か</returns>
	bool CheckCollisionSphere(const Sphere& sphere, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	/// <summary>
	/// Collision detection with Ray
	/// </summary>
	/// <param name="sphere">レイ</param>
	/// <param name="distance">距離（出力用）</param>
	/// <param name="inter">交点（出力用）</param>
	/// <returns>交差しているか否か</returns>
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

private:
	void CheckMaxMinPoint(const XMVECTOR& point);

	std::vector<Triangle> triangles;

	// Inverse of the world matrix
	DirectX::XMMATRIX invMatWorld;

	// The vertex with the lowest value position
	XMVECTOR minPoint;

	// Vertex with the highest value position
	XMVECTOR maxPoint;
};