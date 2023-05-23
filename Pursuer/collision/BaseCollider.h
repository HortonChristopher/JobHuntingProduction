#pragma once

#include "CollisionTypes.h"
#include "BasicOBJ.h"
#include "CollisionInfo.h"

/// <summary>
/// Collider base class
/// </summary>
class BaseCollider
{
public:
	friend class CollisionManager;

	BaseCollider() = default;

	// Virtual destructor
	virtual ~BaseCollider() = default;

	inline void SetObject(BasicOBJ* object) 
	{
		this->object = object; 
	}

	inline BasicOBJ* GetObject3D() 
	{
		return object;
	}

	/// <summary>
	/// Collision callback function
	/// </summary>
	inline void OnCollision(const CollisionInfo& info)
	{
		object->OnCollision(info);
	}

	/// <summary>
	/// Update
	/// </summary>
	virtual void Update() = 0;
	// Shape type acquisition

	inline CollisionShapeType GetShapeType() { return shapeType; }

	inline void SetAttribute(unsigned short attribute) 
	{
		this->attribute = attribute;
	}

	inline void AddAttribute(unsigned short attribute) 
	{
		this->attribute |= attribute;
	}

	inline void RemoveAttribute(unsigned short attribute) 
	{
		this->attribute &= !attribute;
	}

	inline unsigned short GetAttribute()
	{
		return attribute;
	}

protected:
	BasicOBJ* object = nullptr;
	// Shape type
	CollisionShapeType shapeType = SHAPE_UNKNOWN;

	unsigned short attribute = 0b1111111111111111;
};