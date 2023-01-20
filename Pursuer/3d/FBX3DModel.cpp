#include "FBX3DModel.h"

FBX3DModel::~FBX3DModel()
{
	// Release FBX scene
	fbxScene->Destroy();
}

void FBX3DModel::CreateBuffers(ID3D12Device* device)
{
	HRESULT result;

	// Overall size of vertex data
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUvSkin) * vertices.size());

	// Vertex buffer generation
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	// Data transfer to vertex buffer
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	// Create vertex buffer view
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	// Overall size of Index Buffer
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	// Create Index Buffer
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	// Data transfer to index buffer
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result))
	{
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	// Create Index Buffer View
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	// Texture Image Data
	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0); // Raw data extraction
	assert(img);

	// Resource settings
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	// Setting Texture Buffer
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // Texture specifications
		nullptr,
		IID_PPV_ARGS(&texBuff));

	// Transfer Data to Texture Buffer
	result = texBuff->WriteToSubresource(
		0,
		nullptr, // copy to all areas
		img->pixels, // Original teledata address
		(UINT)img->rowPitch, // 1 line size
		(UINT)img->slicePitch // 1 sheet size
	);

	// SRV descriptor heap creation
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // As visible from the shader
	descHeapDesc.NumDescriptors = 1; // Number of textures
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeapSRV)); // Creation

	// Shader Resource View Creation
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D Texture
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texBuff.Get(), // Buffer associated with view
		&srvDesc, // Texture setting information
		descHeapSRV->GetCPUDescriptorHandleForHeapStart() // Heap destination address
	);
}

void FBX3DModel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// Set vertex buffer (VBV)
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	// Set index buffer (IBV)
	cmdList->IASetIndexBuffer(&ibView);

	// Set descriptor heap
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// Shader Resource View set
	cmdList->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());

	// Draw command
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}

void FBX3DModel::AddAnimation(const std::string& animationName, const int startFrame, const int endFrame)
{
	AnimationTime time;
	time.startTime.SetTime(0, 0, 0, startFrame, 0, FbxTime::EMode::eFrames60);
	time.endTime.SetTime(0, 0, 0, endFrame, 0, FbxTime::EMode::eFrames60);
	animations[animationName] = time;
}

const FBX3DModel::AnimationTime& FBX3DModel::GetAnimation(const std::string& animationName)
{
	return animations[animationName];
}

void FBX3DModel::AnimationInit()
{
	// Get animation number 0
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	// Get animation name
	const char* animstackname = animstack->GetName();
	// Get animation time
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);
	// Get starting time
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	// Get ending time
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	// Set currentTime to startTime
	currentTime = startTime;
	// Set playing state to true
	isPlay = true;
}

void FBX3DModel::SetAnimationFrame(const int startFrame, const int endFrame, const int FrameTime)
{
	startTime.SetTime(0, 0, 0, startFrame, 0, FbxTime::EMode::eFrames60);
	currentTime = startTime;
	endTime.SetTime(0, 0, 0, endFrame, 0, FbxTime::EMode::eFrames60);
	if (startFrame > endFrame)
		frameTime.SetTime(0, 0, 0, -FrameTime, 0, FbxTime::EMode::eFrames60);
	else
		frameTime.SetTime(0, 0, 0, FrameTime, 0, FbxTime::EMode::eFrames60);
	isPlay = true;
}

void FBX3DModel::SetAnimation(const std::string& animationName, const int FrameTime)
{
	isPlay = true;

	// If animation name isn't registered, set to frame 0
	if (animations.count(animationName) == 0)
	{
		startTime.SetTime(0, 0, 0, 0, 0, FbxTime::EMode::eFrames60);
		endTime.SetTime(0, 0, 0, 0, 0, FbxTime::EMode::eFrames60);
		frameTime.SetTime(0, 0, 0, 0, 0, FbxTime::EMode::eFrames60);
		return;
	}
	nowAnimationName = animationName;
	startTime = animations[animationName].startTime;
	endTime = animations[animationName].endTime;
	currentTime = startTime;

	if (startTime > endTime)
		frameTime.SetTime(0, 0, 0, -FrameTime, 0, FbxTime::EMode::eFrames60);
	else
		frameTime.SetTime(0, 0, 0, FrameTime, 0, FbxTime::EMode::eFrames60);
}

bool FBX3DModel::PlayAnimation(bool endless)
{
	if (!isPlay)
		return false;

	currentTime += frameTime;
	if ((currentTime >= endTime && frameTime > 0) || (currentTime <= endTime && frameTime < 0))
	{
		if (!endless)
		{
			currentTime = endTime;
			isPlay = false;
			return false;
		}
		currentTime = startTime;
	}
	//定数バッファのデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	HRESULT result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	assert(SUCCEEDED(result));
	for (int i = 0; i < bones.size(); i++)
	{
		//今の姿勢
		XMMATRIX matCurrentPose;
		//今の姿勢行列を取得
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		//XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
		//合成してスキニングして行列に
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin->Unmap(0, nullptr);

	return true;
}

bool FBX3DModel::PlayAnimation(const std::string& animationName, bool endless, const int frameTime)
{
	if (nowAnimationName == animationName)
	{
		return PlayAnimation(endless);
	}
	if (!blend)
	{
		nowAnimationName = animationName;
		lerpAnimeName = animationName;
		SetAnimation(animationName, frameTime);
		lerpTime = 0.0f;
		return PlayAnimation(endless);
	}

	const auto nextAnimationTime = animations[animationName].startTime;
	if (lerpAnimeName != animationName)
	{
		lerpAnimeName = animationName;
		motionBlendTime = ClucLerpTime(currentTime, nextAnimationTime);

	}
	lerpTime += 1.0f / motionBlendTime;
	//定数バッファのデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	HRESULT result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	assert(SUCCEEDED(result));
	for (int i = 0; i < bones.size(); i++)
	{

		XMMATRIX matCurrentPose, matStartPose, matEndPose;
		////今の姿勢行列を取得
		//FbxAMatrix fbxStartPose =
		//	bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		////今の姿勢行列を取得
		//FbxAMatrix fbxEndPose =
		//	bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(nextAnimationTime);

		//XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFbx(&matStartPose, blendStartBorn[i]);
		FbxLoader::ConvertMatrixFromFbx(&matEndPose, blendEndBorn[i]);
		matCurrentPose = SlerpMatrix(matStartPose, matEndPose, lerpTime);
		//合成してスキニングして行列に
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin->Unmap(0, nullptr);

	if (lerpTime >= 1.0f)
	{
		nowAnimationName = animationName;
		SetAnimation(animationName, frameTime);
		lerpTime = 0.0f;
	}

	return true;
}