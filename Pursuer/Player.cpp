#include "Player.h"
#include "FbxLoader/FbxLoader.h"
#include "input/Input.h"
#include "DeltaTime.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

///<summary>
/// Static Member Variable Entity
///</summary>
ID3D12Device* Player::device = nullptr;
Camera* Player::camera = nullptr;

ComPtr<ID3D12RootSignature> Player::rootsignature;
ComPtr<ID3D12PipelineState> Player::pipelinestate;

extern XMFLOAT3 objectPosition = { 0.0f, -10.0f, 0.0f };
extern XMFLOAT3 objectRotation = { 0.0f, 0.0f, 0.0f };
extern DeltaTime* deltaTime;

void Player::Initialize()
{
	modelStanding = FbxLoader::GetInstance()->LoadModelFromFile("ProtoStanding");
	modelWalking = FbxLoader::GetInstance()->LoadModelFromFile("ProtoWalk");
	modelRunning = FbxLoader::GetInstance()->LoadModelFromFile("ProtoRunning");
	modelDodgeF = FbxLoader::GetInstance()->LoadModelFromFile("ProtoDodgeForward");
	modelDodgeB = FbxLoader::GetInstance()->LoadModelFromFile("ProtoDodgeBack");
	modelDodgeL = FbxLoader::GetInstance()->LoadModelFromFile("ProtoDodgeLeft");
	modelDodgeR = FbxLoader::GetInstance()->LoadModelFromFile("ProtoDodgeRight");
	modelAttacking = FbxLoader::GetInstance()->LoadModelFromFile("ProtoAttack");
	modelDamaged = FbxLoader::GetInstance()->LoadModelFromFile("ProtoDamaged");
	modelDodgeRoll = FbxLoader::GetInstance()->LoadModelFromFile("ProtoDodgeRoll");

	HRESULT result;
	// Creation of Constant Buffer
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform));

	// Constant Buffer Creation (skinning)
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // Upload possible
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin));

	// Data transfer to constant buffer
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++)
	{
		constMapSkin->bones[i] = XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);

	// Create graphics pipeline
	Player::CreateGraphicsPipeline();

	// Set time for 1 second at 60fps
	frameTime.SetTime(0, 0, 1, 0, 0, FbxTime::EMode::eFrames60);

	input = Input::GetInstance();

	SetScale({ 3,3,3 });
	SetModel(modelStanding);
}

