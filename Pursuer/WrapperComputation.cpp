#include "WrapperComputation.h"

// The code is defining a macro named InterlockedGetValue with a parameter object.
// The macro expands to the function call InterlockedCompareExchange(object, 0, 0), which is a function provided by the Windows API for performing an atomic compare-and-exchange operation. 
// The InterlockedCompareExchange function compares the value of object with 0, and if they are equal, it replaces the value of object with 0 and returns the original value. 
// This operation is performed atomically, ensuring that it is thread-safe and avoids race conditions.
// The purpose of this macro is to provide a convenient way to retrieve the value of a variable in a thread-safe manner using atomic operations.
#define InterlockedGetValue(object) InterlockedCompareExchange(object, 0, 0)

WrapperComputation* WrapperComputation::GetInstance()
{
    static WrapperComputation instance;

    return &instance;
}

void WrapperComputation::Initialize()
{
    device = DirectXCommon::GetInstance()->GetDevice();

    // Initialize command queue description with command list type, priority (0), and command queue flags (none)
    D3D12_COMMAND_QUEUE_DESC queueDesc = { D3D12_COMMAND_LIST_TYPE_COMPUTE, 0, D3D12_COMMAND_QUEUE_FLAG_NONE };

    // Creates command queue, with the resulting command queue stored in cmdQueue
    auto result = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
    assert(SUCCEEDED(result));

    // Creates command allocator, with resulting command allocator stored in cmdAllocator
    result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&cmdAllocator));
    assert(SUCCEEDED(result));

    // Creates command list, utilizing node mask (0), command list type, cmdAllocator object, initial pipeline state (nullptr), and address of cmdList object, then stored in cmdList
    result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
    assert(SUCCEEDED(result));

    // Creates fence, utilizing initial fence value (0), fence flag (none), ARGS macro, and fence address, with resulting fence stored in fence
    result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(result));

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    if (fenceEvent == nullptr)
    {
        assert(SUCCEEDED(result));
    }
}

void WrapperComputation::End()
{
}

void WrapperComputation::AsyncComputeThreadProc()
{
    // Extracts raw pointers (Get) from ComPtr smart pointers and assign them to corresponding raw pointer variables
    ID3D12CommandQueue* pCommandQueue = cmdQueue.Get();
    ID3D12CommandAllocator* pCommandAllocator = cmdAllocator.Get();
    ID3D12GraphicsCommandList* pCommandList = cmdList.Get();
    ID3D12Fence* pFence = fence.Get();

    // Run the particle simulation.
    if (shades.size() != 0)
    {
        shades[0]->Dispatch();
    }

    // Close and execute the command list.
    auto result = pCommandList->Close();
    assert(SUCCEEDED(result));

    // An array ppCommandLists is created with a single element pointing to the command list (pCommandList). 
    // The command queue (pCommandQueue) is then used to execute the command lists in the array.
    ID3D12CommandList* ppCommandLists[] = { pCommandList };
    pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

    // Wait for the compute shader to complete the simulation.
    UINT64 threadFenceValue = InterlockedIncrement(&m_threadFenceValue);
    // The command queue (pCommandQueue) is signaled with the fence and the incremented value.
    result = pCommandQueue->Signal(pFence, threadFenceValue);
    assert(SUCCEEDED(result));
    // The SetEventOnCompletion function is used to set fenceEvent to a signaled state when the fence reaches the specified value.
    result = pFence->SetEventOnCompletion(threadFenceValue, fenceEvent);
    assert(SUCCEEDED(result));

    // The execution is blocked until the fenceEvent is signaled. 
    // This ensures synchronization until the compute shader completes its simulation.
    WaitForSingleObject(fenceEvent, INFINITE);

    // Wait for the render thread to be done with the SRV so that
    // the next frame in the simulation can run.
    UINT64 renderContextFenceValue = InterlockedGetValue(&DirectXCommon::GetInstance()->GetCurrentFenceVal());

    //  If the completed value of the fence from the common DirectX context (DirectXCommon::GetInstance()->GetFence()) is less than the renderContextFenceValue,
    if (DirectXCommon::GetInstance()->GetFence()->GetCompletedValue() < renderContextFenceValue)
    {
        // The command queue is instructed to wait until the fence reaches the specified value.
        result = pCommandQueue->Wait(DirectXCommon::GetInstance()->GetFence().Get(), renderContextFenceValue);
        assert(SUCCEEDED(result));
    }

    // Prepare for the next frame.
    result = pCommandAllocator->Reset();
    assert(SUCCEEDED(result));

    // Command list is also reset
    result = pCommandList->Reset(pCommandAllocator, ShadingComputation::GetPipeline());
    assert(SUCCEEDED(result));
}