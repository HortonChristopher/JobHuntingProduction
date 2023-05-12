#include "Textures.h"

std::unordered_map<std::string, TexMetadata> Textures::metaData = {};
std::unordered_map<std::string, ComPtr<ID3D12Resource>> Textures::texBuffs = {};
std::unordered_map<std::string, unsigned char> Textures::texIndexes = {};
std::unordered_map<std::string, XMINT2> Textures::texSizeCreation = {};

ComPtr<ID3D12DescriptorHeap> Textures::basicDescHeaps = nullptr;

unsigned char Textures::texIndex = 0;

void Textures::LoadTexture(const std::string& name, const std::string& texName)
{
	HRESULT result;

	auto dev = DirectXCommon::GetInstance()->GetDevice();

	TexMetadata metadata{};
	ScratchImage scratchlmg{};

	// Combine file paths
	std::string filepath = "Resources/Texture/" + texName;
	wchar_t wfilepath[128];

	// Convert to Unicode string
	MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));

	result = LoadFromWICFile(
		wfilepath,
		WIC_FLAGS_NONE,
		&metadata, scratchlmg);

	const Image* img = scratchlmg.GetImage(0, 0, 0); // Raw data extraction

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	ComPtr < ID3D12Resource> texbuff;
	result = dev->CreateCommittedResource( // GPU resource generation
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // Designation for Texture
		nullptr,
		IID_PPV_ARGS(&texbuff));

	// Data Transfer
	//  Data transfer to texture buffer
	result = texbuff->WriteToSubresource(
		0,
		nullptr, // Copy to all areas
		img->pixels, // Address to original data
		(UINT)img->rowPitch, // 1 line size
		(UINT)img->slicePitch // Total size
	);

	metaData[name] = metadata;
	texBuffs[name] = texbuff;

	// Shader Resource View Creation
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};// Setup structure
	srvDesc.Format = metadata.format; // RGBA format
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D texture
	srvDesc.Texture2D.MipLevels = 1;

	dev->CreateShaderResourceView(texBuffs[name].Get(), // Buffer to associate with view
		&srvDesc, // Texture setting information
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescHeaps->GetCPUDescriptorHandleForHeapStart(),
			texIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	texIndexes[name] = texIndex;
	texIndex++;
}

void Textures::CreateTexture(const std::string& name, const int& width, const int& height, XMFLOAT4* color)
{
	HRESULT result;

	auto dev = DirectXCommon::GetInstance()->GetDevice();

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		(UINT64)width,
		(UINT)height,
		1,
		1
	);

	ComPtr < ID3D12Resource> texbuff;
	result = dev->CreateCommittedResource( // GPU resource generation
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // Designation for Texture
		nullptr,
		IID_PPV_ARGS(&texbuff));

	result = texbuff->WriteToSubresource(
		0,
		nullptr, // Copy to all areas
		color, // Address to original data
		sizeof(XMFLOAT4) * width, // 1 line size
		sizeof(XMFLOAT4) * width * height // Total size
	);

	texBuffs[name] = texbuff;
	texSizeCreation[name].x = width;
	texSizeCreation[name].y = height;

	// Shader Resource View Creation
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // Setup structure
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // RGBA format
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2D texture
	srvDesc.Texture2D.MipLevels = 1;


	dev->CreateShaderResourceView(texBuffs[name].Get(), // Buffer to associate with view
		&srvDesc, // Texture setting information
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescHeaps->GetCPUDescriptorHandleForHeapStart(),
			texIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	texIndexes[name] = texIndex;
	texIndex++;
}

void Textures::AddTexture(const std::string& name, ID3D12Resource* texBuff)
{
	if (texBuffs[name] != nullptr)
		assert(0);

	HRESULT result;

	auto dev = DirectXCommon::GetInstance()->GetDevice();

	texBuffs[name] = texBuff;

	//シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // Setup structure
	srvDesc.Format = texBuff->GetDesc().Format;; // RGBA format
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D texture
	srvDesc.Texture2D.MipLevels = 1;

	dev->CreateShaderResourceView(texBuffs[name].Get(), // Buffer to associate with view
		&srvDesc, // Texture setting information
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescHeaps->GetCPUDescriptorHandleForHeapStart(),
			texIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	texIndexes[name] = texIndex;
	texIndex++;
}

void Textures::SendColor(const std::string& name, XMFLOAT4* color)
{
	HRESULT result;

	result = texBuffs[name]->WriteToSubresource(
		0,
		nullptr, // Copy to all areas
		color, // Address to original data
		sizeof(XMFLOAT4) * texSizeCreation[name].x, // 1 line size
		sizeof(XMFLOAT4) * texSizeCreation[name].x * texSizeCreation[name].y // Total size
	);
}

void Textures::Initialize()
{
	HRESULT result;

	// Setup structure
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // Visible through shaders
	descHeapDesc.NumDescriptors = 255;

	// Generation
	result = DirectXCommon::GetInstance()->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeaps));
	assert(SUCCEEDED(result));
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Textures::GetGpuDescHandleSRV(const std::string& name)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE descHeap = CD3DX12_GPU_DESCRIPTOR_HANDLE(basicDescHeaps->GetGPUDescriptorHandleForHeapStart(),
		texIndexes[name], DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	return descHeap;
}