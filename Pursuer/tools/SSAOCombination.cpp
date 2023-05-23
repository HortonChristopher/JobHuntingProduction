#include "SSAOCombination.h"

SSAOCombination::SSAOCombination()
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

	auto dev = DirectXCommon::GetInstance()->GetDevice();

	// Vertex buffer generation
	result = dev->CreateCommittedResource(
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
	result = dev->CreateCommittedResource(
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
}

void SSAOCombination::Draw()
{
	auto cmdList = DirectXCommon::GetInstance()->GetCommandList();

	// Set pipeline
	PipelineStatus::SetPipeline("SSAOCombine");

	// Descriptor heap array
	ID3D12DescriptorHeap* ppHeaps[] = { Textures::GetBasicDescHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// Index buffer set command
	// All heap heads are constant buffer
	cmdList->SetGraphicsRootDescriptorTable(1, Textures::GetGpuDescHandleSRV("mainTex" + std::to_string(DirectXCommon::GetInstance()->GetBbIndex())));
	cmdList->SetGraphicsRootDescriptorTable(2, Textures::GetGpuDescHandleSRV("SSAO" + std::to_string(DirectXCommon::GetInstance()->GetBbIndex())));
	cmdList->SetGraphicsRootDescriptorTable(3, Textures::GetGpuDescHandleSRV("normalTex" + std::to_string(DirectXCommon::GetInstance()->GetBbIndex())));
	cmdList->SetGraphicsRootDescriptorTable(4, Textures::GetGpuDescHandleSRV("depthTex" + std::to_string(DirectXCommon::GetInstance()->GetBbIndex())));
	cmdList->SetGraphicsRootDescriptorTable(5, Textures::GetGpuDescHandleSRV("halfNormalTex" + std::to_string(DirectXCommon::GetInstance()->GetBbIndex())));
	cmdList->SetGraphicsRootDescriptorTable(6, Textures::GetGpuDescHandleSRV("halfDepthTex" + std::to_string(DirectXCommon::GetInstance()->GetBbIndex())));

	cmdList->DrawInstanced(4, 1, 0, 0);
}