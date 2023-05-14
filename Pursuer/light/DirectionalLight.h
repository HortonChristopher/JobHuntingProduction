#pragma once

#include <DirectXMath.h>

#include "Vector.h"

class DirectionalLight
{
private:
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // Data for buffer
	struct ConstBuff
	{
		XMVECTOR lightv;
		XMFLOAT3 lightcolor;
		float pad;
	};

public: // member function
	void SetLightDir(const XMVECTOR& lightDir);

	inline const XMVECTOR& GetLightDir() { return lightDir; }

	void SetLightColor(const Vector3& lightColor);

	inline const Vector3& GetLightColor() { return lightColor; }

	inline void SetActive(bool active) { this->active = active; }

	inline bool IsActive() { return active; }

private:
	XMVECTOR lightDir = { 1,0,0,0 };
	Vector3 lightColor = { 1,1,1 };

	bool dirty = false;
	bool active = false;
};