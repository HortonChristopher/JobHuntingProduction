#pragma once

#include <d3dcompiler.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>

#include "DirectXCommon.h"
#include "Object3D.h"
#include "PipelineStatus.h"
#include "ParticleParameters.h"
#include "Vector.h"

class ShadingComputation
{
public:
	ShadingComputation(const int numElement);

	void Update(void* sendInputData1, void* sendInputData2, const int element);

	void Dispatch();

	static ID3D12PipelineState* GetPipeline() { return pipeline.Get(); }
private:
	void CreateInputResource();

	void CreateOutputResource();

	void CreateDescHeap();

	Microsoft::WRL::ComPtr<ID3D12Resource> outputResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> inputResource1;
	Microsoft::WRL::ComPtr<ID3D12Resource> inputResource2;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;

	void* outputData = nullptr;
	void* inputData1 = nullptr;
	void* inputData2 = nullptr;

	int element;
	int maxElement;
public: // Static member function
	static void StaticInitialize();

	OutputData* GetOutputData();

private: // Static Member Variables and Functions
	static void CreatePipeline();

	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline;

	static ID3D12Device* device;
	static ID3D12GraphicsCommandList* cmdList;
};