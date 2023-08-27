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

EnemyHuman::EnemyHuman()
{
	
}

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
		if (timer > standTimerMax)
		{
			if (patrolStatus == FRONT)
			{
				newPosition.x = rand() % 401 - 150 + homePosition.x;
				newPosition.z = rand() % 401 - 150 + homePosition.y;
				frontPatrolPosition.x = newPosition.x;
				frontPatrolPosition.z = newPosition.z;
			}
			else
			{
				newPosition.x = frontPatrolPosition.x - 10.0f;
				newPosition.z = frontPatrolPosition.z;
			}
			timer = timerReset;
			enumStatus = status::WANDER;
		}

		if (!FirstRun)
		{
			if (slowMotion)
			{
				timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			}
		}
		else
		{
			FirstRun = false;
		}

		break;
	case WANDER:
		if (!set)
		{
			x = (newPosition.x - position.x) / wanderTime;
			y = (newPosition.z - position.z) / wanderTime;
			float x2 = newPosition.x - position.x;
			float y2 = newPosition.z - position.z;
			float radians = atan2(y2, x2);
			degrees = XMConvertToDegrees(radians);
			set = true;
			modelChange = true;
			if (modelChange)
			{
				animationSet = false;
				animationNo = Animation::WANDERANIMATION;
				modelChange = false;
			}
		}
		else
		{
			if (timer > timerSixSeconds)
			{
				timer = timerReset;
				wander = false;
				set = false;
				modelChange = true;
				if (modelChange)
				{
					animationSet = false;
					animationNo = Animation::STANDANIMATION;
					modelChange = false;
				}
				enumStatus = status::STAND;
			}
			else
			{
				if (slowMotion)
				{
					timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
				}
				else
				{
					timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
				}
			}

			if (!FirstRun)
			{
				if (slowMotion)
				{
					position.x += (x * wanderSpeed) * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
					position.z += (y * wanderSpeed) * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
				}
				else
				{
					position.x += x * wanderSpeed * deltaTime->DeltaTimeDividedByMiliseconds();
					position.z += y * wanderSpeed * deltaTime->DeltaTimeDividedByMiliseconds();
				}
			}
			else
			{
				FirstRun = false;
			}
			SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });
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
				animationNo = Animation::AGGROANIMATION;
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
				if (slowMotion)
				{
					position.x += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
					position.z += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse);
				}
				else
				{
					position.x += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
					position.z += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse);
				}
			}
			else
			{
				FirstRun = false;
			}
			SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });
			SetPosition(position);
			if (aggroSwitch)
			{
				animationSet = false;
				animationNo = Animation::STANDANIMATION;
				aggroSet = false;
				aggro = false;
				wander = false;
				set = false;
				timer = timerReset;
				aggroSwitch = false;
				enumStatus = status::STAND;
			}
		}
		break;
	case ATTACK:
		if (!attackAnimation)
		{
			attackTimer = timerReset;
			animationSet = false;
			animationNo = Animation::BASICATTACKANIMATION;
			attackAnimation = true;
		}

		if (attackTimer < normalAttackTimerStart || attackTimer > normalAttackTimerEnd)
		{
			if (slowMotion)
			{
				frameSpeed = POINTTWOFIVE;
			}
			else
			{
				frameSpeed = POINTSEVENFIVE;
			}
		}
		else
		{
			if (slowMotion)
			{
				frameSpeed = POINTTWOFIVE;
			}
			else
			{
				frameSpeed = NORMAL;
			}
		}

		if (attackTimer > normalAttackCanDamageStart && attackTimer < normalAttackCanDamageEnd)
		{
			attackDamagePossible = true;
		}
		else
		{
			attackDamagePossible = false;
		}

		if (currentTime >= endTime && attackTimer > timerReset)
		{
			attackTimer = timerReset;
			modelChange = true;
			attackAnimation = false;
			timer = timerReset;
			enumStatus = COOLDOWN;
		}

		switch (frameSpeed)
		{
		case NORMAL:
			attackTimer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			break;
		case HALF:
			attackTimer += timerHalfSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			break;
		case DOUBLE:
			attackTimer += timerTwoSeconds * deltaTime->DeltaTimeDividedByMiliseconds();
			break;
		case ONEPOINTFIVE:
			attackTimer += timerSecondAndAHalf * deltaTime->DeltaTimeDividedByMiliseconds();
			break;
		case POINTSEVENFIVE:
			attackTimer += timerThreeQuartersSeconds * deltaTime->DeltaTimeDividedByMiliseconds();
			break;
		case POINTTWOFIVE:
			attackTimer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			break;
		default:
			attackTimer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			break;
		}
		break;
	case COOLDOWN:
		if (modelChange)
		{
			animationSet = false;
			animationNo = Animation::STANDANIMATION;
			cooldownPosition = position;
			modelChange = false;
		}
		SetPosition(cooldownPosition);
		if (slowMotion)
		{
			timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
		}
		else
		{
			timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
		}
		if (timer > timerOneThirdSecond)
		{
			timer = timerReset;
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
			animationNo = Animation::DAMAGEANIMATION;
			modelChange = false;
		}
		if (slowMotion)
		{
			timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
		}
		else
		{
			timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
		}
		if (timer > maxDamagedTime)
		{
			timer = timerReset;
			aggroSet = false;
			modelChange = true;
			attackAnimation = false;
			enumStatus = AGGRO;
		}
		break;
	case DEAD:
		if (modelChange)
		{
			animationSet = false;
			animationNo = Animation::DEATHANIMATION;
			modelChange = false;
		}
		break;
	case PARTICLEATTACK:
		switch (particleAttackStage)
		{
		case 0:
			if (modelChange)
			{
				timer = timerReset;
				animationSet = false;
				animationNo = Animation::JUMPANIMATION;
				modelChange = false;
			}
			x = objectPosition.x - position.x;
			y = objectPosition.z - position.z;
			hypotenuse = sqrt((x * x) + (y * y));
			radians = atan2(y, x);
			degrees = XMConvertToDegrees(radians);
			if (slowMotion)
			{
				position.x -= particleAttackJumpBackSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
				position.z -= particleAttackJumpBackSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse);
			}
			else
			{
				position.x -= particleAttackJumpBackSpeed * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
				position.z -= particleAttackJumpBackSpeed * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse);
			}
			SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });
			if (currentTime < endTime / 2 && timer > timerReset)
			{
				if (slowMotion)
				{
					position.y += 1.0f * slowMotionMultiplier;
				}
				else
				{
					position.y += 1.0f;
				}
			}
			SetPosition(position);
			if (currentTime > endTime && timer > timerReset)
			{
				particleAttackStage = 1;
				modelChange = true;
			}
			if (slowMotion)
			{
				timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			break;
		case 1:
			if (modelChange)
			{
				timer = timerReset;
				particleAttackPosition = { position.x, position.y + 8.0f, position.z };
				animationSet = false;
				animationNo = Animation::PARTICLEATTACKANIMATION;
				modelChange = false;
			}
			if (currentTime - startTime < (endTime - startTime) / 2 && !particleAttackActive)
			{
				if (slowMotion)
				{
					frameSpeed = POINTTWOFIVE;
				}
				else
				{
					frameSpeed = POINTSEVENFIVE;
				}
			}
			else
			{
				if (slowMotion)
				{
					frameSpeed = POINTTWOFIVE;
				}
				else
				{
					frameSpeed = NORMAL;
				}
			}
			if (currentTime - startTime > (endTime - startTime) / 2 && timer > timerReset && !particleAttackActive)
			{
				particleAttackActive = true;
				timer = timerReset;
			}
			if (particleAttackActive)
			{
				if (slowMotion)
				{
					particleAttackPosition.x += timerThreeSeconds * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
					particleAttackPosition.y += timerThreeSeconds * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse);
					particleAttackPosition.z += timerThreeSeconds * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (z / hypotenuse);
					timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
				}
				else
				{
					particleAttackPosition.x += timerThreeSeconds * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
					particleAttackPosition.y += timerThreeSeconds * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse);
					particleAttackPosition.z += timerThreeSeconds * deltaTime->DeltaTimeDividedByMiliseconds() * (z / hypotenuse);
					timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
				}
			}
			else
			{
				x = objectPosition.x - position.x;
				y = objectPosition.y - position.y;
				z = objectPosition.z - position.z;
				hypotenuse = sqrt((x * x) + (y * y) + (z * z));
				radians = atan2(z, x);
				degrees = XMConvertToDegrees(radians);
				SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });
				if (slowMotion)
				{
					timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
				}
				else
				{
					timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
				}
			}
			if (currentTime - startTime > (endTime - startTime) && particleAttackActive)
			{
				ableToDamage = true;
				timer = timerReset;
				particleAttackActive = false;
				particleAttackStage = jetStreamAttackStageReset;
				modelChange = true;
				aggroSet = false;
				enumStatus = AGGRO;
			}
			break;
		}
		break;
	case FLEE:
		if (animationNo != Animation::FLEEANIMATION)
		{
			animationSet = false;
			animationNo = Animation::FLEEANIMATION;
			modelChange = false;
		}
		break;
	case LANDINGATTACK:
		switch (landingAttackStage)
		{
		case 0:
			if (animationNo != Animation::JUMPANIMATION)
			{
				animationSet = false;
				animationNo = Animation::JUMPANIMATION;
				modelChange = false;
			}

			if (position.y < landingAttackYMax)
			{
				if (slowMotion)
				{
					position.y += landingAttackStage0Speed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
				}
				else
				{
					position.y += landingAttackStage0Speed * deltaTime->DeltaTimeDividedByMiliseconds();
				}
				landingAttackPosition = objectPosition;
			}
			else
			{
				position.y = landingAttackYMax;
				x = (landingAttackPosition.x - position.x);
				y = (landingAttackPosition.y - position.y);
				z = (landingAttackPosition.z - position.z);
				hypotenuse = sqrt((x * x) + (z * z));
				radians = atan2(z, x);
				degrees = XMConvertToDegrees(radians);
				x = (landingAttackPosition.x - position.x) / landingAttackStage0SpeedHypotenuse;
				y = (landingAttackPosition.y - position.y) / landingAttackStage0SpeedHypotenuse;
				z = (landingAttackPosition.z - position.z) / landingAttackStage0SpeedHypotenuse;
				SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });
				landingAttackStage = 1;
			}
			break;
		case 1:
			if (position.y > landingAttackPosition.y)
			{
				if (animationNo != Animation::JUMPANIMATION)
				{
					animationSet = false;
					animationNo = Animation::JUMPANIMATION;
					modelChange = false;
					landed = true;
				}

				if (slowMotion)
				{
					position.x += x * landingAttackStage1XZSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
					position.y -= landingAttackStage1YSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
					position.z += z * landingAttackStage1XZSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
				}
				else
				{
					position.x += x * landingAttackStage1XZSpeed * deltaTime->DeltaTimeDividedByMiliseconds();
					position.y -= landingAttackStage1YSpeed * deltaTime->DeltaTimeDividedByMiliseconds();
					position.z += z * landingAttackStage1XZSpeed * deltaTime->DeltaTimeDividedByMiliseconds();
				}
			}
			else
			{
				if (animationNo != Animation::LANDINGANIMATION)
				{
					animationSet = false;
					animationNo = Animation::LANDINGANIMATION;
					modelChange = false;
					if (position.y <= landingAttackPosition.y)
					{
						position.y = landingAttackPosition.y;
					}
					landed = false;
					landingParticles = true;
					timer = timerReset;
					SetScale({ scale.x * landingAttackScale, scale.y * landingAttackScale, scale.z * landingAttackScale });
					SetRotation({ rotation.x + yRotationOffset, rotation.y, rotation.z });
				}

				if (currentTime >= endTime && timer > timerReset)
				{
					timer = timerReset;
					SetPosition({ position.x, position.y + landingAttackYOffset, position.z });
					modelChange = true;
					enumStatus = COOLDOWN;
					landed = false;
					SetRotation({ rotation.x - yRotationOffset, rotation.y, rotation.z });
				}

				timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			break;
		}
		break;
	case CHARGEATTACK:
		switch (chargeAttackStage)
		{
		case 0:
			if (animationNo != Animation::CHARGEANIMATION)
			{
				animationSet = false;
				animationNo = Animation::CHARGEANIMATION;
				modelChange = false;
				timer = timerReset;
				SetScale({ scale.x * chargeAttackScale, scale.y * chargeAttackScale, scale.z * chargeAttackScale });
				SetRotation({ rotation.x + yRotationOffset, rotation.y, rotation.z });
				chargeAttackCheck = true;
				frameSpeed = POINTSEVENFIVE;
			}

			landingAttackPosition = objectPosition;

			x = (landingAttackPosition.x - position.x);
			z = (landingAttackPosition.z - position.z);
			hypotenuse = sqrt((x * x) + (z * z));
			radians = atan2(z, x);
			degrees = XMConvertToDegrees(radians);
			SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });

			if (currentTime >= endTime && timer > timerReset) // Timer is 53.0f
			{
				timer = timerReset;
				landingAttackPosition = objectPosition;
				x = (landingAttackPosition.x - position.x);
				z = (landingAttackPosition.z - position.z);
				hypotenuse = sqrt((x * x) + (z * z));
				radians = atan2(z, x);
				degrees = XMConvertToDegrees(radians);
				SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });
				frameSpeed = NORMAL;
				chargeAttackStage = 1;
			}

			if (slowMotion)
			{
				timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			break;
		case 1:
			if (slowMotion)
			{
				position.x += chargeAttackSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
				position.z += chargeAttackSpeed * slowMotionMultiplier *deltaTime->DeltaTimeDividedByMiliseconds()* (z / hypotenuse);
			}
			else
			{
				position.x += chargeAttackSpeed * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
				position.z += chargeAttackSpeed * deltaTime->DeltaTimeDividedByMiliseconds() * (z / hypotenuse);
			}

			if (timer > chargeAttackTimerMax)
			{
				timer = timerReset;
				modelChange = true;
				SetPosition({ position.x, position.y + chargeAttackYOffset, position.z });
				enumStatus = COOLDOWN;
				SetRotation({ rotation.x - yRotationOffset, rotation.y, rotation.z });
			}

			if (slowMotion)
			{
				timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			break;
		default:
			timer = timerReset;
			enumStatus = status::STAND;
			break;
		}
		break;
	case TWOENEMYSURROUND:
		if (animationNo != Animation::AGGROANIMATION)
		{
			timer = timerReset;
			animationSet = false;
			animationNo = Animation::AGGROANIMATION;
			modelChange = false;
		}

		switch (twoEnemySurroundStage)
		{
		case 0:
			if (timer == timerReset)
			{
				meetingPoint = meetingPointReset;
				meetingPoint.x = objectPosition.x + cos(objectRotation.y - twoEnemySurroundYRotOffset) * twoEnemySurroundCircleRadius;
				meetingPoint.z = objectPosition.z + sin(objectRotation.y - twoEnemySurroundYRotOffset) * twoEnemySurroundCircleRadius;
				x = (zeroDegrees - position.x);
				if (patrolStatus == FRONT)
				{
					z = (10.0f - position.z);
				}
				else
				{
					z = (-10.0f - position.z);
				}
				hypotenuse = sqrtf((x * x) + (z * z));
				radians = atan2(z, x);
				degrees = XMConvertToDegrees(radians);
				SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });
			}

			if (slowMotion)
			{
				position.x += twoEnemySurroundSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
				position.z += twoEnemySurroundSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (z / hypotenuse);
			}
			else
			{
				position.x += twoEnemySurroundSpeed * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
				position.z += twoEnemySurroundSpeed * deltaTime->DeltaTimeDividedByMiliseconds() * (z / hypotenuse);
			}
			SetPosition(position);

			if (timer >= twoEnemySurroundStage1TimerMax)
			{
				timer = timerReset;
				nextPosition = nextPositionReset;
				midpoint.x = (objectPosition.x + position.x) / half;
				midpoint.y = (objectPosition.z + position.z) / half;
				circleRadius.x = (abs(objectPosition.x) + abs(position.x)) / half;
				circleRadius.y = (abs(objectPosition.z) + abs(position.z)) / half;
				initialDegree = nextDegree = radians;
				distanceFloat = sqrt((position.x - objectPosition.x) * (position.x - objectPosition.x) + (position.z - objectPosition.z) * (position.z - objectPosition.z));
				origDistanceFloat = distanceFloat;
				nextDegreeAngle = nextDegreeAngleReset;
				surroundSpeed = surroundSpeedReset;
				twoEnemySurroundStage = 1;
				break;
			}

			if (slowMotion)
			{
				timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			break;
		case 1:
			if (patrolStatus == FRONT)
			{
				if (nextDegree < (initialDegree + XMConvertToRadians(twoEnemySurroundCase1RotOffset)))
				{
					midpoint.x = (objectPosition.x + position.x) / half;
					midpoint.y = (objectPosition.z + position.z) / half;
					circleRadius.x = (abs(objectPosition.x) + abs(position.x)) / half;
					circleRadius.y = (abs(objectPosition.z) + abs(position.z)) / half;
					distanceFloat = sqrt((position.x - objectPosition.x) * (position.x - objectPosition.x) + (position.z - objectPosition.z) * (position.z - objectPosition.z));
				}
			}
			else
			{
				if (nextDegree > (initialDegree - XMConvertToRadians(twoEnemySurroundCase1RotOffset)))
				{
					midpoint.x = (objectPosition.x + position.x) / half;
					midpoint.y = (objectPosition.z + position.z) / half;
					circleRadius.x = (abs(objectPosition.x) + abs(position.x)) / half;
					circleRadius.y = (abs(objectPosition.z) + abs(position.z)) / half;
					distanceFloat = sqrt((position.x - objectPosition.x) * (position.x - objectPosition.x) + (position.z - objectPosition.z) * (position.z - objectPosition.z));
				}
			}
			nextPosition.x = midpoint.x + cosf(nextDegree) * (distanceFloat);
			nextPosition.y = midpoint.y + sinf(nextDegree) * (distanceFloat);
			x = (nextPosition.x - position.x);
			z = (nextPosition.y - position.z);
			hypotenuse = sqrtf((x * x) + (z * z));
			radians = atan2(z, x);
			degrees = XMConvertToDegrees(radians);
			SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });
			if (slowMotion)
			{
				position.x += surroundSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
				position.z += surroundSpeed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (z / hypotenuse);
			}
			else
			{
				position.x += surroundSpeed * deltaTime->DeltaTimeDividedByMiliseconds() * (x / hypotenuse);
				position.z += surroundSpeed * deltaTime->DeltaTimeDividedByMiliseconds() * (z / hypotenuse);
			}
			SetPosition(position);

			if (patrolStatus == FRONT)
			{
				if (nextDegree > (initialDegree + XMConvertToRadians(90.0f)))
				{
					if (surroundSpeed < oneEightyDegrees)
					{
						if (slowMotion)
						{
							surroundSpeed += case1SurroundSpeedDefault * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
						}
						else
						{
							surroundSpeed += case1SurroundSpeedDefault * deltaTime->DeltaTimeDividedByMiliseconds();
						}
					}
					else
					{
						if (slowMotion)
						{
							surroundSpeed = case1SurroundSpeedMax * slowMotionMultiplier;
						}
						else
						{
							surroundSpeed = case1SurroundSpeedMax;
						}
					}
				}

				if (nextDegree < (initialDegree + XMConvertToRadians(290.0f)))
				{
					if (slowMotion)
					{
						nextDegree += XMConvertToRadians(100.0f * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds());
					}
					else
					{
						nextDegree += XMConvertToRadians(100.0f * deltaTime->DeltaTimeDividedByMiliseconds());
					}
				}
				else
				{
					nextDegree = initialDegree + XMConvertToRadians(290.0f);
					timer = timerReset;
					modelChange = true;
					enumStatus = COOLDOWN;
					break;
				}
			}
			else
			{
				if (nextDegree < (initialDegree - XMConvertToRadians(90.0f)))
				{
					if (surroundSpeed < oneEightyDegrees)
					{
						if (slowMotion)
						{
							surroundSpeed += case1SurroundSpeedDefault * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
						}
						else
						{
							surroundSpeed += case1SurroundSpeedDefault * deltaTime->DeltaTimeDividedByMiliseconds();
						}
					}
					else
					{
						if (slowMotion)
						{
							surroundSpeed = case1SurroundSpeedMax * slowMotionMultiplier;
						}
						else
						{
							surroundSpeed = case1SurroundSpeedMax;
						}
					}
				}

				if (nextDegree > (initialDegree - XMConvertToRadians(290.0f)))
				{
					if (slowMotion)
					{
						nextDegree -= XMConvertToRadians(100.0f * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds());
					}
					else
					{
						nextDegree -= XMConvertToRadians(100.0f * deltaTime->DeltaTimeDividedByMiliseconds());
					}
				}
				else
				{
					nextDegree = initialDegree - XMConvertToRadians(290.0f);
					timer = timerReset;
					modelChange = true;
					enumStatus = COOLDOWN;
					break;
				}
			}
			
			break;
		}
		break;
	case BACKATTACK:
		break;
	case JETSTREAMATTACK:
		switch (jetStreamAttackStage)
		{
		case 0:
			if (animationNo != Animation::JUMPANIMATION)
			{
				timer = timerReset;
				animationSet = false;
				animationNo = Animation::JUMPANIMATION;
				modelChange = false;
			}
			
			x = (objectPosition.x - position.x);
			z = (objectPosition.z - position.z);
			hypotenuse = sqrt((x * x) + (z * z));
			radians = atan2(z, x);
			degrees = XMConvertToDegrees(radians);
			SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });

			if (timer == timerReset)
			{
				if (objectPosition.x >= objectPositionResetJetStreamAttack)
				{
					landingAttackPosition.x = (objectPosition.x - 120.0f - (20.0f * (agrooNumber - 1)));
					xQuadrant = 0;
				}
				else if (objectPosition.x < objectPositionResetJetStreamAttack)
				{
					landingAttackPosition.x = (objectPosition.x + 120.0f + (20.0f * (agrooNumber - 1)));
					xQuadrant = 1;
				}
				if (objectPosition.z < objectPositionResetJetStreamAttack)
				{
					landingAttackPosition.z = (objectPosition.z + 120.0f);
					yQuadrant = 1;
				}
				else if (objectPosition.z >= objectPositionResetJetStreamAttack)
				{
					landingAttackPosition.z = (objectPosition.z - 120.0f);
					yQuadrant = 0;
				}
				x2 = (landingAttackPosition.x - position.x);
				y = (landingAttackPosition.z - position.z);
				hypotenuse2 = sqrt((x2 * x2) + (y * y));
			}

			if (currentTime >= endTime && timer > timerReset /*&& position.x == landingAttackPosition.x && position.z == landingAttackPosition.z*/)
			{
				jetStreamAttackStage = 1;
			}

			if (slowMotion)
			{
				timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			}

			switch (xQuadrant)
			{
			case 0:
				if (position.x <= landingAttackPosition.x)
				{
					position.x = landingAttackPosition.x;
				}
				else
				{
					if (slowMotion)
					{
						position.x += jetStreamAttackSpeedStage0 * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (x2 / hypotenuse2);
					}
					else
					{
						position.x += jetStreamAttackSpeedStage0 * deltaTime->DeltaTimeDividedByMiliseconds() * (x2 / hypotenuse2);
					}
				}
				break;
			case 1:
				if (position.x >= landingAttackPosition.x)
				{
					position.x = landingAttackPosition.x;
				}
				else
				{
					if (slowMotion)
					{
						position.x += jetStreamAttackSpeedStage0 * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (x2 / hypotenuse2);
					}
					else
					{
						position.x += jetStreamAttackSpeedStage0 * deltaTime->DeltaTimeDividedByMiliseconds() * (x2 / hypotenuse2);
					}
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
					if (slowMotion)
					{
						position.z += jetStreamAttackSpeedStage0 * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse2);
					}
					else
					{
						position.z += jetStreamAttackSpeedStage0 * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse2);
					}
				}
				break;
			case 1:
				if (position.z >= landingAttackPosition.z)
				{
					position.z = landingAttackPosition.z;
				}
				else
				{
					if (slowMotion)
					{
						position.z += jetStreamAttackSpeedStage0 * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse2);
					}
					else
					{
						position.z += jetStreamAttackSpeedStage0 * deltaTime->DeltaTimeDividedByMiliseconds() * (y / hypotenuse2);
					}
				}
				break;
			}
			if (slowMotion)
			{
				position.y += 5.0f * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				position.y += 5.0f * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			break;
		case 1: // From here on it's the same as charge attack, but with a delay between each enemy doing the attack
			if (animationNo != Animation::CHARGEANIMATION)
			{
				animationSet = false;
				animationNo = Animation::CHARGEANIMATION;
				modelChange = false;
				timer = timerReset;
				SetScale({ scale.x * landingAttackScale, scale.y * landingAttackScale, scale.z * landingAttackScale });
				SetRotation({ rotation.x + yRotationOffset, rotation.y, rotation.z });
				chargeAttackCheck = true;
			}

			landingAttackPosition = objectPosition;

			x = (landingAttackPosition.x - position.x);
			z = (landingAttackPosition.z - position.z);
			hypotenuse = sqrt((x * x) + (z * z));
			radians = atan2(z, x);
			degrees = XMConvertToDegrees(radians);
			SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });

			if (currentTime >= endTime && timer > (timerOneSecond * agrooNumber))
			{
				timer = timerReset;
				landingAttackPosition = objectPosition;
				x = (landingAttackPosition.x - position.x);
				z = (landingAttackPosition.z - position.z);
				hypotenuse = sqrt((x * x) + (z * z));
				radians = atan2(z, x);
				degrees = XMConvertToDegrees(radians);
				x = (landingAttackPosition.x - position.x) / timerOneSecond;
				z = (landingAttackPosition.z - position.z) / timerOneSecond;
				SetRotation({ GetRotation().x, -degrees + yRotationOffset, GetRotation().z });
				jetStreamAttackStage = 2;
			}

			if (slowMotion)
			{
				timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			break;
		case 2:
			if (slowMotion)
			{
				position.x += x * jetStreamAttackCase1Speed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
				position.z += z * jetStreamAttackCase1Speed * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				position.x += x * jetStreamAttackCase1Speed * deltaTime->DeltaTimeDividedByMiliseconds();
				position.z += z * jetStreamAttackCase1Speed * deltaTime->DeltaTimeDividedByMiliseconds();
			}

			if (timer > 45.0f)
			{
				timer = timerReset;
				modelChange = true;
				SetPosition({ position.x, position.y + 8.0f, position.z });
				enumStatus = COOLDOWN;
				SetRotation({ rotation.x - yRotationOffset, rotation.y, rotation.z });
			}

			if (slowMotion)
			{
				timer += timerOneSecond * slowMotionMultiplier * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			else
			{
				timer += timerOneSecond * deltaTime->DeltaTimeDividedByMiliseconds();
			}
			break;
		default:
			timer = timerReset;
			enumStatus = status::STAND;
			break;
		}
		break;
	default:
		timer = timerReset;
		enumStatus = status::STAND;
		break;
	}

	if (animationNo != Animation::LANDINGANIMATION && animationNo != Animation::CHARGEANIMATION)
	{
		SetScale({ scaleReset, scaleReset, scaleReset });
		if (rotation.x != initialDegree)
		{
			rotation.x = initialDegree;
		}
	}

	if (animationNo == Animation::LANDINGANIMATION || animationNo == Animation::CHARGEANIMATION)
	{
		SetScale({ scaleReset * landingAttackScale, scaleReset * landingAttackScale, scaleReset * landingAttackScale });
		if (rotation.x != yRotationOffset)
		{
			rotation.x = yRotationOffset;
		}
	}

	if (!animationSet)
	{
		switch (animationNo)
		{
		case 0:
			startFrame = standingStartFrame;
			endFrame = standingEndFrame;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 1:
			startFrame = wanderStartFrame;
			endFrame = wanderEndFrame;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 2:
			startFrame = aggroStartFrame;
			endFrame = aggroEndFrame;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 3:
			startFrame = basicAttackStartFrame;
			endFrame = basicAttackEndFrame;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 4:
			startFrame = damageStartFrame;
			endFrame = damageEndFrame;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 5:
			startFrame = deathStartFrame;
			endFrame = deathEndFrame;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 6:
			startFrame = jumpStartFrame;
			endFrame = jumpEndFrame;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 7:
			startFrame = particleAttackStartFrame;
			endFrame = particleAttackEndFrame;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 8:
			startFrame = fleeStartFrame;
			endFrame = fleeEndFrame;
			repeatAnimation = true;
			isPlay = false;
			animationSet = true;
			break;
		case 9:
			startFrame = landingStartFrame;
			endFrame = landingEndFrame;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		case 10:
			startFrame = chargeStartFrame;
			endFrame = chargeEndFrame;
			repeatAnimation = false;
			isPlay = false;
			animationSet = true;
			break;
		}
	}

	if (position.x >= mapEdgeBoundary)
	{
		position.x = mapEdgeBoundary;
	}
	else if (position.x <= -mapEdgeBoundary)
	{
		position.x = -mapEdgeBoundary;
	}
	if (position.z >= mapEdgeBoundary)
	{
		position.z = mapEdgeBoundary;
	}
	else if (position.z <= -mapEdgeBoundary)
	{
		position.z = -mapEdgeBoundary;
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

	if (slowMotion)
	{
		frameSpeed = POINTTWOFIVE;
	}

	if (isPlay)
	{
		// Advance one frame
		frameTime.SetTime(0, 0, 1, 0, 0, FbxTime::EMode::eFrames60);
		double sec = frameTime.GetSecondDouble();
		switch (frameSpeed)
		{
		case NORMAL:
			break;
		case HALF:
			sec *= slowMotionHalfMultiplier;
			break;
		case DOUBLE:
			sec *= slowMotionDoubleMultiplier;
			break;
		case ONEPOINTFIVE:
			sec *= slowMotionOneAndHalfMultiplier;
			break;
		case POINTSEVENFIVE:
			sec *= slowMotionThreeQuartersMultiplier;
			break;
		case POINTTWOFIVE:
			sec *= slowMotionMultiplier;
			break;
		default:
			break;
		}
		sec *= deltaTime->DeltaTimeDividedByMiliseconds();
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

	if (!slowMotion)
	{
		frameSpeed = NORMAL;
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
	//char msgbuf[256];
	//char msgbuf2[256];
	//char msgbuf3[256];
	//sprintf_s(msgbuf, 256, "X: %f \n", timer);
	//sprintf_s(msgbuf2, 256, "Y: %f\n", landingAttackPosition.y);
	//sprintf_s(msgbuf3, 256, "Z: %f\n", landingAttackPosition.z);
	//OutputDebugStringA(msgbuf);
	//OutputDebugStringA(msgbuf2);
	//OutputDebugStringA(msgbuf3);
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
	attackTimer = timerReset;
	timer = 238.0f;
	particleAttackStage = 0;
	modelChange = true;
	enumStatus = status::STAND;
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

bool EnemyHuman::CheckCollisionWithEnemy(XMFLOAT3 enemyPos, float collisionRadius)
{
	float dx = enemyPos.x - position.x;
	float dy = enemyPos.y - position.y;
	float dz = enemyPos.z - position.z;

	float distance = sqrtf(powf(dx, 2) + powf(dy, 2) + powf(dz, 2));

	return distance <= 15.0f + collisionRadius; // replace 15.0f with playerCollisionRadius
}

void EnemyHuman::MoveTowards(float& current, float target, float speed, float elapsedTime)
{
	float delta = target - current;
	float step = speed * elapsedTime;
	if (step > abs(delta))
	{
		current = target;
	}
	else
	{
		current += step * (delta < 0 ? -1 : 1); // Need to adjust this to delta time
	}
}
