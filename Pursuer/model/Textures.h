#pragma once

#include <DirectXTex.h>
#include <vector>
#include <unordered_map> 
#include <d3dx12.h>
#include <string>

#include"DirectXCommon.h"

using namespace Microsoft::WRL;
using namespace DirectX;

class Textures
{
public:
	/// <summary>
	/// Load 2D image
	/// </summary>
	/// <param name="name">Name for the list key</param>
	/// <param name="texName">Name of the texture file to be loaded</param>
	static void LoadTexture(const std::string& name, const std::string& texName);

	/// <summary>
	/// Create and register own 2D images
	/// </summary>
	/// <param name="name">Name for the list key</param>
	/// <param name="width">Width of texture to be created</param>
	/// <param name="height">Height of texture to be created</param>
	/// <param name="color">Pointer to color array</param>
	static void CreateTexture(const std::string& name, const int& width, const int& height, XMFLOAT4* color);

	/// <summary>
	/// Register externally created texture buffers in the list
	/// </summary>
	/// <param name="name">Name for the list key</param>
	/// <param name="texBuff">Texture buffer to be registered</param>
	static void AddTexture(const std::string& name, ID3D12Resource* texBuff);

	/// <summary>
	/// Change the color of own 2D image
	/// </summary>
	/// <param name="name">Name for the list key</param>
	/// <param name="color">Pointer to color array</param>
	static void SendColor(const std::string& name, XMFLOAT4* color);

	static void CreatePostEffect();

	static void Initialize();

	static const char& GetTexIndex(const std::string& name) { return texIndexes[name]; };

	static ComPtr < ID3D12Resource> GetTexBuff(std::string name) { return texBuffs[name]; };

	static TexMetadata& GetMetadate(std::string name) { return metaData[name]; };

	static ComPtr < ID3D12DescriptorHeap > GetBasicDescHeap() {
		if (basicDescHeaps == nullptr)
			assert(0);
		return basicDescHeaps;
	};

	static CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuDescHandleSRV(const std::string& name);
private:
	static std::unordered_map<std::string, TexMetadata> metaData;
	static std::unordered_map<std::string, ComPtr <ID3D12Resource>> texBuffs;
	static ComPtr<ID3D12DescriptorHeap> basicDescHeaps;
	static std::unordered_map<std::string, unsigned char> texIndexes;
	static unsigned char texIndex;

	// Variables to save the width and height of own images
	static std::unordered_map<std::string, XMINT2> texSizeCreation;
};