void Player::Update()
{
	XMMATRIX matScale, matRot, matTrans;

	// Achievements of scales, rotation, translation
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// World Line Composition
	matWorld = XMMatrixIdentity(); // Reset transformation
	matWorld *= matScale; // Reflect scaling in the world matrix
	matWorld *= matRot; // Reflect the rotation in the world matrix
	matWorld *= matTrans; // Reflect translation in world matrix

	// View projection matrix
	const XMMATRIX& matViewProjection = camera->GetViewProjectionMatrix();

	// Model mesh transformation
	const XMMATRIX& modelTransform = model->GetModelTransform();

	// Camera coordinates
	const XMFLOAT3& cameraPos = camera->GetEye();

	HRESULT result;

	// Data transfer to constant buffer
	ConstBufferDataTransform* constMap = nullptr;
	result = constBuffTransform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->viewproj = matViewProjection;
		constMap->world = modelTransform * matWorld;
		constMap->cameraPos = cameraPos;
		constBuffTransform->Unmap(0, nullptr);
	}

	// Bone array
	std::vector<FBX3DModel::Bone>& bones = model->GetBones();

	if (isPlay == false)
	{
		PlayAnimation();
	}

	if (isPlay)
	{
		// Advance one frame/second
		frameTime.SetTime(0, 0, 1, 0, 0, FbxTime::EMode::eFrames60);
		double sec = frameTime.GetSecondDouble();
		sec *= (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		frameTime.SetSecondDouble(sec);
		currentTime += frameTime;

		// Return to the previous position after playing to the end
		if (currentTime > endTime)
		{
			currentTime = startTime;
		}
	}

	// Constant buffer data transfer
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);

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

	//rotation.y = objectRotation.y;

	if (input->TriggerMouseLeft() && attackTime == 0 || input->TriggerControllerButton(XINPUT_GAMEPAD_A) && attackTime == 0)
	{
		attackTime = 60.0f;
	}

	if (attackTime > 0)
	{
		attackTime -= 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	}
	else
	{
		attackTime = 0;
	}

	XMMATRIX camMatWorld = XMMatrixInverse(nullptr, camera->GetViewMatrix());
	const Vector3 camDirectionZ = Vector3(camMatWorld.r[2].m128_f32[0], 0, camMatWorld.r[2].m128_f32[2]).Normalize();
	const Vector3 camDirectionY = Vector3(camMatWorld.r[1].m128_f32[0], 0, camMatWorld.r[1].m128_f32[2]).Normalize();
	const Vector3 camDirectionX = Vector3(camMatWorld.r[0].m128_f32[0], 0, camMatWorld.r[0].m128_f32[2]).Normalize();

	if (attackTime > 0)
	{
		if (animationNo != 7)
		{
			animationNo = 7;
			animationSet = false;
		}
	}
	else if (!dodge)
	{
		if (input->PushKey(DIK_A) || input->PushKey(DIK_D) || input->PushKey(DIK_S) || input->PushKey(DIK_W) ||
			input->PushLStickLeft() || input->PushLStickRight() || input->PushLStickDown() || input->PushLStickUp())
		{
			moveDirection = {};

			if (input->PushKey(DIK_A))
			{
				moveDirection += camDirectionX * -1;
			}
			if (input->PushKey(DIK_D))
			{
				moveDirection += camDirectionX;
			}
			if (input->PushKey(DIK_S))
			{
				moveDirection += camDirectionZ * -1;
			}
			if (input->PushKey(DIK_W))
			{
				moveDirection += camDirectionZ;
			}
			if (input->PushLStickLeft() || input->PushLStickRight() || input->PushLStickDown() || input->PushLStickUp())
			{
				auto vector = input->GetLStickDirection();

				moveDirection = camDirectionX * vector.x + camDirectionZ * vector.y;
			}

			moveDirection.Normalize();

			direction.Normalize();
			float cosA = direction.Dot(moveDirection);
			if (cosA > 1.0f)
			{
				cosA = 1.0f;
			}
			else if (cosA < -1.0f)
			{
				cosA = -1.0f;
			}

			/*if (input->UpKey(DIK_SPACE) || input->UpControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
			{
				cosA = 1.0f;
			}
			debug2 = cosA;*/

			float rotY = (float)acos(cosA) * 180 / 3.14159365f;
			const Vector3 CrossVec = direction.Cross(moveDirection);

			float rotSpeed = rotateSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (abs(rotY) < 55 && !dodge)
			{
				if (input->PushKey(DIK_LSHIFT) && stamina > 0.0f || input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && stamina > 0.0f)
				{
					position.x += moveDirection.x * sprintSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
					position.y += moveDirection.y * sprintSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
					position.z += moveDirection.z * sprintSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
					stamina -= 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				}
				else
				{
					position.x += moveDirection.x * speed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
					position.y += moveDirection.y * speed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
					position.z += moveDirection.z * speed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				}
			}

			/*if (input->UpKey(DIK_SPACE) || input->UpControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
			{

			}*/

			if (rotSpeed > abs(rotY))
			{
				rotSpeed = rotY;
			}

			/*if (input->UpKey(DIK_SPACE) || input->UpControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
			{
				rotY = 1.0f;
			}
			debug = rotY;*/

			if (CrossVec.y < 0)
			{
				rotSpeed *= -1;
			}

			rotation.y += rotSpeed;

			XMMATRIX matRotation = XMMatrixRotationY(XMConvertToRadians(rotSpeed));
			XMVECTOR dir = { direction.x, direction.y, direction.z, 0 };
			dir = XMVector3TransformNormal(dir, matRotation);
			direction = dir;

			SetPosition(position);
			if (!input->PushKey(DIK_SPACE))
			{
				SetRotation(rotation);
			}

			if (input->TriggerKey(DIK_A) || input->TriggerKey(DIK_D) || input->TriggerKey(DIK_S) || input->TriggerKey(DIK_W) ||
				input->TriggerLStickLeft() || input->TriggerLStickRight() || input->TriggerLStickDown() || input->TriggerLStickUp())
			{
				animationNo = 2;
				animationSet = false;
			}
		}
		else
		{
			if (animationNo != 0)
			{
				animationNo = 0;
				animationSet = false;
			}
		}
	}

	/*if (input->PushKey(DIK_LCONTROL) && !dodge || input->PushControllerButton(XINPUT_GAMEPAD_B) && !dodge)
	{
		if (input->PushKey(DIK_W) || input->PushLStickUp())
		{
			enumDodgeDirection = FORWARD;
			dodge = true;
		}
		else if (input->PushKey(DIK_S) || input->PushLStickDown())
		{
			enumDodgeDirection = BACKWARD;
			dodge = true;
		}
		else if (input->PushKey(DIK_A) || input->PushLStickLeft())
		{
			enumDodgeDirection = LEFT;
			dodge = true;
		}
		else if (input->PushKey(DIK_D) || input->PushLStickRight())
		{
			enumDodgeDirection = RIGHT;
			dodge = true;
		}
	}*/

	if (input->PushKey(DIK_LCONTROL) && !dodge || input->PushControllerButton(XINPUT_GAMEPAD_B) && !dodge)
	{
		dodge = true;
	}

	if (dodge)
	{
		if (frameTimeInt == 0)
		{
			animationNo = 9;
			animationSet = false;
		}
		frameTimeInt += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		position.x += moveDirection.x * rollSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		position.y += moveDirection.y * rollSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		position.z += moveDirection.z * rollSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		if (frameTimeInt > 58.9f)
		{
			frameTimeInt = 0.0f;
			dodge = false;
		}

		/*switch (enumDodgeDirection)
		{
		case FORWARD:
			if (frameTimeInt == 0)
			{
				animationNo = 3;
				animationSet = false;
			}
			frameTimeInt += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (frameTimeInt > 29.9f)
			{
				enumDodgeDirection = NONE;
				frameTimeInt = 0.0f;
				dodge = false;
			}
			break;
		case BACKWARD:
			if (frameTimeInt == 0)
			{
				animationNo = 4;
				animationSet = false;
			}
			frameTimeInt += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (frameTimeInt > 48.9f)
			{
				enumDodgeDirection = NONE;
				frameTimeInt = 0.0f;
				dodge = false;
			}
			break;
		case LEFT:
			if (frameTimeInt == 0)
			{
				animationNo = 6;
				animationSet = false;
			}
			frameTimeInt += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (frameTimeInt > 28.9f)
			{
				enumDodgeDirection = NONE;
				frameTimeInt = 0.0f;
				dodge = false;
			}
			break;
		case RIGHT:
			if (frameTimeInt == 0)
			{
				animationNo = 5;
				animationSet = false;
			}
			frameTimeInt += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (frameTimeInt > 28.9f)
			{
				enumDodgeDirection = NONE;
				frameTimeInt = 0.0f;
				dodge = false;
			}
			break;
		case NONE:
			frameTimeInt = 0.0f;
			dodge = false;
			break;
		}*/
	}

	//Debug Start
	char msgbuf[256];
	char msgbuf2[256];
	//char msgbuf3[256];

	sprintf_s(msgbuf, 256, "Dodge: %d\n", dodge);
	sprintf_s(msgbuf2, 256, "Frame Time: %f\n", frameTimeInt);
	//sprintf_s(msgbuf3, 256, "Z: %f\n", moveDirection.z);
	OutputDebugStringA(msgbuf);
	OutputDebugStringA(msgbuf2);
	//OutputDebugStringA(msgbuf3);
	//Debug End

	if (!input->PushKey(DIK_LSHIFT) && !input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER))
	{
		if (stamina < 100.0f)
		{
			stamina += 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}
	}

	//// 移動ベクトルをY軸周りの角度で回転
	//XMVECTOR move = { 0,0,1.0f,0 };
	//matRot = XMMatrixRotationY(XMConvertToRadians(rotation.y));
	//move = XMVector3TransformNormal(move, matRot);

	//// 向いている方向に移動
	//if (input->PushKey(DIK_S) || input->PushLStickDown()) {
	//	position.x -= move.m128_f32[0];
	//	position.y -= move.m128_f32[1];
	//	position.z -= move.m128_f32[2];
	//}
	//else if (input->PushKey(DIK_W) || input->PushLStickUp()) {
	//	position.x += move.m128_f32[0];
	//	position.y += move.m128_f32[1];
	//	position.z += move.m128_f32[2];
	//}

	if (!animationSet)
	{
		switch (animationNo)
		{
		case 0:
			SetModel(modelStanding);
			isPlay = false;
			animationSet = true;
			break;
		case 1:
			SetModel(modelWalking);
			isPlay = false;
			animationSet = true;
			break;
		case 2:
			SetModel(modelRunning);
			isPlay = false;
			animationSet = true;
			break;
		case 3:
			SetModel(modelDodgeF);
			isPlay = false;
			animationSet = true;
			break;
		case 4:
			SetModel(modelDodgeB);
			isPlay = false;
			animationSet = true;
			break;
		case 5: // Case 5 and 6 are currently running opposite animations: must investigate
			SetModel(modelDodgeL);
			isPlay = false;
			animationSet = true;
			break;
		case 6:
			SetModel(modelDodgeR);
			isPlay = false;
			animationSet = true;
			break;
		case 7:
			SetModel(modelAttacking);
			isPlay = false;
			animationSet = true;
			break;
		case 8:
			SetModel(modelDamaged);
			isPlay = false;
			animationSet = true;
			break;
		case 9:
			SetModel(modelDodgeRoll);
			isPlay = false;
			animationSet = true;
			break;
		}
	}
}

