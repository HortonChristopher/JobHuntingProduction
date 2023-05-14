#pragma once

#include <Windows.h>
#include <array>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include "d3dx12.h"
#include <imgui.h>
#include <stdexcept>
#include <cstdlib>
#include <chrono>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include "WinApp.h"
#include "GameWindow.h"
#include "SafeDelete.h"

using namespace Microsoft::WRL;
class GameWindow;

/// <summary>
/// DirectX汎用
/// </summary>
class DirectXCommon
{
public: // メンバ関数
	static DirectXCommon* GetInstance();

	void Initialize(GameWindow* window);

	inline ID3D12Device* GetDevice() { return device.Get(); }
	inline ID3D12GraphicsCommandList* GetCommandList() { return commandList.Get(); }
	ID3D12CommandQueue* GetCmdQueue() { return cmdQueue.Get(); }

	ComPtr<ID3D12DescriptorHeap>& GetDsvHeap() { return dsvHeap; }
	ComPtr<ID3D12DescriptorHeap>& GetRtvHeaps() { return  rtvHeaps; }
	ComPtr<ID3D12Fence>& GetFence() { return fence[bbIndex]; }

	std::vector <ComPtr<ID3D12Resource>>& GetBackBuffers() { return backBuffers; }

	UINT64& GetCurrentFenceVal() { return fenceVal[bbIndex]; }
	inline const UINT& GetBbIndex() { return bbIndex; }

	void ImguiDraw();
	void PreDraw();
	void DepthClear();
	void PostDraw();
	void Finalize();
	void ReRenderTarget();

private: // メンバ変数
	DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	~DirectXCommon() = default;

	static const int frameCount = 3;

	// Window Application Management
	GameWindow* gameWindow;

	// Direct 3D
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Device> device;
	HRESULT CreateDXGI();
	HRESULT CreateCommands();
	HRESULT CreateSwapChain();
	HRESULT CreateDesc();
	HRESULT CreateFence();
	HRESULT CreateDSV();
	HRESULT ImguiInitialize();

	std::vector <ComPtr<IDXGIAdapter>> adapters;
	ComPtr <IDXGIFactory6> dxgiFactory;
	ComPtr <IDXGISwapChain4> swapchain;
	std::array <ComPtr<ID3D12CommandAllocator>, frameCount> cmdAllocator;
	ComPtr <ID3D12CommandQueue> cmdQueue;
	ComPtr <ID3D12DescriptorHeap> rtvHeaps;
	ComPtr <ID3D12DescriptorHeap> dsvHeap;
	ComPtr <ID3D12Resource> depthBuffer;

	ComPtr<ID3D12DescriptorHeap> imguiHeap;

	std::vector <ComPtr<ID3D12Resource>> backBuffers;
	std::array <ComPtr<ID3D12Fence>, frameCount> fence;
	std::array <UINT64, frameCount> fenceVal;

	UINT bbIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
};

