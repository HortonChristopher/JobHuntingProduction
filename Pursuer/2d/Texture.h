#pragma once
#include <DirectXTex.h>
#include <vector>
#include <unordered_map>
#include <d3dx12.h>
#include <string>

#include "DirectXCommon.h"

using namespace Microsoft::WRL;

class Texture
{
public:
	// Load 2D image
	static void LoadTexture(const std::string& name, const std::string& textureName);

	// Register 2D texture
	static void CreateTexture(const std::string& name, const int& width, const int& height, DirectX::XMFLOAT4* color);

	// Register externally created texture buffers in list
	static void AddTexture(const std::string& name, ID3D12Resource* textureBuffer);

	// Change color of 2D image
	static void ChangeColor(const std::string& name, DirectX::XMFLOAT4* color);

	// Initialization
	static void Initialize();

	// Get Texture Indexes
	static const char& GetTextureIndex(const std::string& name) { return textureIndexes[name]; }

	// Get Texture Buffers
	inline static ComPtr <ID3D12Resource> GetTextureBuffers(std::string name) { return textureBuffers[name]; }
	
	// Get Meta Data
	// static DirectX::TexMetadata& GetMetadata(std::string name) { return metaData[name]; }

	// Get Basic Descriptor Heap
	inline static ComPtr<ID3D12DescriptorHeap> GetBasicDescriptorHeap() 
	{
		if (basicDescriptorHeap == nullptr) assert(0);
		return basicDescriptorHeap;
	}

	// Get GPU Descriptor Handle SRV
	inline static CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleSRV(const std::string& name)
	{
		if (textureIndexes.find(name) == textureIndexes.end()) assert(0);
		CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHeap = CD3DX12_GPU_DESCRIPTOR_HANDLE(basicDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), textureIndexes[name], DirectXCommon::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		return descriptorHeap;
	}

public:
	static std::unordered_map<std::string, DirectX::TexMetadata> textureMetaData;
	static std::unordered_map<std::string, ComPtr<ID3D12Resource>> textureBuffers;
	static ComPtr<ID3D12DescriptorHeap> basicDescriptorHeap;
	static std::unordered_map<std::string, unsigned char> textureIndexes;
	static unsigned char textureIndex;

	// Only for use with self-made images
	static std::unordered_map<std::string, DirectX::XMINT2> createTextureSize;
};