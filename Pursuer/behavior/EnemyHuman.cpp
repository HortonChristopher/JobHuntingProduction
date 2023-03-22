#include "EnemyHuman.h"
#include "Player.h"
#include "GameScene.h"
#include "FbxGeneration.h"
#include "FbxLoader/FbxLoader.h"
#include "DeltaTime.h"

#include <time.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;
extern DeltaTime* deltaTime;

extern int agroodEnemies;
extern int debugJetStream;

///<summary>
/// Static Member Variable Entity
///</summary>
ID3D12Device* EnemyHuman::device = nullptr;
Camera* EnemyHuman::camera = nullptr;

ComPtr<ID3D12RootSignature> EnemyHuman::rootsignature;
ComPtr<ID3D12PipelineState> EnemyHuman::pipelinestate;

void EnemyHuman::Initialize()
{
	modelEnemy = FbxLoader::GetInstance()->LoadModelFromFile("EnemyHuman");

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
	EnemyHuman::CreateGraphicsPipeline();

	// Set time for 1 second at 60fps
	frameTime.SetTime(0, 0, 1, 0, 0, FbxTime::EMode::eFrames60);

	input = Input::GetInstance();

	SetPosition(position);
	SetModel(modelEnemy);
	SetScale(scale);
	srand((unsigned int)time(NULL));
}

