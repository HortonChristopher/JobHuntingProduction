#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "DirectXCommon.h"
#include <unordered_map>
using namespace Microsoft::WRL;

class DirectXCommon;

// Blending Type Enum
enum BLENDTYPE
{
	NOBLEND,
	ALPHA,
	ADD,
	SUB,
	COLORFLIP,
};

enum ShaderType
{
	FBX,
	PARTICLE,
	SPRITE,
	POSTEFFECT,
	OBJ
};

class PipelineSetup
{
public:
	inline static void SetPipeline(const std::string& keyName, const BLENDTYPE blendType = ALPHA)
	{
		if (rootsignature[keyName].Get() == nullptr || pipelinestate[keyName][blendType].Get() == nullptr)
			assert(0);
		auto cmdList = DirectXCommon::GetInstance()->GetCommandList();
		cmdList->SetGraphicsRootSignature(rootsignature[keyName].Get());
		cmdList->SetPipelineState(pipelinestate[keyName][blendType].Get());
		cmdList->IASetPrimitiveTopology(primitiveTopologies[keyName]);

	}
	static void SetComputePipeline(const std::string& keyName, const BLENDTYPE blendType = ALPHA);

	static void CreatePipeline(const std::string& keyName, const ShaderType shader, const BLENDTYPE blendType = ALPHA);
private:
	DirectXCommon* device;

	static std::unordered_map<std::string, D3D_PRIMITIVE_TOPOLOGY> primitiveTopologies;
	static std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> rootsignature;
	static std::unordered_map<std::string, std::unordered_map<BLENDTYPE, ComPtr<ID3D12PipelineState>>> pipelinestate;

	enum SHADER
	{
		VS,
		PS,
		GS,
	};

	static void CompileShader(const std::string& shaderName, ComPtr < ID3DBlob>& shaderBlob, ComPtr < ID3DBlob>& errorBlob, const SHADER shaderType);
	static void SetVSLayout(const LPCSTR& semantics, std::vector<D3D12_INPUT_ELEMENT_DESC>& arg_layouts, const DXGI_FORMAT& format);
	static void SetDescriptorConstantBuffer(std::vector<CD3DX12_ROOT_PARAMETER>& arg_rootparams, const int arg_constBuffNum, const int arg_descriptorNum, std::vector<CD3DX12_DESCRIPTOR_RANGE*>& arg_descRangeSRVs);
};