#include "SphereCollider.h"

using namespace DirectX;

void SphereCollider::Update()
{
	// Extract coordinates from world matrix
	const XMMATRIX& matWorld = object->GetMatWorld();

	// Update sphere member variables
	Sphere::center = matWorld.r[3] + offset;
	Sphere::radius = radius;
}