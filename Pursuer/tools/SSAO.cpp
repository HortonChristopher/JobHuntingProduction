#include "SSAO.h"

Camera* SSAO::camera = nullptr;

SSAO::SSAO()
{
	HRESULT result;

	vertices[0].uv = { 0.0f,1.0f };
	vertices[1].uv = { 0.0f,0.0f };
	vertices[2].uv = { 1.0f,1.0f };
	vertices[3].uv = { 1.0f,0.0f };
	vertices[0].pos = { -1.0f,-1.0f,0.0f };
	vertices[1].pos = { -1.0f,+1.0f,0.0f };
	vertices[2].pos = { +1.0f,-1.0f,0.0f };
	vertices[3].pos = { +1.0f,+1.0f,0.0f };

	auto device = DirectXCommon::GetInstance()->GetDevice();

	// Vertex buffer generation
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	// Transfer vertex buffer
	VERTEX* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	if (SUCCEEDED(result))
	{
		memcpy(vertMap, vertices.data(), sizeof(vertices));

		vertBuff->Unmap(0, nullptr);
	}

	// Creating a vertex buffer view
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	// Creating a constant buffer
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffer) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	ConstBuffer* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

	constMap->mat = XMMatrixIdentity();
	constMap->color = { 1,1,1,1 };
	constBuff->Unmap(0, nullptr);

	XMFLOAT4 normals[count];

	for (int i = 0; i < count; i++)
	{
		Vector3 randNormal = Vector3{ (float)rand() - rand(),(float)rand() - rand(),(float)rand() - rand() }.Normalize();
		normals[i] = { randNormal.x,randNormal.y,randNormal.z,0 };

	}

	for (int i = 0; i < 3; i++)
	{
		// Creating a constant buffer
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffer2) + 0xff) & ~0xff),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&constBuff2[i]));
		assert(SUCCEEDED(result));

		ConstBuffer2* constMap2 = nullptr;
		result = constBuff2[i]->Map(0, nullptr, (void**)&constMap2);
		assert(SUCCEEDED(result));

		memcpy(constMap2->randNormal, normals, sizeof(XMFLOAT4) * count);

		constBuff2[i]->Unmap(0, nullptr);
	}
}

void SSAO::Draw()
{
	auto cmdList = DirectXCommon::GetInstance()->GetCommandList();

	SetCameraBuffer();

	// Set pipeline
	PipelineStatus::SetPipeline("SSAO");

	// Descriptor heap array
	ID3D12DescriptorHeap* ppHeaps[] = { Textures::GetBasicDescHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootConstantBufferView(1, constBuff2[bbindex]->GetGPUVirtualAddress());

	// Set index buffer command
	cmdList->SetGraphicsRootDescriptorTable(2, Textures::GetGpuDescHandleSRV("halfNormalTex" + std::to_string(DirectXCommon::GetInstance()->GetBbIndex()))); // Heap head is constant buffer
	cmdList->SetGraphicsRootDescriptorTable(3, Textures::GetGpuDescHandleSRV("halfDepthTex" + std::to_string(DirectXCommon::GetInstance()->GetBbIndex()))); // Heap head is constant buffer

	cmdList->DrawInstanced(4, 1, 0, 0);
}

void SSAO::SetCameraBuffer()
{
	bbindex = DirectXCommon::GetInstance()->GetBbIndex();

	ConstBuffer2* constMap = nullptr;
	auto result = constBuff2[bbindex]->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

	constMap->view = camera->GetViewMatrix();
	constMap->proj = camera->GetProjectionMatrix();
	constMap->invproj = XMMatrixInverse(nullptr, camera->GetViewMatrixProjection());

	constBuff2[bbindex]->Unmap(0, nullptr);
}