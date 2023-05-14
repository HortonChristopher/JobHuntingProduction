#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>

#include "DirectXCommon.h"
#include "Textures.h"

/// <summary>
/// Material
/// </summary>
class Material
{
private: // Alias
	// Using Microsoft::WRL::
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// Using DirectX::
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // Subclass
	// Data structure for constant buffer B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient; // Ambient coefficient
		float pad1; // Padding
		XMFLOAT3 diffuse; // Diffuse coefficient
		float pad2; // Padding
		XMFLOAT3 specular; // Specular coefficient
		float alpha; // Alpha
	};

public: // Material information
	std::string name;

	XMFLOAT3 ambient;
	XMFLOAT3 diffuse;
	XMFLOAT3 specular;

	float alpha;

	std::string textureFileName;

public: // Static member function
	static Material* Create(); // Creation

public: // Member variables
	void Initialize(); // Initialization

	ID3D12Resource* GetConstBuffer() { return constBuff.Get(); };

	void LoadTexture();

	void Update();

private: // Variables required for interaction with GPU	
	ComPtr<ID3D12Resource> texbuff; // Texture buffer
	ComPtr<ID3D12Resource> constBuff; // Constant buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV; // Shader Resource View Handle CPU
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV; // Shader Resource View Handle GPU
};