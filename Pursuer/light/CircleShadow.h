#pragma once

#include <DirectXMath.h>

#include "Vector.h"

class CircleShadow
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
		XMVECTOR dir;
		XMFLOAT3 casterpos;
		float distanceCasterLight;
		XMFLOAT3 atten;
		float pad;
		XMFLOAT2 factorAngleCos;
		unsigned int active;
		float pad2;
	};
public: // Member function
	inline void SetDir(const XMVECTOR& dir) { this->dir = DirectX::XMVector3Normalize(dir); }

	inline const XMVECTOR& GetDir() { return dir; }

	inline void SetCasterPos(const XMFLOAT3& pos) { this->casterPos = pos; }

	inline const Vector3& GetCasterPos() { return casterPos; }

	inline void SetDistanceCasterLight(float  distanceCasterLight) { this->distanceCasterLight = distanceCasterLight; }

	inline float GetDistanceCasterLight() { return distanceCasterLight; }

	inline void SetAtten(const Vector3& atten) { this->atten = atten; }

	inline const Vector3& GetAtten() { return atten; }

	inline void SetFactorAngle(const XMFLOAT2& factorAngle)
	{
		this->factorAngleCos.x = cosf(DirectX::XMConvertToRadians(factorAngle.x));
		this->factorAngleCos.y = cosf(DirectX::XMConvertToRadians(factorAngle.y));
	}

	inline const XMFLOAT2 GetFactorAngleCos() { return factorAngleCos; }

	inline void SetActive(bool active) { this->active = active; }

	inline bool IsActive() { return active; }
private: // Member variable
	XMVECTOR dir = { 1,0,0,0 }; // Direction (unit vector)
	float distanceCasterLight = 100.0f; // Distance between casters and lights
	Vector3 casterPos = { 0,0,0 }; // Caster coordinates (world coordinate system)
	Vector3 atten = { 0.5f,0.6f,0.0f }; // Distance attenuation coefficient of lights
	XMFLOAT2 factorAngleCos = { 0.2f,0.5f }; // Light attenuation angle (start angle, end angle)
	bool active = false;
};