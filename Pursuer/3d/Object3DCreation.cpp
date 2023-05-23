#include "Object3DCreation.h"

Object3DCreation::Object3DCreation()
{

}

Object3DCreation::~Object3DCreation()
{
}

void Object3DCreation::CreatePyramid(const int vertex, const float topHeight, const int R, XMFLOAT3 scale, XMFLOAT3 rot, XMFLOAT3 trans, bool projective, XMFLOAT4 color, ID3D12Resource* texbuff, TexMetadata& metadata)
{
	HRESULT result;

	ID3D12Device* dev = DirectXCommon::GetInstance()->GetDevice();

	baseColor = color;
	basePosition = trans;
	this->vertex = vertex;
	this->radius = topHeight / 2.0f;

	for (int i = 0; i < vertex; i++)
	{
		vertices[i].pos.x = R * sinf((XM_2PI / (vertex - 1)) * i);
		vertices[i].pos.y = R * cosf((XM_2PI / (vertex - 1)) * i);
		vertices[i].pos.z = 0.0f;
	}

	vertices[vertex - 1].pos = { 0, 0, 0 };
	vertices[vertex].pos = { 0, 0, -topHeight };

	XMFLOAT2 uv = { 0,0 };

	for (int i = 0; i < vertex + 1; i++)
	{
		uv.x = (1.0f / vertex) * i;
		uv.y = (1.0f / vertex) * i;
		vertices[i].uv = uv;
	}

	for (int i = 0; i < (vertex - 1) * 2; i++)
	{
		int k = i * 3;

		if (i < vertex - 1)
		{
			if (i == vertex - 2)
			{
				indices[k] = 0;
				indices[k + 1] = vertex - 2;
			}
			else
			{
				indices[k] = i + 1;
				indices[k + 1] = i;
			}

			indices[k + 2] = vertex - 1;
		}
		else
		{
			indices[k] = i - vertex + 1;

			if (i == (vertex - 1) * 2 - 1)
			{
				indices[k + 1] = 0;
			}
			else
			{
				indices[k + 1] = indices[k] + 1;
			}

			indices[k + 2] = vertex;
		}
	}

	for (int i = 0; i < (vertex - 1) * 2; i++)
	{
		// Calculate each triangle one by one
		// Take the index of a triangle and put it into a temporary variable
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];

		// Assign the coordinates of the vertices that make up the triangle to a vector
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

		// Compute p0->p1 vector, p0->p2 vector (vector subtraction)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		// The outer product is a vector perpendicular from both previous vectors
		XMVECTOR normal = XMVector3Cross(v1, v2);

		// Normalize (to length 1)
		normal = XMVector3Normalize(normal);

		// Assign the obtained normals to the vertex data
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}

	matWorld = XMMatrixIdentity();
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matWorld *= matScale; // Reflects scaling in the world matrix
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationY(XMConvertToRadians(rot.y));
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rot.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rot.x));
	matWorld *= matRot; // Reflect rotation in world matrix
	matTrans = XMMatrixTranslation(trans.x, trans.y, trans.z);
	matWorld *= matTrans; // Reflects translations in the world matrix

	this->position = trans;
	this->rotation = rot;

	baseRot = rot;
	baseScale = matScale;

	D3D12_HEAP_PROPERTIES heapprop{}; // Vertex Heap Settings
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // For transfer to GPU

	D3D12_RESOURCE_DESC resdesc{}; // Resource Configuration
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices); // Size for vertex information
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// GPU resource generation
	result = dev->CreateCommittedResource(
		&heapprop, // Heap settings
		D3D12_HEAP_FLAG_NONE,
		&resdesc, // Resource Configuration
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	// Setup structure
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // Visible through shaders
	descHeapDesc.NumDescriptors = 3; // One SRV and two CBVs

	// Generation
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

	// Retrieve the top handle of the descriptor heap.
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_HEAP_PROPERTIES cbheapprop{}; // Heap settings
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // For transfer to GPU

	// Resource Configuration
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = window_width; // Match render target
	depthResDesc.Height = window_height; // Match render target
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth format
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // Depth stencil

	// Heap properties for depth values
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Clear depth value setting
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // Clear with a depth value of 1.0f (maximum value)
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; // Depth cue format

	// Resource Configuration
	result = dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // Used for writing depth values
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer));

	// Descriptor heap creation for depth view
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1; // One depth view
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // Depth Stencil View

	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	// Depth View Creation
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth cue format
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_RESOURCE_DESC cbresdesc{}; // Resource Configuration
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff; // 256-byte alignment
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// GPU resource generation
	result = dev->CreateCommittedResource(
		&cbheapprop, // Heap settings
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc, // Resource Configuration
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	result = constBuff->Map(0, nullptr, (void**)&constMap); // Mapping
	constMap->color = color; // Gray in RGBA

	// Perspective projection
	if (projective == true)
	{
		matProjection = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(60.0f),
			(float)window_width / window_height,
			0.1f, 1000.0f);
	}
	else
	{
		matProjection = XMMatrixOrthographicOffCenterLH(
			-100, 100, -60, 60, 0.1f, 1000.0f);
	}
	constMap->mat = matWorld * camera->GetViewMatrix() * matProjection;
	constBuff->Unmap(0, nullptr); // Unmapping

	// Creating a Constant Buffer View
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;

	dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
	// Shift the address by the amount consumed
	basicHeapHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Shader Resource View Creation
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // Setup structure
	srvDesc.Format = metadata.format;//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D Textures
	srvDesc.Texture2D.MipLevels = 1;

	dev->CreateShaderResourceView(texbuff, // Buffer to associate with view
		&srvDesc, // Texture setting information
		basicHeapHandle
	);

	// Allocation of index buffer
	resdesc.Width = sizeof(indices); // Size for index information

	// GPU resource generation
	result = dev->CreateCommittedResource(
		&heapprop, // Heap settings
		D3D12_HEAP_FLAG_NONE,
		&resdesc, // Resource Configuration
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	// Get virtual memory corresponding to buffers on above GPU
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	// For all indexes
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i]; // Copy index
	}
	indexBuff->Unmap(0, nullptr);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);

	// Record the first address (GPU) of the descriptor heap
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleStart = basicDescHeap->GetGPUDescriptorHandleForHeapStart();

	// Get the buffer size for one descriptor
	UINT descHandleIncrementSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// The first CBV
	gpuDescHandleCBV = gpuDescHandleStart;
	gpuDescHandleCBV.ptr += descHandleIncrementSize * 0;

	//SRV
	gpuDescHandleSRV = gpuDescHandleStart;
	gpuDescHandleSRV.ptr += descHandleIncrementSize * 1;
}

