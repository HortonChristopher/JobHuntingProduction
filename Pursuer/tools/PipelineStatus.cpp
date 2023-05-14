#include "PipelineStatus.h"
#include "SafeDelete.h"

#include <d3dcompiler.h>

std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> PipelineStatus::rootsignature = {};
std::unordered_map<std::string, D3D_PRIMITIVE_TOPOLOGY> PipelineStatus::primitiveTopologies = {};
std::unordered_map<std::string, std::unordered_map<BLENDING, ComPtr<ID3D12PipelineState>>> PipelineStatus::pipelinestate = {};

void PipelineStatus::SetComputePipeline(const std::string& keyName, const BLENDING blendType)
{
	if (rootsignature[keyName].Get() == nullptr || pipelinestate[keyName][blendType].Get() == nullptr)
	{
		assert(0);
	}

	auto cmdList = DirectXCommon::GetInstance()->GetCommandList();

	cmdList->SetComputeRootSignature(rootsignature[keyName].Get());
	cmdList->SetPipelineState(pipelinestate[keyName][blendType].Get());
}

void PipelineStatus::CreatePipeline(const std::string& keyName, const SHADER shaderType, const BLENDING blendType)
{
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	HRESULT result;

	// Blob nullptr set
	ComPtr <ID3DBlob> vsBlob = nullptr; // Vertex Shader object
	ComPtr <ID3DBlob> psBlob = nullptr; // Pixel Shader object
	ComPtr <ID3DBlob> gsBlob = nullptr; // Pixel Shader object
	ComPtr <ID3DBlob> hsBlob = nullptr; // Hull Shader object
	ComPtr <ID3DBlob> dsBlob = nullptr; // Domain Shader object
	ComPtr <ID3DBlob> csBlob = nullptr; // Compute Shader object
	ComPtr <ID3DBlob> errorBlob = nullptr; // Error object

	// Static Sampler
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// Root Signature Settings
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	// Descriptor Range Setting	
	std::vector<CD3DX12_DESCRIPTOR_RANGE*> descRangeSRVs;

	// Depth Stencil State Settings
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Performing depth testing.
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	primitiveTopologies[keyName] = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;// Specified RGBA from 0 to 255
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // Culling Background
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // No culling

	// Blend settings for render targets
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // Standard setting
	// Set to blend state
	blenddesc.BlendEnable = true; // Enable Blending
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // Addition
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // Use 100% of the source value
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // Use 0% of Dest value

	// Array of Root Parameters
	std::vector<CD3DX12_ROOT_PARAMETER> rootparams = {};

	// Vertex Layout
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {};
	
	switch (shaderType)
	{
	case BasePostEffect:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("BasePostEffectPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 1, 4, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case NormalShadowMap:
	{
		CompileShader("ShadowMapVS", vsBlob, errorBlob, VS);
		CompileShader("ShadowMapPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);

		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;

		SetDescriptorConstantBuffer(rootparams, 1, 0, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case FBXShadowMap:
	{
		CompileShader("ShadowMapFBXVS", vsBlob, errorBlob, VS);

		CompileShader("ShadowMapPS", psBlob, errorBlob, PS);

		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("BONEINDICES", inputLayout, DXGI_FORMAT_R32G32B32A32_UINT);
		SetVSLayout("BONEWEIGHTS", inputLayout, DXGI_FORMAT_R32G32B32A32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 2, 0, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case SPRITE:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("SpritePixelShader", psBlob, errorBlob, PS);
		gpipeline.BlendState.AlphaToCoverageEnable = true;

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case Ssao:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("SSAOPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		gpipeline.BlendState.RenderTarget[0].BlendEnable = false;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
		blenddesc.BlendEnable = false; // ブレンドを無効にする

		SetDescriptorConstantBuffer(rootparams, 2, 2, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case SsaoCombine:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("SSAOCombinePS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		SetDescriptorConstantBuffer(rootparams, 1, 6, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case Blur:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("BlurPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case PARTICLE:
	{
		CompileShader("ParticleVS", vsBlob, errorBlob, VS);
		CompileShader("ParticlePS", psBlob, errorBlob, PS);
		CompileShader("ParticleGS", gsBlob, errorBlob, GS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD1a", inputLayout, DXGI_FORMAT_R32_UINT);
		SetVSLayout("ROTATION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("BILLBOARD", inputLayout, DXGI_FORMAT_R32_UINT);
		SetVSLayout("COLOR", inputLayout, DXGI_FORMAT_R32G32B32A32_FLOAT);
		SetVSLayout("SCALE", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("TEXCOORD4a", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("TEXCOORD0a", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD2a", inputLayout, DXGI_FORMAT_R32_UINT);

		// デプスの書き込みを禁止
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		primitiveTopologies[keyName] = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;

		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case BasicObj:
	{
		CompileShader("BasicVertexShader", vsBlob, errorBlob, VS);
		CompileShader("BasicPixelShader", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 3, 2, descRangeSRVs);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc, samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case SkyDome:
	{
		CompileShader("SkyVS", vsBlob, errorBlob, VS);
		CompileShader("SkyPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 2, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		gpipeline.NumRenderTargets = 2;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;

		break;
	}
	case NormalMap:
	{
		CompileShader("NormalMapVS", vsBlob, errorBlob, VS);
		CompileShader("NormalMapPS", psBlob, errorBlob, PS);
		CompileShader("NormalMapGS", gsBlob, errorBlob, GS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 3, 2, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case FBX:
	{
		CompileShader("FBXVS", vsBlob, errorBlob, VS);
		CompileShader("FBXPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("BONEINDICES", inputLayout, DXGI_FORMAT_R32G32B32A32_UINT);
		SetVSLayout("BONEWEIGHTS", inputLayout, DXGI_FORMAT_R32G32B32A32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 4, 2, descRangeSRVs);
		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R32_FLOAT;

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc, samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case DrawShadowOBJ:
	{
		CompileShader("DrawShadowOBJVS", vsBlob, errorBlob, VS);
		CompileShader("DrawShadowOBJPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 4, 2, descRangeSRVs);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc, samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R32_FLOAT;

		break;
	}
	default:
		assert(0);
		break;
	}

	gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
	gpipeline.InputLayout.NumElements = inputLayout.size();

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	if (gsBlob != nullptr)
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	if (hsBlob != nullptr)
		gpipeline.HS = CD3DX12_SHADER_BYTECODE(hsBlob.Get());
	if (dsBlob != nullptr)
		gpipeline.DS = CD3DX12_SHADER_BYTECODE(dsBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // Standard setting
	gpipeline.SampleDesc.Count = 1; // One sampling per pixel
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT; // Depth value format

	ComPtr<ID3DBlob> rootSigBlob;

	// Serialization of automatic version determination
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	// Root Signature Generation
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature[keyName]));
	assert(SUCCEEDED(result));

	// Set root signature in pipeline
	gpipeline.pRootSignature = rootsignature[keyName].Get();

	if (pipelinestate[keyName][blendType] != nullptr)
		pipelinestate[keyName][blendType]->Release();

	switch (blendType)
	{
	case NOBLEND:
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][NOBLEND]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case ALPHA:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		if (shaderType == BasicObj)
		{
			gpipeline.BlendState.RenderTarget[1] = blenddesc;
			gpipeline.BlendState.RenderTarget[2] = blenddesc;
		}

		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][ALPHA]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case ADD:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][ADD]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case SUB:
		blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][SUB]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case COLORFLIP:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blenddesc.DestBlend = D3D12_BLEND_ZERO;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][COLORFLIP]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	default:
		break;
	}

	// Delete created ranges
	for (auto it = descRangeSRVs.begin(); it != descRangeSRVs.end(); ++it)
	{
		safe_delete(*it);
	}
}

void PipelineStatus::CompileShader(const std::string& shaderName, ComPtr<ID3DBlob>& shaderBlob, ComPtr<ID3DBlob>& errorBlob, const SHADERTYPE shaderType)
{
	// Combine file paths
	std::string filepath = "Resources/shaders/" + shaderName + ".hlsl";
	wchar_t wfilepath[128];

	// Convert to Unicode string
	MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));

	LPCSTR entryName = "", modelName = "";
	switch (shaderType)
	{
	case VS:
		entryName = "VSmain";
		modelName = "vs_5_0";
		break;
	case HS:
		entryName = "HSmain";
		modelName = "hs_5_0";
		break;
	case DS:
		entryName = "DSmain";
		modelName = "ds_5_0";
		break;
	case GS:
		entryName = "GSmain";
		modelName = "gs_5_0";
		break;
	case PS:
		entryName = "PSmain";
		modelName = "ps_5_0";
		break;

	default:
		break;
	}

	auto result = D3DCompileFromFile(
		wfilepath, // Shader file name
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
		entryName, modelName, // Entry point name, Shader model designation
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
		0, &shaderBlob, &errorBlob);

	if (FAILED(result)) {
		// Copy error content from errorBlob to string type
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// Error contents are displayed in the output window
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}
}

void PipelineStatus::SetVSLayout(const LPCSTR& semantics, std::vector<D3D12_INPUT_ELEMENT_DESC>& arg_layouts, const DXGI_FORMAT& format)
{
	D3D12_INPUT_ELEMENT_DESC layout =
	{
		semantics, 0, format, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}; // uv coordinates
	arg_layouts.push_back(layout);
}

void PipelineStatus::SetDescriptorConstantBuffer(std::vector<CD3DX12_ROOT_PARAMETER>& arg_rootparams, const int arg_constBuffNum, const int arg_descriptorNum, std::vector<CD3DX12_DESCRIPTOR_RANGE*>& arg_descRangeSRVs)
{
	int constBuffNumber = 0;
	int descriptorNumber = 0;

	// Setting Route Parameters
	CD3DX12_ROOT_PARAMETER param;
	arg_descRangeSRVs.resize(arg_descriptorNum);

	// The model's texture location is fixed, forcing multiple const buffers and texture areas to fit together when they are created.

	if (arg_constBuffNum > 1)
	{
		for (; constBuffNumber < 2; constBuffNumber++)
		{
			param.InitAsConstantBufferView(constBuffNumber, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
			arg_rootparams.push_back(param);
		}
	}
	else if (arg_constBuffNum == 1)
	{
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		arg_rootparams.push_back(param);
		constBuffNumber++;
	}

	if (arg_descriptorNum != 0)
	{
		arg_descRangeSRVs[descriptorNumber] = new CD3DX12_DESCRIPTOR_RANGE;
		arg_descRangeSRVs[descriptorNumber]->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, descriptorNumber);

		param.InitAsDescriptorTable(1, arg_descRangeSRVs[descriptorNumber], D3D12_SHADER_VISIBILITY_ALL);
		arg_rootparams.push_back(param);
		descriptorNumber++;
	}

	for (; constBuffNumber < arg_constBuffNum; constBuffNumber++)
	{
		param.InitAsConstantBufferView(constBuffNumber, 0, D3D12_SHADER_VISIBILITY_ALL);
		arg_rootparams.push_back(param);
	}

	for (; descriptorNumber < arg_descriptorNum; descriptorNumber++)
	{
		arg_descRangeSRVs[descriptorNumber] = new CD3DX12_DESCRIPTOR_RANGE;
		arg_descRangeSRVs[descriptorNumber]->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, descriptorNumber);

		param.InitAsDescriptorTable(1, arg_descRangeSRVs[descriptorNumber], D3D12_SHADER_VISIBILITY_ALL);
		arg_rootparams.push_back(param);
	}
}