void EnemyHuman::Update()
{
	switch (enumStatus)
	{
	case STAND:
		timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		if (timer > 239.0f)
		{
			newPosition.x = rand() % 401 - 150 + homePosition.x;
			newPosition.z = rand() % 401 - 150 + homePosition.y;
			timer = 0.0f;
			enumStatus = WANDER;
		}
		break;
	case WANDER:
		if (!set)
		{
			x = (newPosition.x - position.x) / 300.0f;
			y = (newPosition.z - position.z) / 300.0f;
			float x2 = newPosition.x - position.x;
			float y2 = newPosition.z - position.z;
			float radians = atan2(y2, x2);
			degrees = XMConvertToDegrees(radians);
			set = true;
			modelChange = true;
			if (modelChange)
			{
				animationSet = false;
				animationNo = 1;
				modelChange = false;
			}
		}
		else
		{
			if (timer > 360.0f)
			{
				timer = 0.0f;
				wander = false;
				set = false;
				modelChange = true;
				if (modelChange)
				{
					animationSet = false;
					animationNo = 0;
					modelChange = false;
				}
				enumStatus = STAND;
			}
			else
			{
				timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}

			if (!FirstRun)
			{
				position.x += x * 50.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				position.z += y * 50.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			else
			{
				FirstRun = false;
			}
			SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });
			SetPosition(position);
		}
		break;
	case AGGRO:
		if (!aggroSet)
		{
			modelChange = true;
			if (modelChange)
			{
				animationSet = false;
				animationNo = 2;
				modelChange = false;
			}
			aggroSet = true;
		}
		else if (aggroSet)
		{
			x = objectPosition.x - position.x;
			y = objectPosition.z - position.z;
			hypotenuse = sqrt((x * x) + (y * y));
			float radians = atan2(y, x);
			degrees = XMConvertToDegrees(radians);
			if (!FirstRun)
			{
				position.x += 40.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (x / hypotenuse);
				position.z += 40.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (y / hypotenuse);
			}
			else
			{
				FirstRun = false;
			}
			SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });
			SetPosition(position);
			if (aggroSwitch)
			{
				animationSet = false;
				animationNo = 0;
				aggroSet = false;
				aggro = false;
				wander = false;
				set = false;
				timer = 0.0f;
				aggroSwitch = false;
				enumStatus = STAND;
			}
		}
		break;
	case ATTACK:
		if (!attackAnimation)
		{
			attackTimer = 0.0f;
			animationSet = false;
			animationNo = 3;
			attackAnimation = true;
		}

		if (attackTimer > 53.3f && attackTimer < 63.2f)
		{
			attackDamagePossible = true;
		}
		else
		{
			attackDamagePossible = false;
		}

		if (currentTime >= endTime && attackTimer > 0.0f)
		{
			attackTimer = 0.0f;
			modelChange = true;
			attackAnimation = false;
			timer = 0.0f;
			enumStatus = COOLDOWN;
		}
		attackTimer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		break;
	case COOLDOWN:
		if (modelChange)
		{
			animationSet = false;
			animationNo = 0;
			cooldownPosition = position;
			modelChange = false;
		}
		SetPosition(cooldownPosition);
		timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		if (timer > 20.0f)
		{
			timer = 0.0f;
			ableToDamage = true;
			aggroSet = false;
			modelChange = true;
			enumStatus = AGGRO;
		}
		break;
	case DAMAGED:
		if (modelChange)
		{
			animationSet = false;
			animationNo = 4;
			modelChange = false;
		}
		timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		if (timer > 53.0f)
		{
			timer = 0.0f;
			aggroSet = false;
			modelChange = true;
			enumStatus = AGGRO;
		}
		break;
	case DEAD:
		if (modelChange)
		{
			animationSet = false;
			animationNo = 5;
			modelChange = false;
		}
		break;
	case PARTICLEATTACK:
		switch (particleAttackStage)
		{
		case 0:
			if (modelChange)
			{
				timer = 0.0f;
				animationSet = false;
				animationNo = 6;
				modelChange = false;
			}
			x = objectPosition.x - position.x;
			y = objectPosition.z - position.z;
			hypotenuse = sqrt((x * x) + (y * y));
			radians = atan2(y, x);
			degrees = XMConvertToDegrees(radians);
			position.x -= 90.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (x / hypotenuse);
			position.z -= 90.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (y / hypotenuse);
			SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });
			if (currentTime < endTime / 2 && timer > 0.0f)
			{
				position.y += 1.0f;
			}
			SetPosition(position);
			if (currentTime > endTime && timer > 0.0f)
			{
				particleAttackStage = 1;
				modelChange = true;
			}
			timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		case 1:
			if (modelChange)
			{
				timer = 0.0f;
				particleAttackPosition = { position.x, position.y + 8.0f, position.z };
				animationSet = false;
				animationNo = 7;
				modelChange = false;
			}
			if (currentTime - startTime > (endTime - startTime) / 2 && timer > 0.0f && !particleAttackActive)
			{
				particleAttackActive = true;
				timer = 0.0f;
			}
			if (particleAttackActive)
			{
				particleAttackPosition.x += 180.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (x / hypotenuse);
				particleAttackPosition.y += 180.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (y / hypotenuse);
				particleAttackPosition.z += 180.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (z / hypotenuse);
				timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			else
			{
				x = objectPosition.x - position.x;
				y = objectPosition.y - position.y;
				z = objectPosition.z - position.z;
				hypotenuse = sqrt((x * x) + (y * y) + (z * z));
				radians = atan2(z, x);
				degrees = XMConvertToDegrees(radians);
				SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });
				timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			if (currentTime - startTime > (endTime - startTime) && particleAttackActive)
			{
				ableToDamage = true;
				timer = 0.0f;
				particleAttackActive = false;
				particleAttackStage = 0;
				modelChange = true;
				aggroSet = false;
				enumStatus = AGGRO;
			}
			break;
		}
		break;
	case FLEE:
		if (animationNo != 8)
		{
			animationSet = false;
			animationNo = 8;
			modelChange = false;
		}
		break;
	case LANDINGATTACK:
		switch (landingAttackStage)
		{
		case 0:
			if (animationNo != 6)
			{
				animationSet = false;
				animationNo = 6;
				modelChange = false;
			}

			if (position.y < 100.0f)
			{
				position.y += 50.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				landingAttackPosition = objectPosition;
			}
			else
			{
				position.y = 100.0f;
				x = (landingAttackPosition.x - position.x);
				y = (landingAttackPosition.y - position.y);
				z = (landingAttackPosition.z - position.z);
				hypotenuse = sqrt((x * x) + (z * z));
				radians = atan2(z, x);
				degrees = XMConvertToDegrees(radians);
				x = (landingAttackPosition.x - position.x) / 160.0f;
				y = (landingAttackPosition.y - position.y) / 160.0f;
				z = (landingAttackPosition.z - position.z) / 160.0f;
				SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });
				landingAttackStage = 1;
			}
			break;
		case 1:
			if (position.y > landingAttackPosition.y)
			{
				if (animationNo != 6)
				{
					animationSet = false;
					animationNo = 6;
					modelChange = false;
					landed = true;
				}

				position.x += x * 240.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				position.y -= 150.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				position.z += z * 240.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			else
			{
				if (animationNo != 9)
				{
					animationSet = false;
					animationNo = 9;
					modelChange = false;
					if (position.y <= landingAttackPosition.y)
					{
						position.y = landingAttackPosition.y;
					}
					landed = false;
					landingParticles = true;
					timer = 0.0f;
					SetScale({ scale.x * 0.01f, scale.y * 0.01f, scale.z * 0.01f });
					SetRotation({ rotation.x + 90.0f, rotation.y, rotation.z });
				}

				if (currentTime >= endTime && timer > 0.0f)
				{
					timer = 0.0f;
					SetPosition({ position.x, position.y + 8.0f, position.z });
					modelChange = true;
					enumStatus = COOLDOWN;
					landed = false;
					SetRotation({ rotation.x - 90.0f, rotation.y, rotation.z });
				}

				timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			break;
		}
		break;
	case CHARGEATTACK:
		switch (chargeAttackStage)
		{
		case 0:
			if (animationNo != 10)
			{
				animationSet = false;
				animationNo = 10;
				modelChange = false;
				timer = 0.0f;
				SetScale({ scale.x * 0.01f, scale.y * 0.01f, scale.z * 0.01f });
				SetRotation({ rotation.x + 90.0f, rotation.y, rotation.z });
				chargeAttackCheck = true;
			}

			landingAttackPosition = objectPosition;

			x = (landingAttackPosition.x - position.x);
			z = (landingAttackPosition.z - position.z);
			hypotenuse = sqrt((x * x) + (z * z));
			radians = atan2(z, x);
			degrees = XMConvertToDegrees(radians);
			SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });

			if (currentTime >= endTime && timer > 0.0f) // Timer is 53.0f
			{
				timer = 0.0f;
				landingAttackPosition = objectPosition;
				x = (landingAttackPosition.x - position.x);
				z = (landingAttackPosition.z - position.z);
				hypotenuse = sqrt((x * x) + (z * z));
				radians = atan2(z, x);
				degrees = XMConvertToDegrees(radians);
				x = (landingAttackPosition.x - position.x) / 60.0f;
				z = (landingAttackPosition.z - position.z) / 60.0f;
				SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });
				chargeAttackStage = 1;
			}

			timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		case 1:
			position.x += x * 120.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			position.z += z * 120.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

			if (timer > 45.0f)
			{
				timer = 0.0f;
				modelChange = true;
				SetPosition({ position.x, position.y + 8.0f, position.z });
				enumStatus = COOLDOWN;
				SetRotation({ rotation.x - 90.0f, rotation.y, rotation.z });
			}

			timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		default:
			timer = 0.0f;
			enumStatus = STAND;
			break;
		}
		break;
	case TWOENEMYSURROUND:
		if (animationNo != 2)
		{
			timer = 0.0f;
			animationSet = false;
			animationNo = 2;
			modelChange = false;
		}

		switch (twoEnemySurroundStage)
		{
		case 0:
			break;
		case 1:
			x = (objectPosition.x - position.x);
			z = (objectPosition.z - position.z);
			hypotenuse = sqrt((x * x) + (z * z));
			radians = atan2(z, x);
			degrees = XMConvertToDegrees(radians);
			SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });
			break;
		}
		break;
	case BACKATTACK:
		break;
	case JETSTREAMATTACK:
		switch (jetStreamAttackStage)
		{
		case 0:
			if (animationNo != 6)
			{
				timer = 0.0f;
				animationSet = false;
				animationNo = 6;
				modelChange = false;
			}
			
			x = (objectPosition.x - position.x);
			z = (objectPosition.z - position.z);
			hypotenuse = sqrt((x * x) + (z * z));
			radians = atan2(z, x);
			degrees = XMConvertToDegrees(radians);
			SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });

			if (timer == 0.0f)
			{
				if (objectPosition.x >= 0.0f)
				{
					landingAttackPosition.x = (objectPosition.x - 120.0f - (20.0f * (agrooNumber - 1)));
					xQuadrant = 0;
				}
				else if (objectPosition.x < 0.0f)
				{
					landingAttackPosition.x = (objectPosition.x + 120.0f + (20.0f * (agrooNumber - 1)));
					xQuadrant = 1;
				}
				if (objectPosition.z < 0.0f)
				{
					landingAttackPosition.z = (objectPosition.z + 120.0f);
					yQuadrant = 1;
				}
				else if (objectPosition.z >= 0.0f)
				{
					landingAttackPosition.z = (objectPosition.z - 120.0f);
					yQuadrant = 0;
				}
				x2 = (landingAttackPosition.x - position.x);
				y = (landingAttackPosition.z - position.z);
				hypotenuse2 = sqrt((x2 * x2) + (y * y));
			}

			if (currentTime >= endTime && timer > 0.0f /*&& position.x == landingAttackPosition.x && position.z == landingAttackPosition.z*/)
			{
				jetStreamAttackStage = 1;
			}

			timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

			switch (xQuadrant)
			{
			case 0:
				if (position.x <= landingAttackPosition.x)
				{
					position.x = landingAttackPosition.x;
				}
				else
				{
					position.x += 240.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (x2 / hypotenuse2);
				}
				break;
			case 1:
				if (position.x >= landingAttackPosition.x)
				{
					position.x = landingAttackPosition.x;
				}
				else
				{
					position.x += 240.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (x2 / hypotenuse2);
				}
				break;
			}

			switch (yQuadrant)
			{
			case 0:
				if (position.z <= landingAttackPosition.z)
				{
					position.z = landingAttackPosition.z;
				}
				else
				{
					position.z += 240.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (y / hypotenuse2);
				}
				break;
			case 1:
				if (position.z >= landingAttackPosition.z)
				{
					position.z = landingAttackPosition.z;
				}
				else
				{
					position.z += 240.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (y / hypotenuse2);
				}
				break;
			}
			position.y += 5.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		case 1: // From here on it's the same as charge attack, but with a delay between each enemy doing the attack
			if (animationNo != 10)
			{
				animationSet = false;
				animationNo = 10;
				modelChange = false;
				timer = 0.0f;
				SetScale({ scale.x * 0.01f, scale.y * 0.01f, scale.z * 0.01f });
				SetRotation({ rotation.x + 90.0f, rotation.y, rotation.z });
				chargeAttackCheck = true;
			}

			landingAttackPosition = objectPosition;

			x = (landingAttackPosition.x - position.x);
			z = (landingAttackPosition.z - position.z);
			hypotenuse = sqrt((x * x) + (z * z));
			radians = atan2(z, x);
			degrees = XMConvertToDegrees(radians);
			SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });

			if (currentTime >= endTime && timer > (60.0f * agrooNumber))
			{
				timer = 0.0f;
				landingAttackPosition = objectPosition;
				x = (landingAttackPosition.x - position.x);
				z = (landingAttackPosition.z - position.z);
				hypotenuse = sqrt((x * x) + (z * z));
				radians = atan2(z, x);
				degrees = XMConvertToDegrees(radians);
				x = (landingAttackPosition.x - position.x) / 60.0f;
				z = (landingAttackPosition.z - position.z) / 60.0f;
				SetRotation({ GetRotation().x, -degrees + 90.0f, GetRotation().z });
				jetStreamAttackStage = 2;
			}

			timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		case 2:
			position.x += x * 120.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			position.z += z * 120.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

			if (timer > 45.0f)
			{
				timer = 0.0f;
				modelChange = true;
				SetPosition({ position.x, position.y + 8.0f, position.z });
				enumStatus = COOLDOWN;
				SetRotation({ rotation.x - 90.0f, rotation.y, rotation.z });
			}

			timer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			break;
		default:
			timer = 0.0f;
			enumStatus = STAND;
			break;
		}
		break;
	default:
		timer = 0.0f;
		enumStatus = STAND;
		break;
	}

	if (animationNo != 9 && animationNo != 10)
	{
		SetScale({ 0.15f, 0.15f, 0.15f });
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
			endFrame = 443;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 2:
			startFrame = 445;
			endFrame = 474;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 3:
			startFrame = 476;
			endFrame = 623;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 4:
			startFrame = 625;
			endFrame = 729;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 5:
			startFrame = 731;
			endFrame = 946;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 6:
			startFrame = 948;
			endFrame = 1027;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 7:
			startFrame = 1029;
			endFrame = 1189;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 8:
			startFrame = 1191;
			endFrame = 1226;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 9:
			startFrame = 1228;
			endFrame = 1346;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 10:
			startFrame = 1228;
			endFrame = 1281;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		}
	}

	if (position.x >= 398.0f)
	{
		position.x = 398.0f;
	}
	else if (position.x <= -398.0f)
	{
		position.x = -398.0f;
	}
	if (position.z >= 398.0f)
	{
		position.z = 398.0f;
	}
	else if (position.z <= -398.0f)
	{
		position.z = -398.0f;
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

	if (isPlay == false)
	{
		PlayAnimation();
	}

	if (isPlay)
	{
		// Advance one frame
		frameTime.SetTime(0, 0, 1, 0, 0, FbxTime::EMode::eFrames60);
		double sec = frameTime.GetSecondDouble();
		sec *= (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		frameTime.SetSecondDouble(sec);
		currentTime += frameTime;

		// Return to the previous position after playing to the end
		if (currentTime > endTime && repeatAnimation == true)
		{
			currentTime = startTime;
		}
		else if (currentTime > endTime && repeatAnimation == false 
			&& enumStatus != PARTICLEATTACK && enumStatus!= ATTACK)
		{
			currentTime = endTime;
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

	//Debug Start
	/*char msgbuf[256];
	char msgbuf2[256];
	char msgbuf3[256];
	sprintf_s(msgbuf, 256, "X: %f \n", landingAttackPosition.x);
	sprintf_s(msgbuf2, 256, "Y: %f\n", landingAttackPosition.y);
	sprintf_s(msgbuf3, 256, "Z: %f\n", landingAttackPosition.z);
	OutputDebugStringA(msgbuf);
	OutputDebugStringA(msgbuf2);
	OutputDebugStringA(msgbuf3);*/
	//Debug End
}

void EnemyHuman::CreateGraphicsPipeline()
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

void EnemyHuman::Draw(ID3D12GraphicsCommandList* cmdList)
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

void EnemyHuman::Reset()
{
	aggro = false;
	aggroSwitch = false;
	attack = false;
	attackAnimation = false;
	animationSet = false;
	aggroSet = false;
	modelChange = false;
	wander = false;
	ableToDamage = true;
	set = false;
	particleAttackActive = false;
	attackTimer = 0.0f;
	timer = 238.0f;
	particleAttackStage = 0;
	modelChange = true;
	enumStatus = STAND;
}

void EnemyHuman::PlayAnimation()
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