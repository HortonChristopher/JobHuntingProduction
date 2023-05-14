#pragma once

#include <array>
#include <assert.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

#include "DirectXCommon.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"
#include "Vector.h"

using namespace DirectX;

/// <summary>
/// Light class
/// </summary>
class LightGroup
{
private: // Alias
	// Using Microsoft::WRL::
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// Using DirectX::
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // Constant
	// Number of parallel light sources
	static const int DirLightNum = 3;
	// Number of point light sources
	static const int PointLightNum = 9;
	// Number of spotlights
	static const int SpotLightNum = 3;
	// Number of round shadows
	static const int CircleShadowNum = 1;

public: // Subclass
	// Data structure for constant buffer
	struct ConstBufferData
	{
		// Color of ambient light
		XMFLOAT3 ambientColor;
		float pad;
		// For Parallel Light Source
		std::array<DirectionalLight::ConstBuff, DirLightNum> dirLights;
		// For point light source
		std::array<PointLight::ConstBuff, PointLightNum> pointLights;
		// For Spotlight
		std::array<SpotLight::ConstBuff, SpotLightNum> spotLights;
		// for round shadows
		std::array<CircleShadow::ConstBuff, CircleShadowNum> circleShadows;
	};

public: // Static member function
	/// <summary>
	/// Instance creation
	/// </summary>
	/// <returns>Instance</returns>
	static LightGroup* Create();

public: // Member function
	void Initialize();
	void TransferConstBuff();

	// Directional light source
	void SetAmbientColor(const Vector3& color);
	void SetDirLightActive(int index, bool active);
	void SetDirLightDir(const int& index, const XMVECTOR& lightdir);
	void SetDirLightColor(const int& index, const Vector3 lightcolor);

	// Positional light source
	void SetPointLightActive(int index, bool active);
	void SetPointLightPos(int index, const Vector3& pos);
	void SetPointLightColor(int index, const Vector3& color);
	void SetPointLightAtten(int index, const Vector3& atten);

	// Spotlight
	void SetSpotLightActive(int index, bool active);
	void SetSpotLightDir(int index, const XMVECTOR& dir);
	void SetSpotLightPos(int index, const Vector3& pos);
	void SetSpotLightColor(int index, const Vector3& color);
	void SetSpotLightAtten(int index, const Vector3& atten);
	void SetSpotLightFactorAngle(int index, const XMFLOAT2& lightFactorAngle);

	// Circle shadow
	void SetCircleShadowActive(int index, bool active);
	void SetCircleShadowCasterPos(int index, const Vector3& pos);
	void SetCircleShadowDir(int index, const XMVECTOR& dir);
	void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);
	void SetCircleShadowAtten(int index, const Vector3& atten);
	void SetCircleShadowFactorAngle(int index, const XMFLOAT2& factorAngle);

	void Update();
	void Draw(UINT rootParameterIndex);
private: // Member variable
	ComPtr<ID3D12Resource> constBuff; // Constant buffer

	XMFLOAT3 ambientColor{ 1,1,1 };

	// Array of parallel light sources
	std::array<DirectionalLight, DirLightNum> dirLights;

	// Array of point light sources
	std::array<PointLight, PointLightNum> pointLights;

	// Array of spotlights
	std::array<SpotLight, SpotLightNum> spotLights;

	// Array of round shadows
	std::array<CircleShadow, CircleShadowNum> circleShadows;

	bool dirty = false;
};

