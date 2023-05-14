#include "DirectXCommon.h"

DirectXCommon* DirectXCommon::GetInstance()
{
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize(GameWindow* window)
{
	// nullptr check
	assert(window);

	this->gameWindow = window;

	// DXGI device initialization
	if (CreateDXGI() != S_OK)
	{
		assert(0);
	}

	// Command-related initialization
	if (CreateCommands() != S_OK)
	{
		assert(0);
	}

	// Swap Chain Generation
	if (CreateSwapChain() != S_OK)
	{
		assert(0);
	}

	if (CreateDesc() != S_OK)
	{
		assert(0);
	}

	// Depth buffer generation
	if (CreateDSV() != S_OK)
	{
		assert(0);
	}

	// Fence generation
	if (CreateFence() != S_OK)
	{
		assert(0);
	}

	// imgui initialization
	if (ImguiInitialize() != S_OK)
	{
		assert(0);
	}
}

HRESULT DirectXCommon::CreateDXGI()
{
	HRESULT result;

	// DXGI Factory Generation
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	if (result != S_OK) 
	{
		return result;
	}

	// For adapter enumeration
	// An adapter object with a specific name is entered here.
	ComPtr < IDXGIAdapter> tmpAdapter;

	for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter); // Add to dynamic array
	}

	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC adesc{};
		adapters[i]->GetDesc(&adesc);  // Get adapter information
		std::wstring strDesc = adesc.Description;  // Adapter Name

		// Microsoft Basic Render Diver avoidance
		if (strDesc.find(L"Microsoft") == std::wstring::npos)
		{
			tmpAdapter = adapters[i];
			break;
		}
	}

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		// Generate device
		result = D3D12CreateDevice(nullptr, levels[i], IID_PPV_ARGS(&device));

		if (result == S_OK)
		{
			// Exit the loop when the device can be generated
			featureLevel = levels[i];
			break;
		}
	}

	return result;
}

HRESULT DirectXCommon::CreateCommands()
{
	HRESULT result;

	for (int i = 0; i < frameCount; i++)
	{
		// Create command allocator
		result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator[i]));

		if (result != S_OK)
		{
			return result;
		}
	}

	// Create command list
	result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator[bbIndex].Get(), nullptr, IID_PPV_ARGS(&commandList));
	if (result != S_OK)
	{
		return result;
	}

	// Generate command queue in standard configuration
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

	return result;
}

HRESULT DirectXCommon::CreateSwapChain()
{
	HRESULT result;

	// Generate swap chain with various settings
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = gameWindow->GetWindowWidth();
	swapchainDesc.Height = gameWindow->GetWindowHeight();
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // Color Information Format
	swapchainDesc.SampleDesc.Count = 1; // No multisampling
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // For back buffer
	swapchainDesc.BufferCount = frameCount; // Set the number of buffers to frame count
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Discard after flip
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain1> swapchain1;
	result = dxgiFactory->CreateSwapChainForHwnd(cmdQueue.Get(), gameWindow->GetHwnd(), &swapchainDesc, nullptr, nullptr, (IDXGISwapChain1**)&swapchain1);

	assert(SUCCEEDED(result));

	swapchain1.As(&swapchain);

	bbIndex = swapchain->GetCurrentBackBufferIndex();

	return result;
}

HRESULT DirectXCommon::CreateDesc()
{
	HRESULT result;

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// Generate descriptor heap with various settings
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // Render Target View
	heapDesc.NumDescriptors = swcDesc.BufferCount; // Three buffers

	result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	if (result != S_OK)
	{
		return result;
	}

	// For three buffers
	backBuffers.resize(swcDesc.BufferCount);
	for (int i = 0; i < backBuffers.size(); i++)
	{
		// Get buffer from swap chain
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));

		if (result != S_OK)
		{
			return result;
		}

		// Get a handle on the descriptor heap
		D3D12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
			i,
			device->GetDescriptorHandleIncrementSize(heapDesc.Type));

		// Creating a render target view
		device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, handle);
	}

	return result;
}

HRESULT DirectXCommon::CreateFence()
{
	HRESULT result;

	// Generating fences
	for (int i = 0; i < frameCount; i++)
	{
		fence[i] = nullptr;
		fenceVal[i] = 0;

		result = device ->CreateFence(fenceVal[bbIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence[i]));
	}

	return result;
}

