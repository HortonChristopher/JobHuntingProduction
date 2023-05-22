#pragma once

#include <cassert>
#include <d3d12.h>
#include <wrl/client.h>

#include "ShadingComputation.h"
#include "DirectXCommon.h"

class WrapperComputation
{
public:
	static WrapperComputation* GetInstance();

	void Initialize();

	void AddShade(ShadingComputation* shade) { shades.push_back(shade); }

	void End();

	ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }
private:
	WrapperComputation() = default;

	~WrapperComputation() = default;

	WrapperComputation(const WrapperComputation&) = delete;

	WrapperComputation& operator=(const WrapperComputation&) = delete;

	void AsyncComputeThreadProc();

	std::vector<ShadingComputation*> shades;
private:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;

	// volatile indicates that variable may be modified asynchornously by external factors that are not explicitly visible in the code
	// Informs compiler that value of variable can change unexpectedly and therefore optimizations should be limited to avoid potential issues
	volatile HANDLE fenceEvent;
	HANDLE threadHandle;
	UINT64 volatile m_threadFenceValue = 0;
	LONG volatile m_terminating = 0;

	ID3D12Device* device;

	friend class DirectXCommon;
};