void Player::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // Vertex shader object
	ComPtr<ID3DBlob> psBlob;    // Pixel shader object
	ComPtr<ID3DBlob> errorBlob; // Error object

	assert(device);

	// Load and compile vertex shader
	result = D3DCompileFromFile(
		L"Resources/shaders/FBXVS.hlsl",    // Shader file name
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enable to include
		"main", "vs_5_0",    // Entry point name, shader model specification
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Debug settings
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// Copy the error content from errorBlob to string type
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// Display error details in output window
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// Loading and compiling pixel shaders
	result = D3DCompileFromFile(
		L"Resources/shaders/FBXPS.hlsl",    // Shader file name
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // Enable to include
		"main", "ps_5_0",    // Entry point name, shader model specification
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Debug settings
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// Copy the error content from errorBlob to string type
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// Display error details in output window
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy coordinates (it is easier to see if written in one line)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // Normal vector (easier to see if written in one line)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv coordinates (it is easier to see if written in one line)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // Bone number to receive (4)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // Bone skin weight (4)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// Set the flow of the graphics pipeline
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// Sample mask
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// Rasterizer state
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// Depth stencil state
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// Render target blend settings
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA Draw all channels
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// Blend state setting
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// Depth buffer format
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// Vertex layout settings
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// Shape setting (triangle)
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;    // Two drawing targets
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA specified from 0 to 255
	gpipeline.SampleDesc.Count = 1; // Sampling once per pixel

	// Descriptor range
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 register

	// ルートパラメータ
	//CD3DX12_ROOT_PARAMETER rootparams[2];
	CD3DX12_ROOT_PARAMETER rootparams[3];
	// CBV (for coordinate transformation matrix)
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	// SRV (texture)
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	// CBV (skinning)
	rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);

	// Static sampler
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// Route signature settings
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// Serialization of automatic version judgment
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// Route signature generation
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = rootsignature.Get();

	// Graphics pipeline generation
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// Return if no model
	if (model == nullptr)
	{
		return;
	}

	// Pipeline state setting
	cmdList->SetPipelineState(pipelinestate.Get());

	// Root Graphics Signature setting
	cmdList->SetGraphicsRootSignature(rootsignature.Get());

	// Set the primitive shape
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set constant buffer view
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffTransform->GetGPUVirtualAddress());

	// Set constant buffer view (skinning)
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffSkin->GetGPUVirtualAddress());

	// Model Drawing
	model->Draw(cmdList);
}

void Player::PlayAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();

	// Get animation frame 0
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);

	// Get name of animation
	const char* animstackname = animstack->GetName();

	// Animation time information
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	// Get start time
	startTime = takeinfo->mLocalTimeSpan.GetStart();

	// Get end time
	endTime = takeinfo->mLocalTimeSpan.GetStop();

	// Match start time
	currentTime = startTime;

	// Make request during playback
	isPlay = true;
}