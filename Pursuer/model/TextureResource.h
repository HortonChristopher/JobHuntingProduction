#pragma once

#include "DirectXCommon.h"
#include "Textures.h"
#include "Vector.h"

#include <d3d12.h>
#include <string>
#include <wrl/client.h>

class TextureResource
{
private:
	friend class SSAO;
public:
	TextureResource(const std::string& name, const bool noDepth);
	TextureResource(const std::string& name, const Vector2& size = { 1920,1080 }, const DXGI_FORMAT resourceFormat = DXGI_FORMAT_R8G8B8A8_UNORM, const DirectX::XMFLOAT4& arg_clearColor = { 1,1,1,1 }, const bool noDepth = false);

	void PostDraw(const bool renderTargetReset = true);
	void PreDraw(const UINT arg_numRTD = 1, const float topLeftX = 0, const float topLeftY = 0, const float width = 1920, const float height = 1080, const LONG& left = 0, const LONG& top = 0, const LONG& right = 1920, const LONG& bottom = 1080);
	void DepthClear();
private:
	void Initialize(const std::string& name);
	void ResetRenderTarget();

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 3> resource;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap; // For depth-stencil view
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraRTVHeap; // For render target view
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraSRVHeap; // For shader resource view (texture)

	ID3D12GraphicsCommandList* cmdList;
	ID3D12Device* device;

	DXGI_FORMAT format;
	std::array<float, 4> clearColor;
	int resourceWidth;
	int resourceHeight;
	UINT numRTD = 1;
	bool noDepth;
private:
	static std::vector<TextureResource*> nowRenderTargets;

	static int bbindex; // Back buffer index

	static TextureResource* mainResource;
public:
	static void SetBbIndex()
	{
		bbindex = DirectXCommon::GetInstance()->GetBbIndex();
	}

	static void SetMainResource(TextureResource* ptr)
	{
		mainResource = ptr;
	}
};