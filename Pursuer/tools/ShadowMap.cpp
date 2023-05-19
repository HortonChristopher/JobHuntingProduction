#include "ShadowMap.h"

GameWindow* ShadowMap::gameWindow = nullptr;

ShadowMap::ShadowMap()
{
	HRESULT result;

	device = DirectXCommon::GetInstance()->GetDevice();
	cmdList = DirectXCommon::GetInstance()->GetCommandList();

	vertices[0].uv = { 0.0f, 1.0f };
	vertices[1].uv = { 0.0f, 0.0f };
	vertices[2].uv = { 1.0f, 1.0f };
	vertices[3].uv = { 1.0f, 0.0f };
	vertices[0].pos = { -1.0f, -1.0f, 0.0f };
	vertices[1].pos = { -1.0f, +1.0f, 0.0f };
	vertices[2].pos = { +1.0f, -1.0f, 0.0f };
	vertices[3].pos = { +1.0f, +1.0f, 0.0f };

	// Vertex Buffer Generation
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	// Transfer Vertex Buffer
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		memcpy(vertMap, vertices.data(), sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	// Create Vertex Buffer View
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	// Create Constant Buffer
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	Initialize();
}

void ShadowMap::Initialize()
{
	HRESULT result;

	auto heapDesc = DirectXCommon::GetInstance()->GetRTVHeap()->GetDesc();

	// Use information about the back buffer you are using
	auto& bbuff = DirectXCommon::GetInstance()->GetBackBuffers()[0];
	auto resDesc = bbuff->GetDesc();
	resDesc.Format = DXGI_FORMAT_R32_FLOAT;
	D3D12_HEAP_PROPERTIES heapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	resDesc.Height = resourceHeight;
	resDesc.Width = resourceWidth;

	// Same value as clear value when rendering
	float clsClr[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R32_FLOAT, clsClr);

	// Resource Creation
	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(shadowResource.ReleaseAndGetAddressOf()));
	if (result != S_OK)
		assert(0);

	// Create a heap for RTV
	heapDesc.NumDescriptors = 1;
	result = device->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(rtvHeap.ReleaseAndGetAddressOf()));
	if (result != S_OK)
		assert(0);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R32_FLOAT;

	// Create Render Target View
	device->CreateRenderTargetView(
		shadowResource.Get(),
		&rtvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtvHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));

	// Create heap for SRV
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;


	result = device->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(srvHeap.ReleaseAndGetAddressOf()));
	if (result != S_OK)
		assert(0);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// Shader resource view creation
	device->CreateShaderResourceView(
		shadowResource.Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			srvHeap->GetCPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
	);

	// Resource Settings
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		resourceWidth,
		resourceHeight,
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	// Resource Settings
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // Uploading not allowed
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // Used for writing depth values
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer));
	assert(SUCCEEDED(result));

	// Depth view descriptor heap creation
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1; // One depth view
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // Depth Stencil View
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	assert(SUCCEEDED(result));

	// Depth view creation
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth Value Format
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// Shader resource view creation
	Textures::AddTexture("shadowMap", shadowResource.Get());
}

void ShadowMap::PreDraw()
{
	// Change Resource Barrier
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowResource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET));

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer;
	rtvHeapPointer = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvHeap->GetCPUDescriptorHandleForHeapStart(), 0,
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	);

	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;

	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (float)gameWindow->GetWindowWidth(), (float)gameWindow->GetWindowHeight());
	scissorRect = CD3DX12_RECT(0, 0, gameWindow->GetWindowWidth(), gameWindow->GetWindowHeight());

	auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvHeapPointer, true, &dsvH);
	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &scissorRect);

	// Clear full screen (RGBA format)
	float clearColor[] = { 0.0f, 1.0f, 1.0f, 1.0f };

	cmdList->ClearRenderTargetView(rtvHeapPointer, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void ShadowMap::Draw()
{
	const DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };

	HRESULT result;

	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (result != S_OK)
	{
		assert(0);
	}

	constMap->mat = DirectX::XMMatrixIdentity();
	constMap->color = color;
	constBuff->Unmap(0, nullptr);

	// Set Pipeline
	PipelineStatus::SetPipeline("ShdaowMap");

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Array of descriptor heaps
	ID3D12DescriptorHeap* ppHeaps[] = { Textures::GetBasicDescriptorHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// Index buffer set command
	cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(Textures::GetBasicDescriptorHeap().Get()->GetGPUDescriptorHandleForHeapStart(), Textures::GetTextureIndex("shadowMap"),
		device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	cmdList->DrawInstanced(4, 1, 0, 0);
}

void ShadowMap::PostDraw()
{
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}