HRESULT DirectXCommon::CreateDSV()
{
	HRESULT result;

	// Resource Configuration
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		gameWindow->GetWindowWidth(),
		gameWindow->GetWindowHeight(),
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	// Resource Configuration
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // Upload Not Available
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // Used for writing depth values
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer));

	if (result != S_OK)
	{
		return result;
	}

	// Descriptor heap creation for depth view
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1; // One depth view
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // Depth Stencil View

	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	if (result != S_OK)
	{
		return result;
	}

	// Depth View Creation
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth cue format
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

HRESULT DirectXCommon::ImguiInitialize()
{
	HRESULT result = S_FALSE;

	// Generate descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&imguiHeap));

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// Get information about the swap chain
	DXGI_SWAP_CHAIN_DESC swcDesc = {};

	result = swapchain->GetDesc(&swcDesc);

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	if (ImGui::CreateContext() == nullptr) {
		assert(0);
		return result;
	}
	
	if (!ImGui_ImplWin32_Init(gameWindow->GetHwnd())) {
		assert(0);
		return result;
	}

	if (!ImGui_ImplDX12_Init(GetDevice(), swcDesc.BufferCount, swcDesc.BufferDesc.Format, imguiHeap.Get(), imguiHeap->GetCPUDescriptorHandleForHeapStart(), imguiHeap->GetGPUDescriptorHandleForHeapStart())) 
	{
		assert(0);
		return result;
	}

	return result;
}

void DirectXCommon::ImguiDraw()
{
	// imgui Initialization
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void DirectXCommon::PreDraw()
{
	// Get a handle on the descriptor heap for the depth stencil view
	commandList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, (float)gameWindow->GetWindowWidth(), (float)gameWindow->GetWindowHeight()));
	commandList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, gameWindow->GetWindowWidth(), gameWindow->GetWindowHeight()));

	// Change resource barriers
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Get a handle on the descriptor heap for the render target view
	rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	// Full screen clear (RGBA)
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	commandList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXCommon::DepthClear()
{
	dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();

	commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXCommon::PostDraw()
{
	// imgui drawing
	ImGui::Render();

	ID3D12DescriptorHeap* ppHeaps[] = { imguiHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

	// Returning the Resource Barrier
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Close instruction
	commandList->Close();

	// Execute command list
	ID3D12CommandList* cmdLists[] = { commandList.Get() }; // Array of command lists
	cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// Flip buffer (flip the reverse side of the buffer)
	swapchain->Present(1, 0);

	// Set fence value in GPU
	const UINT64 currentFenceValue = fenceVal[bbIndex];
	cmdQueue->Signal(fence[bbIndex].Get(), currentFenceValue);

	// Get back buffer number (0 to 2 since there are three buffers total)
	bbIndex = swapchain->GetCurrentBackBufferIndex();

	// Compare fence values to check if GPU execution is complete
	const auto nextVal = fence[bbIndex]->GetCompletedValue();

	if (nextVal < fenceVal[bbIndex])
	{
		// Wait for execution completion
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence[bbIndex]->SetEventOnCompletion(fenceVal[bbIndex], event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	cmdAllocator[bbIndex]->Reset(); // Clear queue

	commandList->Reset(cmdAllocator[bbIndex].Get(), nullptr); // Prepare to store the command list again

	fenceVal[bbIndex]++;

	// DirectX every frame processing ends here
}

void DirectXCommon::Finalize()
{
	for (int i = 0; i < frameCount; i++)
	{
		cmdQueue->Signal(fence[bbIndex].Get(), fenceVal[i]);

		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence[bbIndex]->SetEventOnCompletion(fenceVal[i], event);
		WaitForSingleObjectEx(event, INFINITE, FALSE);

		CloseHandle(event);
	}

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

void DirectXCommon::ReRenderTarget()
{
	rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();

	commandList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, (float)gameWindow->GetWindowWidth(), (float)gameWindow->GetWindowHeight()));
	commandList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, gameWindow->GetWindowWidth(), gameWindow->GetWindowHeight()));
	commandList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
}