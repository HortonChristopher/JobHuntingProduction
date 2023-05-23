#pragma once

#include <array>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include <imgui.h>
#include <stdexcept>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

#include "GameWindow.h"
#include "WrapperComputation.h"
#include "d3dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;
class GameWindow;
/// <summary>
/// DirectX General Purpose
/// </summary>
class DirectXCommon
{
private:
	DirectXCommon() = default;

	DirectXCommon(const DirectXCommon&) = delete;

	~DirectXCommon() = default;

	DirectXCommon& operator=(const DirectXCommon&) = delete;

	static const int frameCount = 3;

	GameWindow* window;

	ComPtr<ID3D12GraphicsCommandList> cmdList;

	ComPtr<ID3D12Device> device;

	HRESULT CreateDXGI();
	// DXGI factory object, which is responsible for creating DXGI resources and enumerating adapters.
	ComPtr<IDXGIFactory6> dxgiFactory;

	HRESULT CreateCommands();
	// Used to store multiple command allocators, one for each frame in flight. Command allocators are used to manage memory for command lists.
	std::array<ComPtr<ID3D12CommandAllocator>, frameCount> cmdAllocator;
	// The command queue, which is responsible for executing command lists on the GPU.
	ComPtr<ID3D12CommandQueue> cmdQueue;

	HRESULT CreateSwapChain();
	// DXGI swap chain, which is used for presenting rendered frames to the screen.
	ComPtr<IDXGISwapChain4> swapchain;

	HRESULT CreateDesc();
	// Descriptor heap for render target views (RTVs). Descriptor heaps are used to store and manage descriptors that are bound to the pipeline.
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;

	// A fence is a synchronization object used to signal and wait for GPU operations to complete. Each element in the fence array represents a fence object associated with a specific frame in flight.
	// By using an array of fences, it becomes possible to track the completion of GPU operations on a per-frame basis. This is useful for synchronization and resource management in multi-threaded rendering scenarios, where multiple frames can be in various stages of execution concurrently.
	HRESULT CreateFence();
	std::array<ComPtr<ID3D12Fence>, frameCount> fence;
	std::array<UINT64, frameCount> fenceVal;

	// Descriptor heap for depth-stencil views (DSVs). Similar to the RTV heap, the DSV heap is used to store and manage descriptors for depth-stencil resources.
	HRESULT CreateDSV();
	ComPtr<ID3D12DescriptorHeap> dsvHeap;

	HRESULT ImguiInitialize();

	// Used to store the available DXGI adapters (graphics adapters) on the system.
	std::vector<ComPtr<IDXGIAdapter>> adapters;
	
	// Depth buffer, which is a 2D texture used for storing depth information in a render pass.
	ComPtr<ID3D12Resource> depthBuffer;

	ComPtr<ID3D12DescriptorHeap> imguiHeap;

	std::vector<ComPtr<ID3D12Resource>> backBuffers;

	UINT bbIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
public:
	static DirectXCommon* GetInstance();

	void Initialize(GameWindow* window);

	inline ID3D12Device* GetDevice() { return dev.Get(); }

	inline ID3D12GraphicsCommandList* GetCommandList() { return cmdList.Get(); }

	ID3D12CommandQueue* GetCmdQueue() { return cmdQueue.Get(); }

	inline const UINT& GetBbIndex() { return bbIndex; }

	std::vector<ComPtr<ID3D12Resource>>& GetBackBuffers() { return backBuffers; }

	ComPtr<ID3D12DescriptorHeap>& GetDsvHeap() { return dsvHeap; }

	ComPtr<ID3D12DescriptorHeap>& GetRtvHeaps() { return  rtvHeaps; }

	ComPtr<ID3D12Fence>& GetFence() { return fence[bbIndex]; }

	UINT64& GetCurrentFenceVal() { return fenceVal[bbIndex]; }

	void ImguiDraw();

	void BeginDraw();

	void ClearDepth();

	void EndDraw();

	void ShutDown();

	void ReRenderTarget();

	void BeginWrapperComputation();
};