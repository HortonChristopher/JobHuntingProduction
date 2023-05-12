#pragma once

#include "DirectXCommon.h"

#include <wrl.h>
#include <d3dx12.h>
#include <unordered_map>

using namespace Microsoft::WRL;

class DirectXCommon;

enum BLENDING
{
	NOBLEND,
	ALPHA,
	ADD,
	SUB,
	COLORFLIP,
};

enum SHADER
{
	FBX,
	PARTICLE,
	SPRITE,
	BasePostEffect,
	BasicObj,
	NormalMap,
	NormalShadowMap,
	FBXShadowMap,
	DrawShadowOBJ,
	GPUPARTICLE,
};

class PipelineStatus
{
public:
	static void SetPipeline(const std::string& keyName, const BLENDING blendType = ALPHA);

	static void SetComputePipeline(const std::string& keyName, const BLENDING blendType = ALPHA);

	static void CreatePipeline(const std::string& keyName, const SHADER shader, const BLENDING blendType = ALPHA);
private:
	DirectXCommon* device;

	static std::unordered_map<std::string, ComPtr<ID3D12RootSignature>>rootsignature;
	static std::unordered_map<std::string, std::unordered_map<BLENDING, ComPtr<ID3D12PipelineState>>> pipelinestate;
};