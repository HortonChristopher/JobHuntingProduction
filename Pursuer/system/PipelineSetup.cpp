#include "PipelineSetup.h"
#include <d3dcompiler.h>
#include "SafeDelete.h"

std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> PipelineSetup::rootsignature = {};
std::unordered_map<std::string, D3D_PRIMITIVE_TOPOLOGY > PipelineSetup::primitiveTopologies = {};
std::unordered_map<std::string, std::unordered_map<BLENDTYPE, ComPtr<ID3D12PipelineState>>> PipelineSetup::pipelinestate = {};

void PipelineSetup::SetComputePipeline(const std::string& keyName, const BLENDTYPE blendType)
{
	if (rootsignature[keyName].Get() == nullptr || pipelinestate[keyName][blendType].Get() == nullptr)
		assert(0);
	auto cmdList = DirectXCommon::GetInstance()->GetCommandList();
	cmdList->SetComputeRootSignature(rootsignature[keyName].Get());
	cmdList->SetPipelineState(pipelinestate[keyName][blendType].Get());
}

void PipelineSetup::CreatePipeline(const std::string& keyName, const ShaderType shader, const BLENDTYPE blendType)
{
	ID3D12Device* dev = DirectXCommon::GetInstance()->GetDevice();

	HRESULT result;

	// Blobs
	ComPtr < ID3DBlob> vsBlob = nullptr; // Vertex shader object
	ComPtr < ID3DBlob> psBlob = nullptr; // Pixel shader object
	ComPtr < ID3DBlob> gsBlob = nullptr; // Pixel shader object
	ComPtr < ID3DBlob> errorBlob = nullptr; // Error object

	// Static Sampler
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// Root Signature Settings
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	// Descriptor Range Setting
	std::vector<CD3DX12_DESCRIPTOR_RANGE*> descRangeSRVs;

	//Depth Stencil State Settings
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Perform depth testing.
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	primitiveTopologies[keyName] = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0 to 255 Specified RGBA
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;// No culling
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // Standard Setting
	// Set to Blend State
	blenddesc.BlendEnable = true; // Enable Blending
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // Addition
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // Use 100% of Source Value
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // Use 0% of Dest value

	// Array of Root Parameters
	std::vector<CD3DX12_ROOT_PARAMETER> rootparams = {};

	// Vertex Layout
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {};

	switch (shader)
	{
	case POSTEFFECT:
	{
		CompileShader("PostEffectTestVS", vsBlob, errorBlob, VS);
		CompileShader("PostEffectTestPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 1, 4, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		//gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // Always Override Rule

		break;
	}
	case SPRITE:
	{
		CompileShader("SpriteVS", vsBlob, errorBlob, VS);
		CompileShader("SpritePS", psBlob, errorBlob, PS);
		gpipeline.BlendState.AlphaToCoverageEnable = true;

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

		// ÉfÉvÉXÇÃèëÇ´çûÇ›Çã÷é~
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		primitiveTopologies[keyName] = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;

		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case FBX:
	{
		//gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0 to 255 Specified RGBA

		CompileShader("FBXVS", vsBlob, errorBlob, VS);
		CompileShader("FBXPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("BONEINDICES", inputLayout, DXGI_FORMAT_R32G32B32A32_UINT);
		SetVSLayout("BONEWEIGHTS", inputLayout, DXGI_FORMAT_R32G32B32A32_FLOAT);
		//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		SetDescriptorConstantBuffer(rootparams, 4, 2, descRangeSRVs);
		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0 to 255 Specified RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0 to 255 Specified RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R32_FLOAT; // 0 to 255 Specified RGBA

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // D3D12_COMPARISON_FUNC_GREATER;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc,samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case OBJ:
	{
		CompileShader("OBJVertexShader", vsBlob, errorBlob, VS);
		CompileShader("OBJPixelShader", psBlob, errorBlob, PS);

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
	default:
		assert(0);
		break;
	}

	gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
	gpipeline.InputLayout.NumElements = inputLayout.size();

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	if (gsBlob != nullptr) gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // Standard Setting
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	gpipeline.SampleDesc.Count = 1; // One Sampling per Pixel
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT; // Depth Value Format

	ComPtr<ID3DBlob> rootSigBlob;
	// Serialization of Automatic Version Determination
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	// Root Signature Generation
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature[keyName]));
	assert(SUCCEEDED(result));

	// Set Root Signature in Pipeline
	gpipeline.pRootSignature = rootsignature[keyName].Get();

	if (pipelinestate[keyName][blendType] != nullptr)
		pipelinestate[keyName][blendType]->Release();
	switch (blendType)
	{
	case NOBLEND:
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][NOBLEND]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case ALPHA:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		if (shader == OBJ)
		{
			gpipeline.BlendState.RenderTarget[1] = blenddesc;
			gpipeline.BlendState.RenderTarget[2] = blenddesc;
		}

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][ALPHA]));
		if (FAILED(result)) {
			assert(0);
		}
		break;
	case ADD:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][ADD]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case SUB:
		blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][SUB]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case COLORFLIP:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blenddesc.DestBlend = D3D12_BLEND_ZERO;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][COLORFLIP]));
		if (FAILED(result)) {
			assert(0);
		}
		break;
	default:
		break;
	}

	// Delete Ranges Created
	for (auto it = descRangeSRVs.begin(); it != descRangeSRVs.end(); ++it)
	{
		safe_delete(*it);
	}
}

