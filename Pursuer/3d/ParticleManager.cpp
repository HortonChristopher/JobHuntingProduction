#include "ParticleManager.h"

ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager instance;

	return &instance;
}

void ParticleManager::Add(Particle* newParticle, const std::string& TexName)
{
	parameterData[TexName].push_back(newParticle->parameter);

	vertData[TexName].push_back(newParticle->parameters);

	activeParticleCount++;
}

void ParticleManager::Initialize()
{
	CreateConstantBuffer();

	CreateModel();

	shadingComputation = std::make_unique<ShadingComputation>(vertexCount);

	WrapperComputation::GetInstance()->AddShade(shadingComputation.get());
}

void ParticleManager::Update()
{
	HRESULT result;

	// Transfers data to compute shaders
	parameters.clear();
	parameters.resize(activeParticleCount);

	int paramOffset = 0;

	for (auto it : parameterData)
	{
		const int size = it.second.size();

		if (size <= 0)
		{
			continue;
		}

		memcpy(parameters.data() + paramOffset, it.second.data(), sizeof(ParticleParameters) * size);

		paramOffset += size;
	}

	vert.clear();
	vert.resize(activeParticleCount);

	int vertOffset = 0;

	for (auto it : vertData)
	{
		const int size = it.second.size();

		if (size <= 0)
		{
			continue;
		}

		memcpy(vert.data() + vertOffset, it.second.data(), sizeof(OutputData) * size);

		vertOffset += size;
	}

	shadingComputation->Update(parameters.data(), vert.data(), activeParticleCount);

	// Data transfer to constant buffer
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

	constMap->mat = camera->GetViewMatrixProjection();
	constMap->matBillboard = camera->GetBillboardMatrix();

	constBuff->Unmap(0, nullptr);
}

void ParticleManager::Draw()
{
	auto cmdList = DirectXCommon::GetInstance()->GetCommandList();

	vert.clear();
	vert.resize(activeParticleCount);

	memcpy(vert.data(), shadingComputation->GetOutputData(), sizeof(OutputData) * activeParticleCount);

	int offset = 0;

	for (auto it = vertData.begin(); it != vertData.end(); ++it)
	{
		const int size = it->second.size();

		if (size <= 0)
		{
			continue;
		}

		memcpy(it->second.data(), vert.data() + offset, sizeof(OutputData) * size);

		offset += size;
	}

	for (auto itr = vertData.begin(); itr != vertData.end(); ++itr)
	{
		for (auto it = itr->second.begin(); it != itr->second.end();)
		{
			if ((*it).isDead == 0)
			{
				++it;
				continue;
			}

			it = itr->second.erase(it);

			parameterData[itr->first].erase(parameterData[itr->first].begin() + (int)std::distance(itr->second.begin(), it));

			if (activeParticleCount > 0)
			{
				activeParticleCount--;
			}

			if (itr->second.size() == 0)
			{
				itr->second.clear();
				parameterData[itr->first].clear();
				break;
			}
		}

		if (activeParticleCount <= 0)
		{
			break;
		}
	}

	vert.clear();
	vert.resize(activeParticleCount);

	int vertOffset = 0;

	for (auto it = vertData.begin(); it != vertData.end(); ++it)
	{
		const int size = it->second.size();

		if (size <= 0)
		{
			continue;
		}

		memcpy(vert.data() + vertOffset, it->second.data(), sizeof(OutputData) * size);

		vertOffset += size;
	}

	const auto outputPtr = vert.data();

	OutputData* vertMap = nullptr;

	auto result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	if (SUCCEEDED(result)) {
		memcpy(vertMap, outputPtr, sizeof(OutputData) * activeParticleCount);
		vertBuff->Unmap(0, nullptr);
	}

	PipelineStatus::SetPipeline("Particle", ADD);

	// Vertex buffer settings
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	// Descriptor heap array
	ID3D12DescriptorHeap* ppHeaps[] = { Textures::GetBasicDescHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// Set constant buffer view
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	int count = 0;

	for (auto it : vertData)
	{
		// If no particles are present
		if (it.second.empty())
		{
			continue;
		}
		UINT drawNum = it.second.size();

		if (drawNum > vertexCount) 
		{
			drawNum = vertexCount;
		}

		// Set shader resource view
		cmdList->SetGraphicsRootDescriptorTable(1, Textures::GetGpuDescHandleSRV(it.first));
		// Drawing commands
		cmdList->DrawInstanced(drawNum, 1, count, 0);

		count += drawNum;
	}
}

void ParticleManager::CreateConstantBuffer()
{
	HRESULT result;

	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // Uploadable 
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	if (FAILED(result)) 
	{
		assert(0);
	}
}

void ParticleManager::CreateModel()
{
	HRESULT result = S_FALSE;

	auto device = DirectXCommon::GetInstance()->GetDevice();

	// Vertex buffer generation
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(OutputData) * vertexCount),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	if (FAILED(result)) {
		assert(0);
		return;
	}

	// Creating a vertex buffer view
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(OutputData) * vertexCount;
	vbView.StrideInBytes = sizeof(OutputData);
}

void ParticleManager::ShutDown()
{

}