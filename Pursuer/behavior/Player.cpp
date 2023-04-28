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
extern bool lockOnActive;
extern XMFLOAT3 playerMovement;

void Player::Initialize()
{
	modelPlayer = FbxLoader::GetInstance()->LoadModelFromFile("Player");

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
	//modelPlayer->FBX3DModel::GraphicsPipelineCreation(device, rootsignature, pipelinestate);
	CreateGraphicsPipeline();

	// Set time for 1 second at 60fps
	frameTime.SetTime(0, 0, 1, 0, 0, FbxTime::EMode::eFrames60);

	input = Input::GetInstance();

	SetScale(scale);
	SetModel(modelPlayer);
}

void Player::Update()
{
	switch (enumStatus)
	{
	case STAND:
		if (animationNo != 0)
		{
			animationNo = 0;
			animationSet = false;
		}
		break;
	case WALK:
		if (animationNo != 1)
		{
			animationNo = 1;
			animationSet = false;
		}
		break;
	case RUN:
		if (animationNo != 2)
		{
			animationNo = 2;
			animationSet = false;
		}
		break;
	case STRAFEL:
		if (animationNo != 3)
		{
			animationNo = 3;
			animationSet = false;
		}
		break;
	case STRAFER:
		if (animationNo != 4)
		{
			animationNo = 4;
			animationSet = false;
		}
		break;
	case STRAFEB:
		if (animationNo != 5)
		{
			animationNo = 5;
			animationSet = false;
		}
		break;
	case DODGE:
		if (animationNo != 6)
		{
			animationNo = 6;
			animationSet = false;
		}
		if (dodgeStartPosition.x == 0.0f && dodgeStartPosition.y == 10.0f && dodgeStartPosition.z == 0.0f)
		{
			timer = 0.0f;
			dodgeStartPosition = position;
		}
		dodgeCameraTime += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		position.x += moveDirection.x * rollSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		position.y += moveDirection.y * rollSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		position.z += moveDirection.z * rollSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		dodgePosition.x = Easing::EaseInSin(dodgeStartPosition.x, dodgeStartPosition.x + (moveDirection.x * 0.75f * 68.0f), 68.0f, dodgeCameraTime);
		dodgePosition.y = position.y;
		dodgePosition.z = Easing::EaseInSin(dodgeStartPosition.z, dodgeStartPosition.z + (moveDirection.z * 0.75f * 68.0f), 68.0f, dodgeCameraTime);
		if (currentTime > endTime && timer > 0.0f)
		{
			timer = 0.0f;
			dodgeStartPosition = { 0.0f, 10.0f, 0.0f };
			dodgeCameraTime = 0.0f;
			enumStatus = STAND;
		}
		timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		break;
	case ATTACK:
		if (animationNo != 7)
		{
			timer = 0.0f;
			animationNo = 7;
			animationSet = false;
		}

		if (timer < 48.0f || timer > 68.0f && timer < 84.0f || timer > 104.0f && timer < 146.0f || timer > 166.0f)
		{
			frameSpeed = ONEPOINTFIVE;
		}
		else
		{
			frameSpeed = NORMAL;
		}

		switch (attackCombo)
		{
		case 0:
			break;
		case 1:
			if (currentTime - startTime > (endTime - startTime) / 3 && timer > 0.0f)
			{
				timer = 0.0f;
				attackCombo = 0;
				frameSpeed = NORMAL;
				enumStatus = STAND;
			}
			break;
		case 2:
			if (currentTime - startTime > (endTime - startTime) / 2 && timer > 0.0f)
			{
				timer = 0.0f;
				attackCombo = 0;
				frameSpeed = NORMAL;
				enumStatus = STAND;
			}
			break;
		case 3:
			if (currentTime - startTime > endTime - startTime && timer > 0.0f)
			{
				timer = 0.0f;
				attackCombo = 0;
				frameSpeed = NORMAL;
				enumStatus = STAND;
			}
			break;
		}

		switch (frameSpeed)
		{
		case NORMAL:
			timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		case HALF:
			timer += 30.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		case DOUBLE:
			timer += 120.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		case ONEPOINTFIVE:
			timer += 90.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		case POINTSEVENFIVE:
			timer += 45.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		default:
			timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		}
		break;
	case DAMAGED:
		if (animationNo != 8)
		{
			timer = 0.0f;
			frameSpeed = ONEPOINTFIVE;
			animationNo = 8;
			animationSet = false;
		}
		if (currentTime > endTime && timer > 0.0f)
		{
			timer = 0.0f;
			frameSpeed = NORMAL;
			enumStatus = STAND;
		}
		timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		break;
	case DEAD:
		if (animationNo != 9)
		{
			timer = 0.0f;
			animationNo = 9;
			animationSet = false;
		}
		if (currentTime - startTime > endTime - startTime && timer > 0.0f)
		{
			timer = 0.0f;
			isPlayerDead = true;
		}
		timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		break;
	case HEAL:
		if (animationNo != 10)
		{
			timer = 0.0f;
			healed = false;
			for (int i = 0; i < 3; i++)
			{
				healParticlePosition[i] = position;
			}
			animationNo = 10;
			animationSet = false;
		}
		if (timer >= 80.0f)
		{
			healParticlePosition[0].y += 40.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (!healed)
			{
				hp += 4.0f;
				if (hp >= 10.0f)
				{
					hp = 10.0f;
				}
				healed = true;
			}
		}
		if (timer >= 90.0f)
		{
			healParticlePosition[1].y += 40.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}
		if (timer >= 100.0f)
		{
			healParticlePosition[2].y += 40.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}
		if (currentTime > endTime && timer > 0.0f)
		{
			debugTimer = timer;
			timer = 0.0f;
			healed = false;
			enumStatus = STAND;
		}
		timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		break;
	}

	if (!baseAreaOpeningCutscene)
	{
		if (input->TriggerMouseLeft() && stamina >= 25.0f || input->TriggerControllerButton(XINPUT_GAMEPAD_A) && stamina >= 25.0f)
		{
			switch (attackCombo)
			{
			case 0:
				attackCombo++;
				enumStatus = ATTACK;
				stamina -= 25.0f;
				break;
			case 1:
				attackCombo++;
				stamina -= 25.0f;
				break;
			case 2:
				attackCombo++;
				stamina -= 25.0f;
				break;
			case 3:
				break;
			}
		}
		else if (input->TriggerMouseLeft() && stamina < 25.0f || input->TriggerControllerButton(XINPUT_GAMEPAD_A) && stamina < 25.0f)
		{
			lowStaminaWarningActivation = true;
		}

		if (input->TriggerKey(DIK_H) && healRemaining > 0 && enumStatus != HEAL||
			input->TriggerControllerButton(XINPUT_GAMEPAD_Y) && healRemaining > 0 && enumStatus != HEAL)
		{
			enumStatus = HEAL;
			healRemaining--;
		}

		XMMATRIX camMatWorld = XMMatrixInverse(nullptr, camera->GetViewMatrix());
		const Vector3 camDirectionZ = Vector3(camMatWorld.r[2].m128_f32[0], 0, camMatWorld.r[2].m128_f32[2]).Normalize();
		const Vector3 camDirectionY = Vector3(camMatWorld.r[1].m128_f32[0], 0, camMatWorld.r[1].m128_f32[2]).Normalize();
		const Vector3 camDirectionX = Vector3(camMatWorld.r[0].m128_f32[0], 0, camMatWorld.r[0].m128_f32[2]).Normalize();

		if (input->PushKey(DIK_A) || input->PushKey(DIK_D) || input->PushKey(DIK_S) || input->PushKey(DIK_W) ||
			input->PushLStickLeft() || input->PushLStickRight() || input->PushLStickDown() || input->PushLStickUp())
		{
			if (enumStatus != DEAD && enumStatus != DAMAGED && enumStatus != DODGE && enumStatus != HEAL)
			{
				if (enumStatus == ATTACK)
				{
					if (timer < 48.0f || timer > 68.0f && timer < 84.0f || timer > 104.0f && timer < 146.0f || timer > 166.0f)
					{
						movementAllowed = true;
					}
					else
					{
						movementAllowed = false;
					}
				}
				else
				{
					movementAllowed = true;
				}
			}
			else
			{
				movementAllowed = false;
			}
		}
		else
		{
			if (enumStatus != DEAD && enumStatus != DAMAGED && enumStatus != ATTACK && enumStatus != DODGE && enumStatus != HEAL)
			{
				enumStatus = STAND;
			}
			movementAllowed = false;
		}

		if (movementAllowed)
		{
			if (input->PushKey(DIK_A) || input->PushKey(DIK_D) || input->PushKey(DIK_S) || input->PushKey(DIK_W) ||
				input->PushLStickLeft() || input->PushLStickRight() || input->PushLStickDown() || input->PushLStickUp())
			{
				moveDirection = {};

				if (input->PushKey(DIK_A))
				{
					moveDirection += camDirectionX * -1;
				}
				else if (input->PushKey(DIK_D))
				{
					moveDirection += camDirectionX;
				}
				if (input->PushKey(DIK_S))
				{
					moveDirection += camDirectionZ * -1;
				}
				else if (input->PushKey(DIK_W))
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

				float cosA;
				/*if (input->UpKey(DIK_SPACE) || input->UpControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
				{
					direction = { 0, 0, 1 };
					rotation = { 0, 0, 0 };
				}*/
				cosA = direction.Dot(moveDirection);

				/*if (input->UpKey(DIK_SPACE) || input->UpControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
				{
					cosA = 1.0f;
				}*/
				if (cosA > 1.0f)
				{
					cosA = 1.0f;
				}
				else if (cosA < -1.0f)
				{
					cosA = -1.0f;
				}

				float rotY = (float)acos(cosA) * 180 / 3.14159365f;
				const Vector3 CrossVec = direction.Cross(moveDirection);

				float rotSpeed = rotateSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				if (abs(rotY) < 55 && !dodge && enumStatus != DAMAGED)
				{
					if (input->PushKey(DIK_LSHIFT) && stamina > 0.0f || input->PushControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && stamina > 0.0f /*|| input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && stamina > 0.0f */ )
					{
						position.x += moveDirection.x * sprintSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
						position.y += moveDirection.y * sprintSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
						position.z += moveDirection.z * sprintSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
						playerMovement = { moveDirection.x * sprintSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f),
										   moveDirection.y * sprintSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f),
										   moveDirection.z * sprintSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) };
						stamina -= 30.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
						if (enumStatus != ATTACK && !input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && !input->PushKey(DIK_SPACE))
						{
							enumStatus = RUN;
						}
					}
					else
					{
						position.x += moveDirection.x * speed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
						position.y += moveDirection.y * speed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
						position.z += moveDirection.z * speed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
						playerMovement = { moveDirection.x * speed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f),
										   moveDirection.y * speed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f),
										   moveDirection.z * speed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) };
						if (enumStatus != ATTACK && !input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && !input->PushKey(DIK_SPACE))
						{
							enumStatus = WALK;
						}
					}
				}

				if (rotSpeed > abs(rotY))
				{
					rotSpeed = rotY;
				}

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
				//if (!input->PushKey(DIK_SPACE))
				//{
				//	//SetRotation(rotation);
				//}

				if (input->PushKey(DIK_SPACE) && lockOnActive || input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && lockOnActive)
				{
					if (enumStatus != ATTACK)
					{
						if (input->PushKey(DIK_A) || input->PushLStickLeft())
						{
							enumStatus = STRAFER;
						}
						else if (input->PushKey(DIK_D) || input->PushLStickRight())
						{
							enumStatus = STRAFEL;
						}
						else if (input->PushKey(DIK_S) || input->PushLStickDown())
						{
							enumStatus = STRAFEB;
						}
						else
						{
							if (input->PushKey(DIK_LSHIFT) && stamina > 0.0f || input->PushControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && stamina > 0.0f || input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && stamina > 0.0f)
							{
								enumStatus = RUN;
							}
							else
							{
								enumStatus = WALK;
							}
						}
					}
				}
			}
		}

		if (input->TriggerKey(DIK_LCONTROL) && !dodge && stamina >= 40.0f || input->TriggerControllerButton(XINPUT_GAMEPAD_B) && !dodge && stamina >= 40.0f)
		{
			stamina -= 40.0f;
			enumStatus = DODGE;
		}
		else if (input->TriggerKey(DIK_LCONTROL) && !dodge && stamina < 40.0f || input->TriggerControllerButton(XINPUT_GAMEPAD_B) && !dodge && stamina < 40.0f)
		{
			lowStaminaWarningActivation = true;
		}

		if (lowStaminaWarningActivation)
		{
			staminaWarningSpriteAlpha = 1.0f;
			lowStaminaWarningActivation = false;
		}
		else if (!lowStaminaWarningActivation && staminaWarningSpriteAlpha > 0.0f)
		{
			if (lowStaminaWarningTimer >= 15.0f)
			{
				staminaWarningSpriteAlpha -= 1.4f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			else
			{
				lowStaminaWarningTimer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
		}
		else
		{
			staminaWarningSpriteAlpha = 0.0f;
			lowStaminaWarningTimer = 0.0f;
		}

		if (!input->PushKey(DIK_LSHIFT) && !input->PushControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && !input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER))
		{
			if (stamina < 100.0f)
			{
				stamina += 25.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
		}
	}

	if (!animationSet)
	{
		switch (animationNo)
		{
		case 0:
			startFrame = 1;
			endFrame = 359;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 1:
			startFrame = 361;
			endFrame = 416;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 2:
			startFrame = 418;
			endFrame = 447;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 3:
			startFrame = 449;
			endFrame = 487;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 4:
			startFrame = 489;
			endFrame = 527;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 5:
			startFrame = 529;
			endFrame = 566;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 6:
			startFrame = 926;
			endFrame = 998;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 7:
			startFrame = 568;
			endFrame = 818;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 8:
			startFrame = 820;
			endFrame = 924;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 9:
			startFrame = 1000;
			endFrame = 1139;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 10:
			startFrame = 1141;
			endFrame = 1299;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		}
	}

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

	if (isPlay == false)
	{
		PlayAnimation();
		//modelPlayer->PlayAnimationInit(startFrame, endFrame, 1, startTime, endTime, frameTime, currentTime, isPlay);
	}

	//modelPlayer->PlayAnimation(repeatAnimation, isPlay, startTime, endTime, frameTime, currentTime);

	if (isPlay)
	{
		// Advance one frame/second
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
		if (currentTime > endTime && enumStatus == DEAD)
		{
			currentTime = endTime + 1;
		}
		else if (currentTime > endTime && repeatAnimation == true)
		{
			currentTime = startTime;
		}
	}

	//Debug Start
	/*char msgbuf[256];
	char msgbuf2[256];
	char msgbuf3[256];

	sprintf_s(msgbuf, 256, "Float Animation Time: %d\n", currentTime);
	sprintf_s(msgbuf2, 256, "Y: %d\n", startTime);
	sprintf_s(msgbuf3, 256, "Z: %d\n", endTime);

	OutputDebugStringA(msgbuf);
	OutputDebugStringA(msgbuf2);
	OutputDebugStringA(msgbuf3);*/
	//Debug End
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

	startTime.SetTime(0, 0, 0, startFrame, 0, FbxTime::EMode::eFrames60);
	currentTime = startTime;
	endTime.SetTime(0, 0, 0, endFrame, 0, FbxTime::EMode::eFrames60);
	if (startFrame > endFrame)
		frameTime.SetTime(0, 0, -1, 0, 0, FbxTime::EMode::eFrames60);
	else
		frameTime.SetTime(0, 0, 1, 0, 0, FbxTime::EMode::eFrames60);

	// Make request during playback
	isPlay = true;
}