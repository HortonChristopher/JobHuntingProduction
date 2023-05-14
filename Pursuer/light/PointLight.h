#pragma once

#include <DirectXMath.h>
#include <Windows.h>

#include "Vector.h"

class PointLight
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
		XMFLOAT3 lightpos;
		float pad1;
		XMFLOAT3 lightcolor;
		float pad2;
		XMFLOAT3 lightatten;
		UINT active;
	};
public: // Member function
	inline void SetLightPos(const Vector3& pos) { this->lightpos = pos; }

	inline const Vector3& GetLightPos() { return lightpos; }

	inline void SetLightColor(const Vector3& color) { this->lightcolor = color; }

	inline const Vector3& GetLightColor() { return lightcolor; }

	inline void SetLightAtten(const Vector3& atten) { this->lightAtten = atten; }

	inline const Vector3& GetLightAtten() { return lightAtten; }

	inline void SetActive(bool active) { this->active = active; }

	inline bool IsActive() { return active; }
private: // Member function
	Vector3 lightpos = { 0,0,0 }; // Light coordinates (world coordinate system)
	Vector3 lightcolor = { 1,1,1 }; // Light Color
	Vector3 lightAtten = { 0.001f,0.001f,0.001f }; // Distance attenuation coefficient of lights

	bool active = false;
};