#pragma once

// Wrapper or abstraction layer around the compute functionality provided by the underlying graphics API, such as DirectX
// Compute shaders are a type of shader specifically designed for performing general-purpose computations on the GPU. They are commonly used in game development for tasks that can be parallelized, such as physics simulations, AI calculations, or complex rendering effects.
// The purpose of a ComputeWrapper class is to encapsulate the functionality related to compute shaders and provide a simplified interface for game developers to utilize compute capabilities in their game engines or applications. 
// It may provide methods for initializing the compute pipeline, setting up resources (buffers, textures) for compute operations, dispatching compute shader work, and managing synchronization between the CPU and GPU.
// By providing a wrapper around the low-level compute functionality, the ComputeWrapper class abstracts away the complexities of the underlying graphics API and provides a higher-level interface for game programmers to utilize compute shaders effectively.
// This allows game developers to focus more on the logic and algorithms of their compute tasks, rather than dealing with the intricacies of the graphics API.

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