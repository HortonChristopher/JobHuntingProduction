#include "TextureResource.h"

std::vector<TextureResource*> TextureResource::nowRenderTargets = {};

int TextureResource::bbindex = 0;

TextureResource::TextureResource(const std::string& name, const bool noDepth) : resourceWidth(1920), resourceHeight(1080), 
	format(DXGI_FORMAT_R8G8B8A8_UNORM), clearColor{ 1,1,1,1 }, noDepth(noDepth)
{
	Initialize(name);
}

TextureResource::TextureResource(const std::string& name, const Vector2& size, const DXGI_FORMAT resourceFormat, 
	const DirectX::XMFLOAT4& arg_clearColor, const bool noDepth)
	: resourceWidth(size.x), resourceHeight(size.y), format(resourceFormat), 
	clearColor{ arg_clearColor.x,arg_clearColor.y, arg_clearColor.z, arg_clearColor.w }, noDepth(noDepth)
{
	Initialize(name);
}

void TextureResource::Initialize(const std::string& name)
{
	device = DirectXCommon::GetInstance()->GetDevice();
	cmdList = DirectXCommon::GetInstance()->GetCommandList();

	auto heapDesc = DirectXCommon::GetInstance()->GetRtvHeaps()->GetDesc();

	// Use the information from the back buffer currently being used
	auto& bbuff = DirectXCommon::GetInstance()->GetBackBuffers()[0]; // Get primary back buffer
	auto resDesc = bbuff->GetDesc(); // Get resource description in back buffer object
	resDesc.Format = format;

	// Defines properties of the heap where committed resource will be created
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	resDesc.Height = resourceHeight;
	resDesc.Width = resourceWidth;

	// Same value as the clear value at rendering
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(format, clearColor.data());

	HRESULT result;

	result = device->CreateCommittedResource(
		&heapProp, // Heap properties
		D3D12_HEAP_FLAG_NONE, // Resource flags
		&resDesc, // Resource description
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // Initial resource state
		&clearValue, // Clear value
		IID_PPV_ARGS(resource[0].ReleaseAndGetAddressOf())); // Retrieves created resource

	if (result != S_OK)
	{
		assert(0);
	}

	// Create a heap for RTV
	heapDesc.NumDescriptors = 1;

	result = device->CreateDescriptorHeap(
		&heapDesc, // Descriptor heap description
		IID_PPV_ARGS(peraRTVHeap.ReleaseAndGetAddressOf())); // Retrieves created heap

	if (result != S_OK)
	{
		assert(0);
	}

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = format;

	device->CreateRenderTargetView(
		resource[0].Get(), // Resource to bind render target view to
		&rtvDesc, // Render target view description
		CD3DX12_CPU_DESCRIPTOR_HANDLE(peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV))); // CPU descriptor handle for RTV heap

	// Create a heap for SRV
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = device->CreateDescriptorHeap(
		&heapDesc, // Descriptor heap description
		IID_PPV_ARGS(peraSRVHeap.ReleaseAndGetAddressOf())); // Retrieves created heap

	if (result != S_OK)
	{
		assert(0);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // Default mapping

	device->CreateShaderResourceView(
		resource[0].Get(), // Resource to bind SRV to
		&srvDesc, // SRV description
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraSRVHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))); // CPU descriptor handle for SRV heap

	if (!noDepth)
	{
		// Resource Configuration
		CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R32_TYPELESS,
			resourceWidth,
			resourceHeight,
			1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		// Resource Configuration
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // Upload possible
			D3D12_HEAP_FLAG_NONE,
			&depthResDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, // Used for writing depth values
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
			IID_PPV_ARGS(&depthBuffer));
		assert(SUCCEEDED(result));

		// Descriptor heap creation for depth view
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		dsvHeapDesc.NumDescriptors = 1; // One depth view
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // Depth Stencil View
		result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
		assert(SUCCEEDED(result));

		// Depth View Creation
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth cue format
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		device->CreateDepthStencilView(
			depthBuffer.Get(),
			&dsvDesc,
			dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	Textures::AddTexture(name, resource[0].Get());
}

void TextureResource::PreDraw(const UINT arg_numRTD, const float topLeftX, const float topLeftY, const float width, const float height,
	const LONG& left, const LONG& top, const LONG& right, const LONG& bottom)
{
	numRTD = arg_numRTD;
	int bbIndex = 0;

	nowRenderTargets.push_back(this);

	// Resource Barrier Change
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	if (numRTD == 1)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer;
		rtvHeapPointer = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), bbIndex,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);
		D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
		CD3DX12_VIEWPORT viewport;
		CD3DX12_RECT scissorRect;

		viewport = CD3DX12_VIEWPORT(topLeftX, topLeftY, width, height);
		scissorRect = CD3DX12_RECT(left, top, right, bottom);
		dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		cmdList->OMSetRenderTargets(1, &rtvHeapPointer, true, &dsvH);
		cmdList->RSSetViewports(1, &viewport);
		cmdList->RSSetScissorRects(1, &scissorRect);

		cmdList->ClearRenderTargetView(rtvHeapPointer, clearColor.data(), 0, nullptr);

		cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}
	else
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHeapPointer;
		rtvHeapPointer.resize(arg_numRTD);

		auto it = nowRenderTargets.end();
		it--;

		for (int i = arg_numRTD - 1; i >= 0; i--)
		{
			rtvHeapPointer[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
				(*it)->peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), bbIndex,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
			if (it == nowRenderTargets.begin())
				break;
			it--;
		}

		std::vector <CD3DX12_VIEWPORT> viewports;
		std::vector <CD3DX12_RECT> scissorRects;
		viewports.resize(arg_numRTD);
		scissorRects.resize(arg_numRTD);

		for (int i = 0; i < arg_numRTD; i++)
		{
			viewports[i] = CD3DX12_VIEWPORT(topLeftX, topLeftY, width, height);
			scissorRects[i] = CD3DX12_RECT(left, top, right, bottom);
		}

		auto dsvH = (*it)->dsvHeap->GetCPUDescriptorHandleForHeapStart();

		cmdList->OMSetRenderTargets(arg_numRTD, rtvHeapPointer.data(), false, &dsvH);
		cmdList->RSSetViewports(arg_numRTD, viewports.data());
		cmdList->RSSetScissorRects(arg_numRTD, scissorRects.data());
		cmdList->ClearRenderTargetView(rtvHeapPointer[arg_numRTD - 1], clearColor.data(), 0, nullptr);
	}
}

void TextureResource::DepthClear()
{
	auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}


void TextureResource::ResetRenderTarget()
{
	auto rtvH =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(peraRTVHeap->GetCPUDescriptorHandleForHeapStart(), 0, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();

	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, resourceWidth, resourceHeight));
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, resourceWidth, resourceHeight));
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
}

void TextureResource::PostDraw(const bool renderTargetReset)
{
	int bbIndex = 0;

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	nowRenderTargets.pop_back();

	if (!renderTargetReset)
	{
		return;
	}

	if (nowRenderTargets.size() == 0)
	{
		DirectXCommon::GetInstance()->ReRenderTarget();
	}
	else
	{
		auto it = nowRenderTargets.end();
		it--;
		(*it)->ResetRenderTarget();
	}
}