void PipelineSetup::CompileShader(const std::string& shaderName, ComPtr < ID3DBlob>& shaderBlob, ComPtr < ID3DBlob>& errorBlob, const SHADER shaderType)
{
	// Combine File Paths
	std::string filepath = "Resources/Shader/" + shaderName + ".hlsl";
	wchar_t wfilepath[128];

	// Convert to Unicode String
	MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));

	LPCSTR entryName = "", modelName = "";
	switch (shaderType)
	{
	case VS:
		entryName = "VSmain";
		modelName = "vs_5_0";
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
		wfilepath,  // Shader File Name
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // Include-Enable
		entryName, modelName, // Entry Point Name, Shader Model Designation
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for Debugging
		0, &shaderBlob, &errorBlob);

	if (FAILED(result)) {
		// Copy Error Content from errorBlob to String Type
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// Error Contents are Displayed in the Output Window
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}
}

void PipelineSetup::SetVSLayout(const LPCSTR& semantics, std::vector<D3D12_INPUT_ELEMENT_DESC>& arg_layouts, const DXGI_FORMAT& format)
{
	D3D12_INPUT_ELEMENT_DESC layout =
	{
		semantics, 0, format, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}; // UV coordinates
	arg_layouts.push_back(layout);

}

void PipelineSetup::SetDescriptorConstantBuffer(std::vector<CD3DX12_ROOT_PARAMETER>& arg_rootparams, const int arg_constBuffNum, const int arg_descriptorNum, std::vector<CD3DX12_DESCRIPTOR_RANGE*>& arg_descRangeSRVs)
{
	int constBuffNumber = 0;
	int descriptorNumber = 0;

	// Setting Route Parameters
	CD3DX12_ROOT_PARAMETER param;
	arg_descRangeSRVs.resize(arg_descriptorNum);

	// The Model's Texture Location is Fixed, Forcing Multiple Const buffers and texture areas to fit together when they are created.
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
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		arg_rootparams.push_back(param);
		constBuffNumber++;
	}

	if (arg_descriptorNum != 0)
	{
		arg_descRangeSRVs[descriptorNumber] = new CD3DX12_DESCRIPTOR_RANGE;
		arg_descRangeSRVs[descriptorNumber]->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, descriptorNumber);

		param.InitAsDescriptorTable(1, arg_descRangeSRVs[descriptorNumber], D3D12_SHADER_VISIBILITY_ALL); // Type
		arg_rootparams.push_back(param);
		descriptorNumber++;
	}

	for (; constBuffNumber < arg_constBuffNum; constBuffNumber++)
	{
		param.InitAsConstantBufferView(constBuffNumber, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		arg_rootparams.push_back(param);
	}

	for (; descriptorNumber < arg_descriptorNum; descriptorNumber++)
	{
		arg_descRangeSRVs[descriptorNumber] = new CD3DX12_DESCRIPTOR_RANGE;
		arg_descRangeSRVs[descriptorNumber]->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, descriptorNumber);

		param.InitAsDescriptorTable(1, arg_descRangeSRVs[descriptorNumber], D3D12_SHADER_VISIBILITY_ALL); // Type
		arg_rootparams.push_back(param);
	}
}