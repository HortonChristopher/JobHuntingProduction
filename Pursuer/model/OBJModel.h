#pragma once

class OBJModel
{
public:
	OBJModel() = default;

	virtual ~OBJModel() {};

	virtual void Draw() = 0;
};