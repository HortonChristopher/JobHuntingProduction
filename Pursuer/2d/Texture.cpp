#include "Texture.h"
#include "DirectXCommon.h"

std::unordered_map<std::string, DirectX::TexMetadata> Texture::textureMetaData = {};
std::unordered_map<std::string, ComPtr <ID3D12Resource>> Texture::textureIndexes = {};
ComPtr <ID3D12DescriptorHeap> Texture::basicDescriptorHeap = nullptr;
std::unordered_map<std::string, unsigned char>Texture::textureIndexes = {};
unsigned char Texture::textureIndex = 0;
std::unordered_map<std::string, DirectX::XMINT2> Texture::createTextureSize = {};

void Texture::LoadTexture(const std::string& name, const std::string& textureName)
{
	HRESULT result;
	auto dev = DirectXCommon::GetInstance()->GetDevice();

	DirectX::TexMetadata metaData{};
	DirectX::ScratchImage scratchImg{};

	// File path
	std::string filePath = "Resources/Sprite/" + textureName;
	wchar_t wFilePath[128];

	MultiByteToWideChar(CP_ACP, 0, filePath.c_str(), -1, wFilePath, _countof(wFilePath));
	
	result = DirectX::LoadFromWICFile(
		wFilePath,
		DirectX::WIC_FLAGS_NONE,
		&metaData, scratchImg);

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0); // Raw Data

	CD3DX12_RESOURCE_DESC textureResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metaData.format,
		metaData.width,
		(UINT)metaData.height,
		(UINT16)metaData.arraySize,
		(UINT16)metaData.mipLevels
	);

	ComPtr < ID3D12Resource> textureBuffer;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&textureResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuffer));

	// Data transfer to texture buffer
	result = textureBuffer->WriteToSubresource(
		0,
		nullptr, // Copy to all areas
		img->pixels, // Address to original data
		(UINT)img->rowPitch, // 1 line size
		(UINT)img->slicePitch // Whole Size
	);

	textureMetaData[name] = metaData;
	textureBuffers[name] = textureBuffer;

	// Shader resource view creation
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // Configuration Structure
	srvDesc.Format = metaData.format; // RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D Texture Component
	srvDesc.Texture2D.MipLevels = 1;

	dev->CreateShaderResourceView(textureBuffers[name].Get(), // Associate view buffer
		&srvDesc, // Texture setting information
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			textureIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	textureIndexes[name] = textureIndex;

	textureIndex++;
}

void Texture::CreateTexture(const std::string& name, const int& width, const int& height, DirectX::XMFLOAT4* color)
{
	HRESULT result;
	auto dev = DirectXCommon::GetInstance()->GetDevice();

	CD3DX12_RESOURCE_DESC textureResDescriptor = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		(UINT64)width,
		(UINT)height,
		1,
		1
	);


	ComPtr < ID3D12Resource> textureBuffer;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&textureResDescriptor,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuffer));

	result = textureBuffer->WriteToSubresource(
		0,
		nullptr,
		color,
		sizeof(DirectX::XMFLOAT4) * width,
		sizeof(DirectX::XMFLOAT4) * width * height
	);

	textureBuffers[name] = textureBuffer;
	createTextureSize[name].x = width;
	createTextureSize[name].y = height;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescriptor{};
	srvDescriptor.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDescriptor.Texture2D.MipLevels = 1;


	dev->CreateShaderResourceView(textureBuffers[name].Get(),//ビューと関連付けるバッファ
		&srvDescriptor,//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			textureIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	textureIndexes[name] = textureIndex;

	textureIndex++;
}

void Texture::AddTexture(const std::string& name, ID3D12Resource* textureBuffer)
{
	if (textureBuffers[name] != nullptr) assert(0);
	auto dev = DirectXCommon::GetInstance()->GetDevice();

	textureBuffers[name] = textureBuffer;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureBuffer->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;


	dev->CreateShaderResourceView(textureBuffers[name].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			textureIndex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	textureIndexes[name] = textureIndex;

	textureIndex++;
}

void Texture::ChangeColor(const std::string& name, DirectX::XMFLOAT4* color)
{
	HRESULT result;
	result = textureBuffers[name]->WriteToSubresource(
		0,
		nullptr,
		color,
		sizeof(DirectX::XMFLOAT4) * createTextureSize[name].x,
		sizeof(DirectX::XMFLOAT4) * createTextureSize[name].x * createTextureSize[name].y
	);
}

void Texture::Initialize()
{
	HRESULT result;

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 255;

	result = DirectXCommon::GetInstance()->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescriptorHeap));
	assert(SUCCEEDED(result));
}