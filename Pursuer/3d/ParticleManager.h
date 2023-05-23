#pragma once

#include <forward_list>
#include <unordered_map>
#include <string>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

#include "Particle.h"
#include "ShadingComputation.h"
#include "WrapperComputation.h"
#include "DirectXCommon.h"
#include "PipelineStatus.h"
#include "Textures.h"
#include "Camera.h"

/// <summary>
/// Particle Manager
/// </summary>
class ParticleManager
{
private: // Alias
	// Using Microsoft::WRL::
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// Using DirectX::
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	// Constant Buffer
	struct ConstBufferData
	{
		XMMATRIX mat; // View projection matrix
		XMMATRIX matBillboard; // Billboard matrix
	};

public:
	static ParticleManager* GetInstance();

	void Add(Particle* newParticle,const std::string& TexName);

	void Initialize();

	void Update();

	void Draw();

	inline void SetCamera(Camera* camera) { this->camera = camera; }

	void CreateConstantBuffer();

	void CreateModel();

	void ShutDown();
private:
	ParticleManager() = default;

	ParticleManager(const ParticleManager&) = delete;

	~ParticleManager() = default;

	ParticleManager& operator=(const ParticleManager&) = delete;

	int activeParticleCount = 0;

	// Data not changed on update
	std::unordered_map<std::string,std::vector<ParticleParameters>> parameterData;

	// Data changed during update and sent to VS
	std::unordered_map<std::string, std::vector<OutputData>> vertData;
private:
	Camera* camera = nullptr;

	ComPtr<ID3D12Resource> constBuff;
	ComPtr<ID3D12Resource> vertBuff;

	D3D12_VERTEX_BUFFER_VIEW vbView;

	std::unique_ptr<ShadingComputation> shadingComputation;
	std::vector<OutputData> vert;
	std::vector<ParticleParameters> parameters;
private:
	static const int vertexCount = 10000; // Number of vertices
};

