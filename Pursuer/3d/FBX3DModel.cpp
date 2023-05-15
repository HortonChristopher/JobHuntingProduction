#include "FBX3DModel.h"

extern DeltaTime* deltaTime;

FBX3DModel::~FBX3DModel()
{
}

void FBX3DModel::CreateBuffers()
{
	auto device = DirectXCommon::GetInstance()->GetDevice();

	HRESULT result;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // Uploadable
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin));

	assert(SUCCEEDED(result));

	// Overall vertex data of 'n' size
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUvSkin) * vertices.size());

	// Vertex buffer generation
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	assert(SUCCEEDED(result));

	// Data transfer to vertex buffer
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	if (SUCCEEDED(result))
	{
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	// Creating Vertex Buffer View (VBV)
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	// Overall vertex index size
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	// Index buffer creation
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	assert(SUCCEEDED(result));

	// Data transfer to index buffer
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	if (SUCCEEDED(result))
	{
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	// Creating Index Buffer View (IBV)
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);

	assert(SUCCEEDED(result));

	for (int i = 0; i < MAX_BONES; i++)
	{
		constMapSkin->bones[i] = XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);

	// アニメーションの初期化
	frameTime.SetTime(0, 0, 1, 0, 0, FbxTime::EMode::eFrames60);
	isPlay = true;
}

void FBX3DModel::Draw()
{
	auto cmdList = DirectXCommon::GetInstance()->GetCommandList();

	// Set vertex buffer (VBV)
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	// Set index buffer
	cmdList->IASetIndexBuffer(&ibView);

	// Descriptor heap set
	ID3D12DescriptorHeap* ppHeaps[] = { Textures::GetBasicDescriptorHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	if (!Object3D::GetShadowDrawing())
	{
		// Set shader resource view
		cmdList->SetGraphicsRootDescriptorTable(2, Textures::GetGPUDescriptorHandleSRV(texName));
		cmdList->SetGraphicsRootConstantBufferView(4, constBuffSkin->GetGPUVirtualAddress());
	}
	else
	{
		cmdList->SetGraphicsRootConstantBufferView(1, constBuffSkin->GetGPUVirtualAddress());
	}

	//描画コマンド
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

	// Time acquisition of animations
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	// Start time acquisition
	startTime = takeinfo->mLocalTimeSpan.GetStart();

	// End time acquisition
	endTime = takeinfo->mLocalTimeSpan.GetStop();

	// Adjust to start time
	currentTime = startTime;

	// Entering a state of playback
	isPlay = true;
}

void FBX3DModel::SetAnimationFrame(const int startFrame, const int endFrame, const int FrameTime)
{
	startTime.SetTime(0, 0, 0, startFrame, 0, FbxTime::EMode::eFrames60);
	currentTime = startTime;
	endTime.SetTime(0, 0, 0, endFrame, 0, FbxTime::EMode::eFrames60);

	if (startFrame > endFrame)
	{
		frameTime.SetTime(0, 0, 0, -FrameTime, 0, FbxTime::EMode::eFrames60);
	}
	else
	{
		frameTime.SetTime(0, 0, 0, FrameTime, 0, FbxTime::EMode::eFrames60);
	}

	isPlay = true;
}

void FBX3DModel::SetAnimation(const std::string& animationName, const int FrameTime)
{
	isPlay = true;

	// If the animation name is not registered, set all frames to 0
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
	{
		frameTime.SetTime(0, 0, 0, -FrameTime, 0, FbxTime::EMode::eFrames60);
	}
	else
	{
		frameTime.SetTime(0, 0, 0, FrameTime, 0, FbxTime::EMode::eFrames60);
	}
}

bool FBX3DModel::PlayAnimation(bool endless)
{
	// Advance one frame
	frameTime.SetTime(0, 0, 1, 0, 0, FbxTime::EMode::eFrames60);
	double sec = frameTime.GetSecondDouble();
	switch (frameSpeed)
	{
	case NORMAL:
		break;
	case HALF:
		sec *= 0.5f;
		break;
	case DOUBLE:
		sec *= 2.0f;
		break;
	case ONEPOINTFIVE:
		sec *= 1.5f;
		break;
	case POINTSEVENFIVE:
		sec *= 0.75f;
		break;
	default:
		break;
	}
	sec *= (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	frameTime.SetSecondDouble(sec);
	currentTime += frameTime;

	// Return to the previous position after playing to the end
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

	/*if (currentTime > endTime && repeatAnimation == true)
	{
		currentTime = startTime;
	}
	else if (currentTime > endTime && repeatAnimation == false
		&& enumStatus != PARTICLEATTACK && enumStatus != ATTACK)
	{
		currentTime = endTime;
	}*/

	// Constant buffer data transfer
	ConstBufferDataSkin* constMapSkin = nullptr;
	HRESULT result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	assert(SUCCEEDED(result));

	for (int i = 0; i < bones.size(); i++)
	{
		// Current posture matrix
		XMMATRIX matCurrentPose;
		// Get the current posture matrix
		FbxAMatrix fbxCurrentPose = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		// Convert to XMMATRIX
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
		// Synthesize into a skinning matrix
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	}
	constBuffSkin->Unmap(0, nullptr);

	return true;
}