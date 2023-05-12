#include "PipelineStatus.h"
#include <d3dcompiler.h>

std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> PipelineStatus::rootsignature = {};
std::unordered_map<std::string, std::unordered_map<BLENDING, ComPtr<ID3D12PipelineState>>> PipelineStatus::pipelinestate = {};

void PipelineStatus::SetPipeline(const std::string& keyName, const BLENDING blendType)
{
	// Nullptr assertion check
	if (rootsignature[keyName].Get() == nullptr || pipelinestate[keyName][blendType].Get() == nullptr)
	{
		assert(0);
	}

	// Get command list
	auto cmdList = DirectXCommon::GetInstance()->GetCommandList();

	cmdList->SetGraphicsRootSignature(rootsignature[keyName].Get());
	cmdList->SetPipelineState(pipelinestate[keyName][blendType].Get());
}

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

void PipelineStatus::CreatePipeline(const std::string& keyName, const SHADER shader, const BLENDING blendType)
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
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 register
	// Depth Stencil State Settings
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Performing depth testing.
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;// Specified RGBA from 0 to 255

	// Array of Root Parameters
	std::vector<CD3DX12_ROOT_PARAMETER> rootparams = {};

	// Vertex Layout
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {};

	switch (shader)
	{
	case BasePostEffect:
	{
		// Load and Compile Vertex Shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/SpriteVS.hlsl",  // Shader File Name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling Include
			"main", "vs_5_0", // Entry Point Name, Shader Model Name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Debugging Setup
			0, &vsBlob, &errorBlob);

		// Load and Compile Pixel Shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/BasePostEffectPS.hlsl",  // Shader File Name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling Include
			"PSmain", "ps_5_0", // Entry Point Name, Shader Model Name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Debugging Setup
			0, &psBlob, &errorBlob);

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

		// Vertex Layout
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // XY coordinates
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // UV coordinate
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		// Descriptor range's second setting
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
		descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 register

		// Descriptor range's third setting		
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV3;
		descRangeSRV3.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); // t2 register

		// Descriptor range's fourth setting		
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV4;
		descRangeSRV4.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3); // t3 register

		// Setting Route Parameters
		CD3DX12_ROOT_PARAMETER param;
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		rootparams.push_back(param);

		// For Texture
		// First texture
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// Second texture
		param.InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// Third texture
		param.InitAsDescriptorTable(1, &descRangeSRV3, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// Fourth texture
		param.InitAsDescriptorTable(1, &descRangeSRV4, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // Always Override Rule

		break;
	}
	case NormalShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT; // R-only 32-bit FLOAT format
		// Load and compile vertex shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapVS.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"VSmain", "vs_5_0", // Entrypoint name、Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &vsBlob, &errorBlob);

		// Load and compile pixel shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"PSmain", "ps_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &psBlob, &errorBlob);

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

		// Vertex layout
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; //xy coordinates
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		// Setting Route Parameters
		CD3DX12_ROOT_PARAMETER param;
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // Always Override Rule

		break;
	}
	case FBXShadowMap:
	{
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT; // R-only 32-bit FLOAT format
		// Load and compile vertex shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapFBXVS.hlsl", // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"VSmain", "vs_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &vsBlob, &errorBlob);

		//ピクセルシェーダの読み込みとコンパイル
		result = D3DCompileFromFile(
			L"Resources/Shader/ShadowMapPS.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"PSmain", "ps_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &psBlob, &errorBlob);

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

		// Vertex layout
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // xy coordinates
		inputLayout.push_back(layout);

		layout =
		{ // Affected Bone Numbers (4)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);

		layout =
		{ // Bone Skinning Weights (4)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		// Setting Route Parameters
		// Zero
		CD3DX12_ROOT_PARAMETER param;
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		rootparams.push_back(param);
		// First
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case SPRITE:
	{
		gpipeline.BlendState.AlphaToCoverageEnable = true;
		// Load and compile vertex shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/SpriteVertexShader.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"VSmain", "vs_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &vsBlob, &errorBlob);
		// Load and compile pixel shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/SpritePixelShader.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"PSmain", "ps_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &psBlob, &errorBlob);

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
		// Vertex layout
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // xy coordinates
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // uv coordinates
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();
		CD3DX12_ROOT_PARAMETER param;
		// Zero
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		rootparams.push_back(param);

		// For Texture
		// First
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case PARTICLE:
	{
		// Load and compile vertex shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/ParticleVS.hlsl",	// Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"main", "vs_5_0",	// Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0,
			&vsBlob, &errorBlob);

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

		// Load and compile pixel shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/ParticlePS.hlsl",	// Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"main", "ps_5_0",	// Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0,
			&psBlob, &errorBlob);

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

		// Loading and Compiling Geometry Shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/ParticleGS.hlsl",	// Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"main", "gs_5_0",	// Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0,
			&gsBlob, &errorBlob);

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

		// Vertex layout
		D3D12_INPUT_ELEMENT_DESC
			layout =
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"SCALE",0,DXGI_FORMAT_R32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"ROTATION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"BILLBOARD",0,DXGI_FORMAT_R32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD0a",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD1a",0,DXGI_FORMAT_R32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD2a",0,DXGI_FORMAT_R32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD3a",0,DXGI_FORMAT_R32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD4a",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		// Prohibit the writing of depths
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		CD3DX12_ROOT_PARAMETER param;
		// Zero
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		rootparams.push_back(param);

		// For texture
		// First
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case BasicObj:
	{
		// Load and compile vertex shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/BasicVertexShader.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"VSmain", "vs_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &vsBlob, &errorBlob);

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

		// Load and compile pixel shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/BasicPixelShader.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"PSmain", "ps_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &psBlob, &errorBlob);

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

		// Vertex layout
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // xy coordinates
		inputLayout.push_back(layout);
		layout =
		{ // Normal vector
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // uv coordinates
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		// Setting Route Parameters
		CD3DX12_ROOT_PARAMETER param;
		// Zero
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		rootparams.push_back(param);
		// First
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// For texture
		// Second
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// Third
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // Specified RGBA from 0 to 255 
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		break;
	}
	case NormalMap:
	{
		// Load and compile vertex shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/NormalMapVS.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"VSmain", "vs_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &vsBlob, &errorBlob);

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

		// Load and compile pixel shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/NormalMapPS.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"PSmain", "ps_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &psBlob, &errorBlob);

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

		// Loading and Compiling Geometry Shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/NormalMapGS.hlsl", // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"GSmain", "gs_5_0",	// Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0,
			&gsBlob, &errorBlob);

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

		// Vertex layout
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // xy coordinates
		inputLayout.push_back(layout);
		layout =
		{ // Normal vector
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // uv coordinates
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		// Second descriptor range setting
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
		descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1 register

		// Setting Route Parameters
		CD3DX12_ROOT_PARAMETER param;
		// Zero
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		rootparams.push_back(param);
		// First
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// For texture
		// Second
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// Third
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// For texture
		// Fourth
		param.InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case FBX:
	{
		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // Specified RGBA from 0 to 255 
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Load and compile vertex shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/FBXVS.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"VSmain", "vs_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &vsBlob, &errorBlob);

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

		// Load and compile pixel shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/FBXPS.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"PSmain", "ps_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &psBlob, &errorBlob);

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

		// Vertex layout
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // xy coordinates
		inputLayout.push_back(layout);
		layout =
		{ // Normal vector
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // uv coordinates
		inputLayout.push_back(layout);
		layout =
		{ // Affected Bone Numbers (4)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);
		layout =
		{ // Bone Skinning Weights (4)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();

		// Setting Route Parameters
		CD3DX12_ROOT_PARAMETER param;
		// Zero
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		rootparams.push_back(param);
		// First
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// For texture
		// Second
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// Third
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// Fourth
		param.InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case DrawShadowOBJ:
	{
		// Load and compile vertex shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/DrawShadowOBJVS.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"VSmain", "vs_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &vsBlob, &errorBlob);

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

		// Load and compile pixel shaders
		result = D3DCompileFromFile(
			L"Resources/Shader/DrawShadowOBJPS.hlsl",  // Shader file name
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enabling include
			"PSmain", "ps_5_0", // Entrypoint name, Shader model name
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
			0, &psBlob, &errorBlob);
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

		// Vertex layout
		D3D12_INPUT_ELEMENT_DESC layout = {
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // xy coordinates
		inputLayout.push_back(layout);
		layout =
		{ // Normal vector
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		};
		inputLayout.push_back(layout);

		layout =
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}; // uv coordinates
		inputLayout.push_back(layout);

		gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
		gpipeline.InputLayout.NumElements = inputLayout.size();
		// Second descriptor range setting
		CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
		descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//t1 レジスタ

		// Setting Route Parameters
		CD3DX12_ROOT_PARAMETER param;
		// Zero
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Type
		rootparams.push_back(param);
		// First
		param.InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// For texture
		// Second
		param.InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// For texture
		// Third
		param.InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);
		// Third
		param.InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams.push_back(param);

		// Second sampler setting
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc,samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		break;
	}
	default:
		assert(0);
		break;
	}

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	if (gsBlob != nullptr)
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	if (hsBlob != nullptr)
		gpipeline.HS = CD3DX12_SHADER_BYTECODE(hsBlob.Get());
	if (dsBlob != nullptr)
		gpipeline.DS = CD3DX12_SHADER_BYTECODE(dsBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // Standard setting
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // No culling
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // Culling Background
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	// Blend settings for render targets (there are 8, but only one is used now)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // Standard setting
	// Set to blend state
	blenddesc.BlendEnable = true; // Enable Blending
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // Addition
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // Use 100% of the source value
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // Use 0% of Dest value

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
		if (shader == BasicObj)
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
}