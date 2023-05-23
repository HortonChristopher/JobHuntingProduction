#pragma once

#include "CollisionPrimitive.h"
#include "GameWindow.h"
#include "ImGui.h"

#include <d3d12.h>
#include <string>
#include <utility>

using namespace DirectX;

/// <summary>
/// Collision Detection helper class
/// </summary>
class Collision
{
private:
	static GameWindow* window;
public:
	static void SetWindow(GameWindow* window) { Collision::window = window; }

	/// <summary>
	/// Collision Detection between sphere and sphere
	/// </summary>
	/// <param name="sphere">Sphere/Ball</param>
	/// <param name="inter">Intersection (nearest point of contact on a plane)</param>
	/// <returns>Whether or not they are intersecting</returns>
	//static bool CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::XMVECTOR* inter);
	static bool CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	/// <summary>
	/// Collision Detection between sphere and plane
	/// </summary>
	/// <param name="sphere">��</param>
	/// <param name="plane">����</param>
	/// <param name="inter">��_�i���ʏ�̍ŋߐړ_�j</param>
	/// <returns>�������Ă��邩�ۂ�</returns>
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter = nullptr);

	/// <summary>
	/// Find the most recent contact between a point and a triangle
	/// </summary>
	/// <param name="point">�_</param>
	/// <param name="triangle">�O�p�`</param>
	/// <param name="closest">�ŋߐړ_�i�o�͗p�j</param>
	static void ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point, const Triangle& triangle, DirectX::XMVECTOR* closest);

	/// <summary>
	/// Check for hits between spheres and triangles with normals
	/// </summary>
	/// <param name="sphere">��</param>
	/// <param name="triangle">�O�p�`</param>
	/// <param name="inter">��_�i�O�p�`��̍ŋߐړ_�j</param>
	/// <returns>�������Ă��邩�ۂ�</returns>
	//static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter = nullptr);
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	/// <summary>
	/// Collision detection between ray and plane
	/// </summary>
	/// <param name="ray">���C</param>
	/// <param name="plane">����</param>
	/// <param name="inter">�����i�o�͗p�j</param>
	/// <param name="inter">��_�i�o�͗p�j</param>
	/// <returns>�������Ă��邩�ۂ�</returns>
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	/// <summary>
	/// Collision detection of rays and triangles with normals
	/// </summary>
	/// <param name="ray">���C</param>
	/// <param name="triangle">�O�p�`</param>
	/// <param name="inter">�����i�o�͗p�j</param>
	/// <param name="inter">��_�i�o�͗p�j</param>
	/// /// <returns>�������Ă��邩�ۂ�</returns>
	static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	/// <summary>
	/// Collision detection of ray and sphere
	/// </summary>
	/// <param name="ray">���C</param>
	/// <param name="sphere">��</param>
	/// <param name="inter">�����i�o�͗p�j</param>
	/// <param name="inter">��_�i�o�͗p�j</param>
	/// <returns>�������Ă��邩�ۂ�</returns>
	static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	static bool CheckRay2OBB(const Ray& ray, const Box& obb, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);

	// Determination of sphere/ball and rectangle hits
	static bool CheckSphereBox(const Sphere& sphere, const Box& box, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	// Rectangle-to-rectangle hit detection
	static bool CheckAABBAABB(const Box& box1, const Box& box2, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	static bool CheckOBBOBB(const Box& box1, const Box& box2, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);

	// Calculate projected line segment lengths from the axis components projected on the separated axes.
	static float LengthSegmentOnSeparateAxis(const Vector3& Sep, const Vector3& e1, const Vector3& e2, const Vector3& e3 = Vector3{ 0,0,0 });

	static bool CheckSphereCapsule(const Sphere& sphere, const Capsule& capsule);

	static bool CheckOBBPlane(const Box& obb, const Plane& plane, DirectX::XMVECTOR* inter = nullptr, DirectX::XMVECTOR* reject = nullptr);
};