#include "Mesh.h"

void Mesh::CreateBuffers()
{
	HRESULT result;

	auto dev = DirectXCommon::GetInstance()->GetDevice();

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices.size());

	// Vertex buffer generation
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	// Data transfer to vertex buffer
	VertexPosNormalUv* vertMap = nullptr;

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	// Creating a vertex buffer view
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	if (FAILED(result)) {
		assert(0);
		return;
	}

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	// Index buffer generation
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	if (FAILED(result)) {
		assert(0);
		return;
	}

	// Data transfer to index buffer
	unsigned short* indexMap = nullptr;

	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	if (SUCCEEDED(result)) {
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	// Create index buffer view
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}

void Mesh::Draw()
{
	auto cmd = DirectXCommon::GetInstance()->GetCommandList();

	cmd->IASetVertexBuffers(0, 1, &vbView);
	// Set index buffer
	cmd->IASetIndexBuffer(&ibView);

	ID3D12DescriptorHeap* ppHeaps[] = { Textures::GetBasicDescHeap().Get() };

	cmd->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	if (!Object3d::GetDrawShadow())
	{
		// Set shader resource view
		cmd->SetGraphicsRootDescriptorTable(2, Textures::GetGpuDescHandleSRV(material->textureFileName));

		// Set material constant buffer
		ID3D12Resource* constBuff = material->GetConstBuffer();

		cmd->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());
	}
	// Drawing commands
	cmd->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}

void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData.begin();

	for (; itr != smoothData.end(); ++itr)
	{
		// Common vertex collection for each face
		std::vector<unsigned short>& v = itr->second;

		// Averages normals over all vertices
		XMVECTOR normal = {};

		for (unsigned short index : v)
		{
			normal += XMVectorSet(vertices[index].normal.x, vertices[index].normal.y, vertices[index].normal.z, 0);
		}

		normal = XMVector3Normalize(normal / (float)v.size());

		// Write to all vertex data using a common normal
		for (unsigned short index : v)
		{
			vertices[index].normal = { normal.m128_f32[0],normal.m128_f32[1],normal.m128_f32[2] };
		}
	}
}