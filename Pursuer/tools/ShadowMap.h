#pragma once

#include <array>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "Vector.h"
#include "PipelineStatus.h"
#include "Textures.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "GameWindow.h"

class ShadowMap
{
public:
	ShadowMap();

	static void SetWindow(GameWindow* window) { ShadowMap::gameWindow = window; }

	void PreDraw();

	void Draw();

	void PostDraw();
private:
	void Initialize();
	static GameWindow* gameWindow;

	// For shader use
	struct ConstBufferData {
		DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX mat;
	};

	struct Vertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	std::array<Vertex, 4> vertices;
	D3D12_VERTEX_BUFFER_VIEW vbView{}; // Vertex Buffer View
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff; // Vertex Buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff; // Constant Buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> shadowResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap; // For Render Targets
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap; // For texture
	ID3D12GraphicsCommandList* cmdList;
	ID3D12Device* device;

	const int resourceWidth = 1920;
	const int resourceHeight = 1080;
};