void Object3DCreation::CreateBox(const int vertex, const float TopHeight, const int Width, XMFLOAT3 scale, XMFLOAT3 rot, XMFLOAT3 trans, bool projective, XMFLOAT4 color, ID3D12Resource* texbuff, TexMetadata& metadata)
{
	HRESULT result;

	ID3D12Device* dev = DirectXCommon::GetInstance()->GetDevice();

	baseColor = color;
	this->vertex = vertex;
	this->width = Width;
	this->height = Width;
	this->radius = (float)Width;

	int radius = (int)Width / (int)sin(XM_2PI / vertex);

	for (int i = 0; i < vertex; i++)
	{
		vertices[i].pos.x = radius * sinf((XM_2PI / vertex) * i);
		vertices[i].pos.y = radius * cosf((XM_2PI / vertex) * i);
		vertices[i].pos.z = TopHeight * 0.5f;
	}

	vertices[vertex].pos = { 0,0,TopHeight * 0.5f };

	for (int i = vertex + 1; i < (vertex + 1) * 2 - 1; i++)
	{
		vertices[i].pos.x = radius * sinf((XM_2PI / vertex) * (i - vertex - 1));
		vertices[i].pos.y = radius * cosf((XM_2PI / vertex) * (i - vertex - 1));
		vertices[i].pos.z = -TopHeight * 0.5f;
	}

	vertices[(vertex + 1) * 2 - 1].pos = { 0,0,-TopHeight * 0.5f };

	XMFLOAT2 uv = { 0,0 };

	for (int i = 0; i < (vertex + 1) * 2; i++)
	{
		uv.x = (1.0f / vertex) * i;
		uv.y = (1.0f / vertex) * i;
		vertices[i].uv = uv;
	}

	int k = 0;

	for (int i = 0; i < vertex * 2; i++)
	{
		if (i <= vertex - 1)
		{
			if (i == vertex - 1)
			{
				indices[k] = 0;
				indices[k + 1] = vertex - 1;
			}
			else
			{
				indices[k] = i + 1;
				indices[k + 1] = i;
			}

			indices[k + 2] = vertex;
		}
		else if (i <= vertex * 2 - 1)
		{
			if (i == vertex * 2 - 1)
			{
				indices[k] = (vertex + 1) * 2 - 2;
				indices[k + 1] = vertex + 1;
			}
			else
			{
				indices[k] = i + 1;
				indices[k + 1] = i + 2;
			}

			indices[k + 2] = (vertex + 1) * 2 - 1;
		}

		k += 3;
	}

	for (int i = 0; i < vertex; i++)
	{
		if (i == vertex - 1)
		{
			indices[k] = 0;
			indices[k + 1] = vertex + 1;
			indices[k + 2] = vertex - 1;
			indices[k + 3] = vertex + 1;
			indices[k + 4] = (vertex + 1) * 2 - 2;
			indices[k + 5] = vertex - 1;
		}
		else
		{
			indices[k] = vertex + 1 + i;
			indices[k + 1] = i;
			indices[k + 2] = vertex + 2 + i;
			indices[k + 3] = i;
			indices[k + 4] = i + 1;
			indices[k + 5] = vertex + 2 + i;

			k += 6;
		}
	}

	for (int i = 0; i < vertex * 2 * 2; i++)
	{
		// Calculate each triangle one by one.
		// Take the index of a triangle and put it into a temporary variable
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];

		// Assign the coordinates of the vertices that make up the triangle to a vector
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

		// Compute p0->p1 vector, p0->p2 vector (vector subtraction)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		// The outer product is a vector perpendicular from both
		XMVECTOR normal = XMVector3Cross(v1, v2);

		// Normalize (to length 1)
		normal = XMVector3Normalize(normal);

		// Assign the obtained normals to the vertex data
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}

	matWorld = XMMatrixIdentity();
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matWorld *= matScale; // Reflects scaling in the world matrix
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationY(XMConvertToRadians(rot.y));
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rot.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rot.x));
	matWorld *= matRot; // Reflect rotation in world matrix
	matTrans = XMMatrixTranslation(trans.x, trans.y, trans.z);
	matWorld *= matTrans; // Reflects translations in the world matrix

	this->position = trans;
	this->rotation = rot;

	D3D12_HEAP_PROPERTIES heapprop{}; // Vertex Heap Settings
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // For transfer to GPU

	D3D12_RESOURCE_DESC resdesc{}; // Resource Configuration
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices); // Size for vertex information
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// GPU resource generation
	result = dev->CreateCommittedResource(
		&heapprop, // Heap settings
		D3D12_HEAP_FLAG_NONE,
		&resdesc, // Resource Configuration
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	// Setup structure
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // Visible through shaders
	descHeapDesc.NumDescriptors = 3; // One SRV and two CBVs

	// Generation
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

	// Retrieve the top handle of the descriptor heap.
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_HEAP_PROPERTIES cbheapprop{}; // Heap settings
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // For transfer to GPU

	// Resource Configuration
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = window_width; // Match render target
	depthResDesc.Height = window_height; // Match render target
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth format
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // Depth stencil

	// Heap properties for depth values
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Clear depth value setting
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // Clear with a depth value of 1.0f (maximum value)
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; // Depth cue format

	// Resource Configuration
	result = dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // Used for writing depth values
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer));

	// Descriptor heap creation for depth view
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1; // One depth view
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // Depth Stencil View

	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	// Depth View Creation
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth cue format
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_RESOURCE_DESC cbresdesc{}; // Resource Configuration
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff; // 256-byte alignment
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// GPU resource generation
	result = dev->CreateCommittedResource(
		&cbheapprop, // Heap settings
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc, // Resource Configuration
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	result = constBuff->Map(0, nullptr, (void**)&constMap); // Mapping
	constMap->color = color; // Gray in RGBA

	// Perspective projection
	if (projective == true)
	{
		matProjection = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(60.0f),
			(float)window_width / window_height,
			0.1f, 1000.0f);
	}
	else
	{
		matProjection = XMMatrixOrthographicOffCenterLH(
			-100, 100, -60, 60, 0.1f, 1000.0f);
	}

	constMap->mat = matWorld * camera->GetViewMatrix() * matProjection;
	constBuff->Unmap(0, nullptr); // Unmapping

	// Creating a Constant Buffer View
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;

	dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	// Shift the address by the amount consumed.
	basicHeapHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Shader Resource View Creation
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // Setup structure
	srvDesc.Format = metadata.format; // RGBA format
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D Textures
	srvDesc.Texture2D.MipLevels = 1;

	dev->CreateShaderResourceView(texbuff, // Buffer to associate with view
		&srvDesc, // Texture setting information
		basicHeapHandle
	);

	// Allocation of index buffer
	resdesc.Width = sizeof(indices); // Size for index information

	// GPU resource generation
	result = dev->CreateCommittedResource(
		&heapprop, // Heap settings
		D3D12_HEAP_FLAG_NONE,
		&resdesc, // Resource Configuration
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	// Get virtual memory corresponding to buffers on above GPU
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	// For all indexes
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i]; // Copy index
	}

	indexBuff->Unmap(0, nullptr);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);

	// Record the first address (GPU) of the descriptor heap
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleStart = basicDescHeap->GetGPUDescriptorHandleForHeapStart();

	// Get the buffer size for one descriptor
	UINT descHandleIncrementSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// The first CBV
	gpuDescHandleCBV = gpuDescHandleStart;
	gpuDescHandleCBV.ptr += descHandleIncrementSize * 0;

	// SRV
	gpuDescHandleSRV = gpuDescHandleStart;
	gpuDescHandleSRV.ptr += descHandleIncrementSize * 1;
}

