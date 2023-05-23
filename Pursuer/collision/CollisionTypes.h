#pragma once

/// <summary>
/// Collision detection related type definition
/// </summary>
enum CollisionShapeType
{
	SHAPE_UNKNOWN = -1, // ���ݒ� Not set

	COLLISIONSHAPE_SPHERE, // Sphere
	COLLISIONSHAPE_MESH, // Mesh
	COLLISIONSHAPE_BOX, // Box
};