void Object3DCreation::DrawPyramid(XMFLOAT4 color, ID3D12PipelineState* pipelinestate, ID3D12RootSignature* rootsignature)
{
	HRESULT result;

	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();

	baseColor = color;
	constMap->color = color;

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	// The first CBV
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];
	}

	// Unmap
	vertBuff->Unmap(0, nullptr);

	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetPipelineState(pipelinestate);
	cmdList->SetGraphicsRootSignature(rootsignature);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetIndexBuffer(&ibView);
	// Set constant buffer view command
	cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandleCBV);
	// Set shader resource view
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);

	cmdList->DrawIndexedInstanced((vertex - 1) * 6, 1, 0, 0, 0);
}

void Object3DCreation::DrawBox(XMFLOAT4 color, ID3D12PipelineState* pipelinestate, ID3D12RootSignature* rootsignature)
{
	HRESULT result;

	ID3D12GraphicsCommandList* cmdList = DirectXCommon::GetInstance()->GetCommandList();

	baseColor = color;
	constMap->color = color;

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	// For all vertices
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];
	}

	// Unmap
	vertBuff->Unmap(0, nullptr);

	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetPipelineState(pipelinestate);
	cmdList->SetGraphicsRootSignature(rootsignature);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetIndexBuffer(&ibView);
	// Set constant buffer view command
	cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandleCBV);
	// Set shader resource view
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);

	cmdList->DrawIndexedInstanced(vertex * 12, 1, 0, 0, 0);
}

void Object3DCreation::Move()
{
	matRot = XMMatrixIdentity();

	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));

	matTrans = XMMatrixTranslation(position.x, position.y, position.z); // Recalculate translation matrix from coordinates

	matWorld = XMMatrixIdentity(); // World matrix reset every frame

	matWorld *= matRot;
	matWorld *= matTrans;

	constMap->mat = matWorld * camera->GetViewMatrix() * matProjection;
}

void Object3DCreation::Update()
{
}

void Object3DCreation::SetDirection()
{
}

XMFLOAT3 Object3DCreation::GetPosition()
{
	return position;
}

void Object3DCreation::Damage(float damage)
{
	hp -= damage;

	if (hp <= 0.0f)
	{
		deathFlag = true;
	}

	damageFlag = true;
}

bool Object3DCreation::GetIsDead()
{
	return deathFlag;
}

void Object3DCreation::Die()
{
	deathFlag = true;
}

float Object3DCreation::GetRadius()
{
	return radius;
}

int Object3DCreation::GetWidth()
{
	return width;
}