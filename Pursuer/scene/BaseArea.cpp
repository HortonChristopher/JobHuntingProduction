#include "BaseArea.h"

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;
extern float loadingProgress;
extern std::atomic<float> loadingPercent;

extern int keyOrMouse;
extern float degreeTransfer = 0.0f;
extern bool lockOnActive = false;

extern int agroodEnemies = 0;
extern int debugJetStream = 0;
extern int participatingJetStream = 0;

BaseArea::BaseArea()
{
}

BaseArea::~BaseArea()
{
	// Safe Delete
	safe_delete(camera);
	collisionManager = nullptr;
	particleMan = nullptr;
	debugText = nullptr;
	safe_delete(HPBarSPRITE);
	safe_delete(HPBarFrameSPRITE);
	safe_delete(STBarSPRITE);
	safe_delete(STBarFrameSPRITE);
	safe_delete(baseAreaMissionSPRITE);
	safe_delete(skydomeMODEL);
	//safe_delete(groundMODEL);
	//safe_delete(extendedGroundMODEL);
	//safe_delete(positionMODEL);
	//safe_delete(attackRangeMODEL);
	//safe_delete(visionRangeMODEL);
	safe_delete(skydomeOBJ);
	for (int i = 0; i < 11; i++) { safe_delete(attackRangeOBJ[i]); }
	for (int i = 0; i < 10; i++) { safe_delete(enemyVisionRangeOBJ[i]); }
	safe_delete(playerPositionOBJ);
	for (int i = 0; i < 10; i++) { safe_delete(baseAreaEnemyPositionOBJ[i]); }
	safe_delete(groundOBJ);
	safe_delete(extendedGroundOBJ);
	safe_delete(playerFBX);
	for (int i = 0; i < 10; i++) { safe_delete(baseAreaEnemyFBX[i]); }
	safe_delete(baseAreaMinimapSPRITE);
	safe_delete(baseAreaMinimapPlayerSPRITE);
	for (int i = 0; i < 10; i++) { safe_delete(baseAreaMinimapEnemySPRITE[i]); }
	for (int i = 0; i < 10; i++) { safe_delete(baseAreaEnemyHPBarSPRITE[i]); }
	for (int i = 0; i < 10; i++) { safe_delete(baseAreaEnemyHPBarFrameSPRITE[i]); }
	safe_delete(baseAreaDamageOverlaySPRITE);
	audio->StopWave("MainArea.wav");
}

void BaseArea::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// Checking for nullptr
	assert(dxCommon);
	assert(input);
	assert(audio);

	// Assigning variables to this
	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	audio->PlayWave("MainArea.wav", audio->gameplayVolume, true);

	// Camera initialization
	camera = new DebugCamera(WinApp::window_width, WinApp::window_height, input);

	// Collision Manager initialization
	collisionManager = CollisionManager::GetInstance();

	// Particle Manager initialization/generation
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	// Light Group Creation
	lightGroup = LightGroup::Create();

	// Setting DxCommon device
	FBXGeneration::SetDevice(dxCommon->GetDevice());
	EnemyHuman::SetDevice(dxCommon->GetDevice());
	Player::SetDevice(dxCommon->GetDevice());

	// Setting camera
	Object3d::SetCamera(camera);
	FBXGeneration::SetCamera(camera);
	EnemyHuman::SetCamera(camera);
	Player::SetCamera(camera);
	PlayerPositionObject::SetCamera(camera);

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);

	std::thread th1(&BaseArea::thread1, this); // 2D Initialization
	std::thread th2(&BaseArea::thread2, this); // 3D Initialization (other than touchable objects)
	std::thread th3(&BaseArea::thread3, this); // Model and Touchable Object Initialization
	th1.join();
	th2.join();
	th3.join();

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (i % 2 == 0 || i == 0)
		{
			baseAreaEnemyFBX[i]->SetPatrolStatus(EnemyHuman::FRONT);
		}
		else
		{
			baseAreaEnemyFBX[i]->SetPatrolStatus(EnemyHuman::BACK);
		}
	}

	initializeFinished = true;
}

void BaseArea::Update()
{
	lightGroup->Update();
	particleMan->Update();
	
	// Debug Text string
	std::ostringstream missionTracker;
	std::ostringstream healTracker;

	if (!screenShake)
	{
		if (!lockOnActive && !playerFBX->dodge)
		{
			camera->SetTarget(playerPositionOBJ->GetPosition());
		}
		else if (playerFBX->dodge)
		{
			camera->SetTarget(playerFBX->dodgePosition);
		}
		camera->Update();
	}

#pragma region openingCutscene
	if (!gameStart && initializeFinished == true)
	{
		if (startTimer >= startTimerLimit)
		{
			fadeSpriteALPHA -= fadeSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

			fadeSPRITE->SetColor({ 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });

			if (fadeSpriteALPHA <= 0.0f)
			{
				fadeSpriteALPHA = 0.0f;
				fadeSPRITE->SetColor({ 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
				gameStart = true;
			}
		}
		else
		{
			startTimer += startTimerIntegral * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}
	}

	if (playerFBX->baseAreaOpeningCutscene && initializeFinished == true && startTimer > startTimerLimit)
	{
		playerFBX->SetEnumStatus(Player::WALK);
		playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, playerFBX->GetPosition().z + 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) });
		if (playerFBX->GetPosition().z >= -movementStartZPosition)
		{
			startMissionSpriteMovement = true;
		}
		if (playerFBX->GetPosition().z >= -mapBorder)
		{
			playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, -398.0f });
			playerFBX->baseAreaOpeningCutscene = false;
		}
	}
#pragma endregion

	objectPosition = playerFBX->GetPosition();
	objectRotation = playerFBX->GetRotation();

	skydomeOBJ->SetPosition(objectPosition);

#pragma region LockOn
	//if (input->PushKey(DIK_SPACE) || input->PushControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	//{
	//	if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	//	{
	//		float calculatedDegrees;
	//		if (48.0f >= camera->GetEye().z - playerFBX->GetPosition().z && camera->GetEye().z - playerFBX->GetPosition().z > 0.0f
	//			&& 48.0f > camera->GetEye().x - playerFBX->GetPosition().x && camera->GetEye().x - playerFBX->GetPosition().x >= 0.0f)
	//		{
	//			calculatedDegrees = (camera->GetEye().x - playerFBX->GetPosition().x) * 1.875f;
	//		}
	//		else if (-48.0f < camera->GetEye().z - playerFBX->GetPosition().z && camera->GetEye().z - playerFBX->GetPosition().z <= 0.0f
	//			&& 48.0f >= camera->GetEye().x - playerFBX->GetPosition().x && camera->GetEye().x - playerFBX->GetPosition().x > 0.0f)
	//		{
	//			calculatedDegrees = -(camera->GetEye().z - playerFBX->GetPosition().z) * 1.875f + 90.0f;
	//		}
	//		else if (-48.0f <= camera->GetEye().z - playerFBX->GetPosition().z && camera->GetEye().z - playerFBX->GetPosition().z < 0.0f
	//			&& 0.0f >= camera->GetEye().x - playerFBX->GetPosition().x && camera->GetEye().x - playerFBX->GetPosition().x > -48.0f)
	//		{
	//			calculatedDegrees = -(camera->GetEye().x - playerFBX->GetPosition().x) * 1.875f + 180.0f;
	//		}
	//		else if (0.0f <= camera->GetEye().z - playerFBX->GetPosition().z && camera->GetEye().z - playerFBX->GetPosition().z < 48.0f
	//			&& 0.0f > camera->GetEye().x - playerFBX->GetPosition().x && camera->GetEye().x - playerFBX->GetPosition().x >= -48.0f)
	//		{
	//			calculatedDegrees = (camera->GetEye().z - playerFBX->GetPosition().z) * 1.875f + 270.0f;

	//		}
	//		degreeTransfer = calculatedDegrees;
	//	}
	//	float min = FLT_MAX;
	//	int closestEnemy = 0;
	//	for (int i = 0; i < 10; i++)
	//	{
	//		if (!baseAreaEnemyFBX[i]->dead)
	//		{
	//			float x = baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x;
	//			float y = baseAreaEnemyFBX[i]->GetPosition().z - playerFBX->GetPosition().z;
	//			if (abs(sqrt(x * x + y * y)) < min)
	//			{
	//				min = abs(sqrt(x * x + y * y));
	//				closestEnemy = i;
	//			}
	//		}
	//	}
	//	if (min <= 96.0f)
	//	{
	//		float x2 = baseAreaEnemyFBX[closestEnemy]->GetPosition().x - playerFBX->GetPosition().x;
	//		float y2 = baseAreaEnemyFBX[closestEnemy]->GetPosition().z - playerFBX->GetPosition().z;
	//		float radians = atan2(y2, x2);
	//		float degrees = XMConvertToDegrees(radians);
	//		playerFBX->SetRotation({ playerFBX->GetRotation().x, -degrees + 90.0f, playerFBX->GetRotation().z });
	//		objectRotation = playerFBX->GetRotation();
	//		lockOnActive = true;
	//		//camera->SetTarget(playerFBX->GetPosition());
	//		//camera->SetDistance(48.0f);
	//		camera->SetTarget(baseAreaEnemyFBX[closestEnemy]->GetPosition());
	//		if (min > 0.0f)
	//		{
	//			camera->SetDistance(min + 48.0f);
	//		}
	//		else
	//		{
	//			camera->SetDistance(48.0f);
	//		}
	//		camera->Update();
	//	}
	//	else
	//	{
	//		lockOnActive = false;
	//		degreeTransfer = 0.0f;
	//		camera->SetTarget(playerFBX->GetPosition());
	//		camera->SetDistance(48.0f);
	//		camera->Update();
	//	}
	//}
	//else
	//{
	//	camera->SetTarget(playerFBX->GetPosition());
	//	camera->SetDistance(48.0f);
	//	camera->Update();
	//	lockOnActive = false;
	//}
#pragma endregion

#pragma region DebugAttackRange
	attackRangeOBJ[0]->SetPosition({ (objectPosition.x + (sinf(XMConvertToRadians(objectRotation.y)) * 15)), objectPosition.y + 0.5f, (objectPosition.z + (cosf(XMConvertToRadians(objectRotation.y)) * 15)) });
	attackRangeOBJ[0]->SetRotation(objectRotation);

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		attackRangeOBJ[i + 1]->SetPosition({ (baseAreaEnemyFBX[i]->GetPosition().x + (sinf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * 15)), baseAreaEnemyFBX[i]->GetPosition().y + 0.5f, (baseAreaEnemyFBX[i]->GetPosition().z + (cosf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * 15)) });
		attackRangeOBJ[i + 1]->SetRotation(baseAreaEnemyFBX[i]->GetRotation());
		landingPositionOBJ[i]->SetPosition(baseAreaEnemyFBX[i]->landingAttackPosition);
	}
#pragma endregion
	
#pragma region VisionRange
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		enemyVisionRangeOBJ[i]->SetPosition({ (baseAreaEnemyFBX[i]->GetPosition().x + (sinf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * 40)), baseAreaEnemyFBX[i]->GetPosition().y + 0.5f, (baseAreaEnemyFBX[i]->GetPosition().z + (cosf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * 40))});
		enemyVisionRangeOBJ[i]->SetRotation(baseAreaEnemyFBX[i]->GetRotation());
	}
#pragma endregion

	if (firstRun)
	{
		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			if (baseAreaEnemyFBX[i]->patrolStatus == EnemyHuman::BACK)
			{
				baseAreaEnemyFBX[i]->SetTimer(119.0f);
			}
		}

		firstRun = false;
	}

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::WANDER || baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::STAND)
		{
			if (i % 2 == 0 || i == 0)
			{
				if (baseAreaEnemyFBX[i + 1]->enumStatus == EnemyHuman::AGGRO || baseAreaEnemyFBX[i + 1]->enumStatus == EnemyHuman::CHARGEATTACK || baseAreaEnemyFBX[i + 1]->enumStatus == EnemyHuman::JETSTREAMATTACK || baseAreaEnemyFBX[i + 1]->enumStatus == EnemyHuman::LANDINGATTACK
					|| baseAreaEnemyFBX[i + 1]->enumStatus == EnemyHuman::PARTICLEATTACK || baseAreaEnemyFBX[i + 1]->enumStatus == EnemyHuman::COOLDOWN || baseAreaEnemyFBX[i + 1]->enumStatus == EnemyHuman::ATTACK)
				{
					baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
				}
			}
			else
			{
				if (baseAreaEnemyFBX[i - 1]->enumStatus == EnemyHuman::AGGRO || baseAreaEnemyFBX[i - 1]->enumStatus == EnemyHuman::CHARGEATTACK || baseAreaEnemyFBX[i - 1]->enumStatus == EnemyHuman::JETSTREAMATTACK || baseAreaEnemyFBX[i - 1]->enumStatus == EnemyHuman::LANDINGATTACK
					|| baseAreaEnemyFBX[i - 1]->enumStatus == EnemyHuman::PARTICLEATTACK || baseAreaEnemyFBX[i - 1]->enumStatus == EnemyHuman::COOLDOWN || baseAreaEnemyFBX[i - 1]->enumStatus == EnemyHuman::ATTACK)
				{
					baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
				}
			}
		}
	}

#pragma region EnemyAggro
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (baseAreaEnemyFBX[i]->HP <= fleeHP && !enemyKnockback && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::DAMAGED && !baseAreaEnemyFBX[i]->helpCall && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::DEAD
			|| baseAreaEnemyFBX[i]->isPartnerDead && !enemyKnockback && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::DAMAGED && !baseAreaEnemyFBX[i]->helpCall && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::DEAD)
		{
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::FLEE);
			if (!baseAreaEnemyFBX[i]->fleeSet)
			{
				if (oddEven == 1)
				{
					if (baseAreaEnemyFBX[i]->patrolStatus == EnemyHuman::FRONT)
					{
						continue;
					}
				}
				else
				{
					if (baseAreaEnemyFBX[i]->patrolStatus == EnemyHuman::BACK)
					{
						continue;
					}
				}
				baseAreaEnemyFBX[i]->SetAggroSwitch(true);
				float min = FLT_MAX;
				baseAreaEnemyFBX[i]->closestEnemy = closestEnemyDefaultNumber;
				for (int j = 0; j < numberOfEnemiesTotal; j++)
				{
					if (!baseAreaEnemyFBX[j]->dead && j != i && !baseAreaEnemyFBX[j]->helpCall)
					{
						if (j == i || baseAreaEnemyFBX[i]->patrolStatus == EnemyHuman::FRONT && j == (i + 1) || baseAreaEnemyFBX[i]->patrolStatus == EnemyHuman::BACK && j == (i - 1))
						{
							continue;
						}
						float x = baseAreaEnemyFBX[j]->GetPosition().x - baseAreaEnemyFBX[i]->GetPosition().x;
						float y = baseAreaEnemyFBX[j]->GetPosition().z - baseAreaEnemyFBX[i]->GetPosition().z;
						if (abs(sqrt(x * x + y * y)) < min && baseAreaEnemyFBX[j]->isBeingCalledToHelp == false)
						{
							min = abs(sqrt(x * x + y * y));
							baseAreaEnemyFBX[i]->closestEnemy = j;
						}
					}
				}
				if (baseAreaEnemyFBX[i]->closestEnemy == closestEnemyDefaultNumber)
				{
					baseAreaEnemyFBX[i]->aggroSet = false;
					baseAreaEnemyFBX[i]->Reset();
					baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
					baseAreaEnemyFBX[i]->helpCall = true;
				}
				baseAreaEnemyFBX[i]->fleeSet = true;
				if (baseAreaEnemyFBX[i]->closestEnemy != closestEnemyDefaultNumber)
				{
					if (baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->patrolStatus == EnemyHuman::FRONT)
					{
						baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->isBeingCalledToHelp = true;
						baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy + 1]->isBeingCalledToHelp = true;
					}
					else
					{
						baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->isBeingCalledToHelp = true;
						baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy - 1]->isBeingCalledToHelp = true;
					}
				}
			}
			float x2 = baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->GetPosition().x - baseAreaEnemyFBX[i]->GetPosition().x;
			float y2 = baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->GetPosition().z - baseAreaEnemyFBX[i]->GetPosition().z;
			float hypotenuse = sqrt((x2 * x2) + (y2 * y2));
			float radians = atan2(y2, x2);
			float degrees = XMConvertToDegrees(radians);
			baseAreaEnemyFBX[i]->SetRotation({ baseAreaEnemyFBX[i]->GetRotation().x, -degrees + yRotationOffset, baseAreaEnemyFBX[i]->GetRotation().z });
			baseAreaEnemyFBX[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x + fleeSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (x2 / hypotenuse),
											   baseAreaEnemyFBX[i]->GetPosition().y,
											   baseAreaEnemyFBX[i]->GetPosition().z + fleeSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (y2 / hypotenuse) });
			if (Distance(baseAreaEnemyFBX[i]->GetPosition(), baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->GetPosition()) <= fleeHelpCallRange && !baseAreaEnemyFBX[i]->helpCall)
			{
				baseAreaEnemyFBX[i]->aggroSet = false;
				baseAreaEnemyFBX[i]->Reset();
				baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
				baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->aggroSet = false;
				baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->SetEnumStatus(EnemyHuman::AGGRO);
				baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->isBeingCalledToHelp = false;
				baseAreaEnemyFBX[i]->helpCall = true;
				baseAreaEnemyFBX[i]->fleeSet = false;
			}
		}
		else if (agroodEnemies > 2 && baseAreaEnemyFBX[i]->debugJetAttacked == false && baseAreaEnemyFBX[i]->jetStreamCounted == false && baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::AGGRO)
		{
			debugJetStream++;
			baseAreaEnemyFBX[i]->jetStreamCounted = true;
		}
		else if (intersect(playerFBX->GetPosition(), enemyVisionRangeOBJ[i]->GetPosition(), playerInteresectSize, enemyAggroVisionRange, enemyAggroVisionRange) && !enemyKnockback && baseAreaEnemyAliveBOOL[i] == true || baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::AGGRO && !enemyKnockback && baseAreaEnemyAliveBOOL[i] == true)
		{
			float distance = sqrt((baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x) * (baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x) + (baseAreaEnemyFBX[i]->GetPosition().z - playerFBX->GetPosition().z) * (baseAreaEnemyFBX[i]->GetPosition().z - playerFBX->GetPosition().z));
			if (distance < maxChargeDistance && distance > minChargeDistance && baseAreaEnemyFBX[i]->chargeAttackCheck == false && baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::AGGRO && !enemyKnockback && baseAreaEnemyAliveBOOL[i] == true)
			{
				if (baseAreaEnemyFBX[i]->agrooNumber == 0)
				{
					agroodEnemies++;
					baseAreaEnemyFBX[i]->agrooNumber = agroodEnemies;
				}

				if (baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::DAMAGED && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::PARTICLEATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::ATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::COOLDOWN
					&& baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::LANDINGATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::CHARGEATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::JETSTREAMATTACK)
				{
					/*int random = rand() % 10;

					if (random < 5)
					{
						baseAreaEnemyFBX[i]->chargeAttackStage = 0;
						baseAreaEnemyFBX[i]->modelChange = true;
						baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::CHARGEATTACK);
						baseAreaEnemyFBX[i]->chargeAttackCheck = true;
					}
					else
					{
						baseAreaEnemyFBX[i]->chargeAttackCheck = true;
					}*/

					baseAreaEnemyFBX[i]->chargeAttackStage = 0;
					baseAreaEnemyFBX[i]->modelChange = true;
					baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::CHARGEATTACK);
				}
			}
			else if (distance <= minChargeDistance)
			{
				if (baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::DAMAGED && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::PARTICLEATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::ATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::COOLDOWN
					&& baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::LANDINGATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::CHARGEATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::JETSTREAMATTACK)
				{
					int random = rand() % 10;

					if (random < 4)
					{
						baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::ATTACK);
					}
					else if (random < 7)
					{
						baseAreaEnemyFBX[i]->particleAttackStage = 0;
						baseAreaEnemyFBX[i]->modelChange = true;
						baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::PARTICLEATTACK);
					}
					else
					{
						baseAreaEnemyFBX[i]->landingAttackStage = 0;
						baseAreaEnemyFBX[i]->modelChange = true;
						baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::LANDINGATTACK);
					}
				}
			}
			else
			{
				if (baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::DAMAGED && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::COOLDOWN && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::PARTICLEATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::ATTACK
					&& baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::LANDINGATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::CHARGEATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::JETSTREAMATTACK)
				{
					if (baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::AGGRO)
					{
						baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
					}
				}
			}
		}

		if (baseAreaEnemyFBX[i]->particleAttackActive)
		{
			ParticleCreation(baseAreaEnemyFBX[i]->particleAttackPosition.x, baseAreaEnemyFBX[i]->particleAttackPosition.y, baseAreaEnemyFBX[i]->particleAttackPosition.z, 60, 0.0f, 10.0f);
		}

		if (baseAreaEnemyFBX[i]->landingParticles)
		{
			ParticleCreation(baseAreaEnemyFBX[i]->landingAttackPosition.x, baseAreaEnemyFBX[i]->landingAttackPosition.y, baseAreaEnemyFBX[i]->landingAttackPosition.z, 60, 2.0f, 30.0f);
		}

		if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::CHARGEATTACK && baseAreaEnemyFBX[i]->chargeAttackStage == 1 
			|| baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::JETSTREAMATTACK && baseAreaEnemyFBX[i]->jetStreamAttackStage == 2)
		{
			ParticleCreation(baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z, 30, 4.0f, 30.0f);
		}
	}
#pragma endregion

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		for (int j = 0; j < numberOfEnemiesTotal; j++)
		{
			if (i = j)
			{
				continue;
			}

			if (intersect(baseAreaEnemyFBX[j]->GetPosition(), enemyVisionRangeOBJ[i]->GetPosition(), playerInteresectSize, enemyAggroVisionRange, enemyAggroVisionRange) && baseAreaEnemyAliveBOOL[j] == true && baseAreaEnemyAliveBOOL[i] == true && baseAreaEnemyFBX[j]->enumStatus != EnemyHuman::STAND
				&& baseAreaEnemyFBX[j]->enumStatus != EnemyHuman::WANDER && baseAreaEnemyFBX[j]->enumStatus != EnemyHuman::DEAD)
			{
				if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::STAND || baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::WANDER)
				{
					baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
				}
			}
		}
	}

	if (debugJetStream > 2)
	{
		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::STAND || baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::DEAD || baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::FLEE || baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::WANDER || baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::COOLDOWN)
			{
				continue;
			}

			if (baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::JETSTREAMATTACK && baseAreaEnemyFBX[i]->debugJetAttacked == false)
			{
				baseAreaEnemyFBX[i]->debugJetAttacked = true;
				baseAreaEnemyFBX[i]->jetStreamAttackStage = 0;
				baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::JETSTREAMATTACK);
			}
		}
	}

#pragma region playerHPDamage
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (intersect(attackRangeOBJ[i + 1]->GetPosition(), playerFBX->GetPosition(), playerInteresectSize, baseAttackRange, baseAttackRange) && baseAreaEnemyAliveBOOL[i] == true && baseAreaEnemyFBX[i]->attackDamagePossible && baseAreaEnemyFBX[i]->ableToDamage)
		{
			baseAreaEnemyFBX[i]->ableToDamage = false;
			damageOverlaySpriteALPHA = 1.0f;
			damageOverlayDisplay = true;
			screenShake = true;
			baseAreaDamageOverlaySPRITE->SetColor({ 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
			playerFBX->hp -= baseAttackDamage;
			playerFBX->damageAdvantage = true;
			playerFBX->SetEnumStatus(Player::DAMAGED);
		}

		if (intersect(baseAreaEnemyFBX[i]->particleAttackPosition, playerFBX->GetPosition(), playerInteresectSize, particleAttackRange, particleAttackRange) && baseAreaEnemyFBX[i]->particleAttackActive && baseAreaEnemyFBX[i]->ableToDamage)
		{
			baseAreaEnemyFBX[i]->ableToDamage = false;
			damageOverlaySpriteALPHA = 1.0f;
			damageOverlayDisplay = true;
			screenShake = true;
			baseAreaDamageOverlaySPRITE->SetColor({ 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
			playerFBX->hp -= particleAttackDamage;
			playerFBX->damageAdvantage = true;
			playerFBX->SetEnumStatus(Player::DAMAGED);
			knockback = true;
		}

		if (intersect(baseAreaEnemyFBX[i]->landingAttackPosition, playerFBX->GetPosition(), playerInteresectSize, landingAttackRange, landingAttackRange) && baseAreaEnemyFBX[i]->landingParticles && baseAreaEnemyFBX[i]->ableToDamage)
		{
			baseAreaEnemyFBX[i]->ableToDamage = false;
			damageOverlaySpriteALPHA = 1.0f;
			damageOverlayDisplay = true;
			screenShake = true;
			baseAreaDamageOverlaySPRITE->SetColor({ 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
			playerFBX->hp -= landingAttackDamage;
			playerFBX->damageAdvantage = true;
			playerFBX->SetEnumStatus(Player::DAMAGED);
			knockback = true;
		}

		if (intersect(baseAreaEnemyFBX[i]->GetPosition(), playerFBX->GetPosition(), playerInteresectSize, chargeAttackRange, chargeAttackRange) && baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::CHARGEATTACK && baseAreaEnemyFBX[i]->chargeAttackStage == 1 && baseAreaEnemyFBX[i]->ableToDamage
			|| intersect(baseAreaEnemyFBX[i]->GetPosition(), playerFBX->GetPosition(), playerInteresectSize, chargeAttackRange, chargeAttackRange) && baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::JETSTREAMATTACK && baseAreaEnemyFBX[i]->jetStreamAttackStage == 2 && baseAreaEnemyFBX[i]->ableToDamage)
		{
			baseAreaEnemyFBX[i]->ableToDamage = false;
			damageOverlaySpriteALPHA = 1.0f;
			damageOverlayDisplay = true;
			screenShake = true;
			baseAreaDamageOverlaySPRITE->SetColor({ 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
			playerFBX->hp -= chargeAttackDamage;
			playerFBX->damageAdvantage = true;
			playerFBX->SetEnumStatus(Player::DAMAGED);
			knockback = true;
		}

		if (knockback && baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::PARTICLEATTACK || knockback && baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::LANDINGATTACK || knockback && baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::CHARGEATTACK)
		{
			XMFLOAT3 xyz = baseAreaEnemyFBX[i]->GetPosition() - playerFBX->GetPosition();
			XMFLOAT3 knockbackPrevPosition = playerFBX->GetPosition();
			float hypotenuse = sqrt((xyz.x * xyz.x) + (xyz.z * xyz.z));
			if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::PARTICLEATTACK)
			{
				playerFBX->SetPosition({
					knockbackPrevPosition.x -= particleAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += 3.0f,
					knockbackPrevPosition.z -= particleAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
			}
			else if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::LANDINGATTACK)
			{
				playerFBX->SetPosition({
					knockbackPrevPosition.x -= landingAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += 3.0f,
					knockbackPrevPosition.z -= landingAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
			}
			else if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::CHARGEATTACK)
			{
				playerFBX->SetPosition({
					knockbackPrevPosition.x += chargeAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += 3.0f,
					knockbackPrevPosition.z += chargeAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
			}
			else if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::JETSTREAMATTACK)
			{
				playerFBX->SetPosition({
					knockbackPrevPosition.x += chargeAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += 3.0f,
					knockbackPrevPosition.z += chargeAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
			}
			knockbackTime += knockbackInterval * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (knockbackTime >= knockbackMaxTime)
			{
				knockbackTime = 0.0f;
				knockback = false;
			}
		}

		if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::LANDINGATTACK && baseAreaEnemyFBX[i]->landingParticles == true)
		{
			baseAreaEnemyFBX[i]->landingParticles = false;
		}
	}

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (playerFBX->damageAdvantage)
		{
			if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::AGGRO || baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::COOLDOWN)
			{
				float distance = sqrt((baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x) * (baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x) + (baseAreaEnemyFBX[i]->GetPosition().z - playerFBX->GetPosition().z) * (baseAreaEnemyFBX[i]->GetPosition().z - playerFBX->GetPosition().z));
				if (distance < damageAdvantageShortLongRangeBorder)
				{
					baseAreaEnemyFBX[i]->chargeAttackStage = 0;
					baseAreaEnemyFBX[i]->modelChange = true;
					baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::CHARGEATTACK);
				}
				else
				{
					baseAreaEnemyFBX[i]->landingAttackStage = 0;
					baseAreaEnemyFBX[i]->modelChange = true;
					baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::LANDINGATTACK);
				}
			}
		}
	}

	playerFBX->damageAdvantage = false;

	if (playerFBX->hp <= 0.0f)
	{
		playerFBX->hp = 0.0f;
		playerFBX->SetEnumStatus(Player::DEAD);
	}
	
	if (playerFBX->isPlayerDead)
	{
		fadeSpriteALPHA += fadeSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		fadeSPRITE->SetColor({ 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
		if (fadeSpriteALPHA >= 1.0f)
		{
			fadeSpriteALPHA = 1.0f;
			audio->StopWave("MainArea.wav");
			result = 1;
			deletion = true;
		}
	}

	if (screenShake)
	{
		camera->SetTarget(ScreenShake(playerFBX->GetPosition()));
		camera->Update();
		shakeTimer += shakeTimerInterval * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		if (shakeTimer >= maxShakeTimer)
		{
			shakeTimer = 0.0f;
			screenShake = false;
		}
	}

	// Damage overlay display
	if (damageOverlayDisplay)
	{
		if (playerFBX->enumStatus != Player::DEAD)
		{
			damageOverlaySpriteALPHA -= fadeSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}
		baseAreaDamageOverlaySPRITE->SetColor({ 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
		if (damageOverlaySpriteALPHA <= 0.0f)
		{
			damageOverlayDisplay = false;
		}
	}
#pragma endregion

#pragma region playerAttack
	if (playerFBX->enumStatus == Player::ATTACK)
	{
		if (playerFBX->timer >= playerFirstAttackStartTimer && playerFBX->timer <= playerFirstAttackEndTimer ||
			playerFBX->timer >= playerSecondAttackStartTimer && playerFBX->timer <= playerSecondAttackEndTimer ||
			playerFBX->timer >= playerThirdAttackStartTimer && playerFBX->timer <= playerThirdAttackEndTimer)
		{
			if (playerFBX->enumStatus == Player::ATTACK && playerFBX->ableToDamage)
			{
				for (int i = 0; i < numberOfEnemiesTotal; i++)
				{
					if (intersect(attackRangeOBJ[0]->GetPosition(), baseAreaEnemyFBX[i]->GetPosition(), playerInteresectSize, playerAttackRange, playerAttackRange) && baseAreaEnemyAliveBOOL[i] == true
						&& (abs(baseAreaEnemyFBX[i]->GetPosition().y - playerFBX->GetPosition().y) <= 4.0f))
					{
						baseAreaEnemyFBX[i]->HP -= playerAttackDamage;
						if (playerFBX->timer >= playerThirdAttackStartTimer && playerFBX->timer <= playerThirdAttackEndTimer)
						{
							enemyKnockbackTime = 0.0f;
							enemyKnockback = true;
						}

						if (baseAreaEnemyFBX[i]->HP < 0.9f)
						{
							baseAreaEnemyAliveBOOL[i] = false;
							enemyDefeated++;
							agroodEnemies--;
							baseAreaEnemyFBX[i]->dead = true;
							baseAreaEnemyFBX[i]->modelChange = true;
							baseAreaEnemyRespawnTimerFLOAT[i] = 600;
							baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::DEAD);
						}
						else
						{
							baseAreaEnemyFBX[i]->timer = 0.0f;
							baseAreaEnemyFBX[i]->modelChange = true;
							baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::DAMAGED);
						}
						if (playerFBX->timer >= playerThirdAttackStartTimer && playerFBX->timer <= playerThirdAttackEndTimer)
						{
							ParticleCreation(baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z, 90, 5.0f, 20.0f);
						}
						else
						{
							ParticleCreation(baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z, 60, 5.0f, 10.0f);
						}
						playerFBX->ableToDamage = false;
					}
				}
			}
		}

		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			if (enemyKnockback)
			{
				XMFLOAT3 xyz = playerFBX->GetPosition() - baseAreaEnemyFBX[i]->GetPosition();
				XMFLOAT3 knockbackPrevPosition = baseAreaEnemyFBX[i]->GetPosition();
				float hypotenuse = sqrt((xyz.x * xyz.x) + (xyz.z * xyz.z));
				baseAreaEnemyFBX[i]->SetPosition({
						knockbackPrevPosition.x -= playerAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
						knockbackPrevPosition.y += 3.0f,
						knockbackPrevPosition.z -= playerAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
				enemyKnockbackTime += playerAttackKnockbackInterval * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				if (enemyKnockbackTime >= playerAttackKnockbackMaxtime)
				{
					enemyKnockbackTime = 0.0f;
					enemyKnockback = false;
				}
			}
		}

		if (playerFBX->timer > playerFirstAttackEndTimer && playerFBX->timer < playerSecondAttackStartTimer ||
			playerFBX->timer > playerSecondAttackEndTimer && playerFBX->timer < playerThirdAttackStartTimer)
		{
			playerFBX->ableToDamage = true;
		}
	}
	else
	{
		playerFBX->ableToDamage = true;
		playerFBX->attackCombo = 0;
	}
	
#pragma endregion

#pragma region playerHeal
	if (playerFBX->enumStatus == Player::HEAL)
	{
		if (playerFBX->timer >= healFirstParticleRingTimer)
		{
			for (int i = 0; i < healNumberOfParticles; i++)
			{
				ParticleCreationHeal(playerFBX->healParticlePosition[0].x + cosf(XMConvertToRadians(i * 2.0f)) * 10.0f,
					playerFBX->healParticlePosition[0].y,
					playerFBX->healParticlePosition[0].z + sinf(XMConvertToRadians(i * 2.0f)) * 10.0f, 10, 0.0f, 1.0f);
			}
		}
		if (playerFBX->timer >= healSecondParticleRingTimer)
		{
			for (int i = 0; i < healNumberOfParticles; i++)
			{
				ParticleCreationHeal(playerFBX->healParticlePosition[1].x + cosf(XMConvertToRadians(i * 2.0f)) * 10.0f,
					playerFBX->healParticlePosition[1].y,
					playerFBX->healParticlePosition[1].z + sinf(XMConvertToRadians(i * 2.0f)) * 10.0f, 10, 0.0f, 1.0f);
			}
		}
		if (playerFBX->timer >= healThirdParticleRingTimer)
		{
			for (int i = 0; i < healNumberOfParticles; i++)
			{
				ParticleCreationHeal(playerFBX->healParticlePosition[2].x + cosf(XMConvertToRadians(i * 2.0f)) * 10.0f,
					playerFBX->healParticlePosition[2].y,
					playerFBX->healParticlePosition[2].z + sinf(XMConvertToRadians(i * 2.0f)) * 10.0f, 10, 0.0f, 1.0f);
			}
		}
	}
#pragma endregion

#pragma region aggroEveryEnemyWhenOneKillAwayFromWinning
	if (enemyDefeated > clearCondition - 2)
	{
		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			if (baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::DEAD)
			{
				if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::STAND || baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::WANDER)
				{
					baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
				}
			}
		}
	}
#pragma endregion

#pragma region clearCondition
	if (enemyDefeated > clearCondition - 1)
	{
		enemyDefeated = clearCondition;
		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::DEAD);
		}
		fadeSpriteALPHA += fadeSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		fadeSPRITE->SetColor({ 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
		if (fadeSpriteALPHA >= 1.0f)
		{
			fadeSpriteALPHA = 1.0f;
			audio->StopWave("MainArea.wav");
			result = 2;
			deletion = true;
		}
	}
#pragma endregion

#pragma region enemyRespawn
	// Testing 10 enemies with no respawn
	/*for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (baseAreaEnemyRespawnTimerFLOAT[i] > 0.0f)
		{
			baseAreaEnemyRespawnTimerFLOAT[i] -= 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}
		else
		{
			baseAreaEnemyRespawnTimerFLOAT[i] = 0.0f;
		}

		if (baseAreaEnemyRespawnTimerFLOAT[i] < 0.1f && !baseAreaEnemyAliveBOOL[i])
		{
			baseAreaEnemyFBX[i]->SetPosition(baseAreaEnemySpawnXMFLOAT3[i]);
			baseAreaEnemyPositionOBJ[i]->SetPosition(baseAreaEnemySpawnXMFLOAT3[i]);
			baseAreaEnemyFBX[i]->dead = false;
			baseAreaEnemyFBX[i]->HP = 5.0f;
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::STAND);
			baseAreaEnemyFBX[i]->set = false;
			baseAreaEnemyFBX[i]->timer = 238.0f;
			baseAreaEnemyFBX[i]->helpCall = false;
			baseAreaEnemyFBX[i]->agrooNumber = 0;
			baseAreaEnemyFBX[i]->Reset();
			baseAreaEnemyAliveBOOL[i] = true;
		}
	}*/
#pragma endregion

#pragma region areaBoundaryLimits
	if (!playerFBX->baseAreaOpeningCutscene)
	{
		if (playerFBX->GetPosition().x > mapBorder)
		{
			playerFBX->SetPosition({ mapBorder, playerFBX->GetPosition().y, playerFBX->GetPosition().z });
			camera->SetTarget(playerFBX->GetPosition());
		}
		else if (playerFBX->GetPosition().x < -mapBorder)
		{
			playerFBX->SetPosition({ -mapBorder, playerFBX->GetPosition().y, playerFBX->GetPosition().z });
			camera->SetTarget(playerFBX->GetPosition());
		}

		if (playerFBX->GetPosition().z > mapBorder)
		{
			playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, mapBorder });
			camera->SetTarget(playerFBX->GetPosition());
		}
		else if (playerFBX->GetPosition().z < -mapBorder)
		{
			playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, -mapBorder });
			camera->SetTarget(playerFBX->GetPosition());
		}

		for (float i = 0.0f; i < mapBorderParticles; i++)
		{
			if (Distance(playerFBX->GetPosition(), { -mapBorderParticlePosition + (8 * i), playerFBX->GetPosition().y, -mapBorderParticlePosition}) <= mapBorderVisibleDistance)
			{
				ParticleCreationEdge(-mapBorderParticlePosition + (8 * i), playerFBX->GetPosition().y, -mapBorderParticlePosition, 30, 5.0f, 3.0f);
			}

			if (Distance(playerFBX->GetPosition(), { -mapBorderParticlePosition + (8 * i), playerFBX->GetPosition().y, mapBorderParticlePosition }) <= mapBorderVisibleDistance)
			{
				ParticleCreationEdge(-mapBorderParticlePosition + (8 * i), playerFBX->GetPosition().y, mapBorderParticlePosition, 30, 5.0f, 3.0f);
			}

			if (Distance(playerFBX->GetPosition(), { -mapBorderParticlePosition, playerFBX->GetPosition().y, -mapBorderParticlePosition + (8 * i) }) <= mapBorderVisibleDistance)
			{
				ParticleCreationEdge(-mapBorderParticlePosition, playerFBX->GetPosition().y, -mapBorderParticlePosition + (8 * i), 30, 5.0f, 3.0f);
			}

			if (Distance(playerFBX->GetPosition(), { mapBorderParticlePosition, playerFBX->GetPosition().y, -mapBorderParticlePosition + (8 * i) }) <= mapBorderVisibleDistance)
			{
				ParticleCreationEdge(mapBorderParticlePosition, playerFBX->GetPosition().y, -mapBorderParticlePosition + (8 * i), 30, 5.0f, 3.0f);
			}
		}
	}
#pragma endregion

#pragma region HPSTUpdates
	if (!startMissionSpriteMovement)
	{
		baseAreaMissionSPRITE->SetPosition({ 100.0f, 50.0f });
		baseAreaMissionSPRITE->SetSize({ 1080.0f, 620.0f });
	}
	else
	{
		if (!xSet || !ySet)
		{
			float x = (1150.0f - baseAreaMissionSPRITE->GetPosition().x);
			float y = (100.0f - baseAreaMissionSPRITE->GetPosition().y);
			float hypotenuse = sqrtf((x * x) + (y * y));
			/*x = (1150.0f - baseAreaMissionSPRITE->GetPosition().x) / 2.0f;
			y = (100.0f - baseAreaMissionSPRITE->GetPosition().y) / 30.0f;*/
			XMFLOAT2 newMissionPosition = baseAreaMissionSPRITE->GetPosition();
			if (baseAreaMissionSPRITE->GetPosition().x < 1150.0f)
			{
				newMissionPosition.x += 360.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (x / hypotenuse);
			}
			if (baseAreaMissionSPRITE->GetPosition().y < 100.0f)
			{
				newMissionPosition.y += 360.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (y / hypotenuse);
			}
			baseAreaMissionSPRITE->SetPosition(newMissionPosition);
		}
		else
		{
			baseAreaMissionSPRITE->SetPosition({ 1150.0f, 100.0f });
		}

		if (!sizeSet)
		{
			XMFLOAT2 missionSpriteSize = baseAreaMissionSPRITE->GetSize();
			if (baseAreaMissionSPRITE->GetSize().x > 100.0f)
			{
				missionSpriteSize.x -= 360.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			if (baseAreaMissionSPRITE->GetSize().y > 80.0f)
			{
				missionSpriteSize.y -= 180.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			baseAreaMissionSPRITE->SetSize(missionSpriteSize);
		}
		else
		{
			baseAreaMissionSPRITE->SetSize({ 100.0f, 80.0f });
		}

		if (baseAreaMissionSPRITE->GetPosition().x >= 1150.0f)
		{
			baseAreaMissionSPRITE->SetPosition({ 1150.0f, baseAreaMissionSPRITE->GetPosition().y });
			xSet = true;
		}
		if (baseAreaMissionSPRITE->GetPosition().y >= 100.0f)
		{
			baseAreaMissionSPRITE->SetPosition({ baseAreaMissionSPRITE->GetPosition().x, 100.0f });
			ySet = true;
		}

		if (baseAreaMissionSPRITE->GetSize().x <= 100.0f || baseAreaMissionSPRITE->GetSize().y <= 80.0f)
		{
			if (baseAreaMissionSPRITE->GetSize().x <= 100.0f)
			{
				baseAreaMissionSPRITE->SetSize({ 100.0f, baseAreaMissionSPRITE->GetSize().y });
			}
			if (baseAreaMissionSPRITE->GetSize().y <= 80.0f)
			{
				baseAreaMissionSPRITE->SetSize({ baseAreaMissionSPRITE->GetSize().x, 80.0f });
			}

			if (baseAreaMissionSPRITE->GetSize().x <= 100.0f && baseAreaMissionSPRITE->GetSize().y <= 80.0f)
			{
				sizeSet = true;
			}
		}
	}
	HPBarSPRITE->SetSize({ playerFBX->hp * 20.0f, 40.0f });
	HPBarFrameSPRITE->SetSize({ 25.0f * 20.0f, 40.0f });
	STBarSPRITE->SetSize({ playerFBX->stamina / 2.0f, 20.0f });
	STBarFrameSPRITE->SetSize({ 50.0f, 20.0f });
	STBarSPRITE->SetPosition({ 720.0f, 180.0f });
	STBarFrameSPRITE->SetPosition({ 720.0f, 180.0f });
	STBarSPRITE->SetRotation(270.0f);
	STBarFrameSPRITE->SetRotation(270.0f);
	if (playerFBX->stamina < 100.0f && playerFBX->stamina >= 40.0f)
	{
		staminaSpriteAlpha = 1.0f;
		blinkingStaminaAlpha = 1.0f;
	}
	else if (playerFBX->stamina >= 100.0f)
	{
		staminaSpriteAlpha -= staminaSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	}
	else
	{
		if (!staminaBlinkingEffect)
		{
			blinkingStaminaAlpha -= blinkingStaminaSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (blinkingStaminaAlpha <= 0.0f)
			{
				blinkingStaminaAlpha = 0.0f;
				staminaBlinkingEffect = true;
			}
		}
		else
		{
			blinkingStaminaAlpha += blinkingStaminaSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (blinkingStaminaAlpha >= 1.0f)
			{
				blinkingStaminaAlpha = 1.0f;
				staminaBlinkingEffect = false;
			}
		}
	}

	STBarSPRITE->SetColor({ 1.0f, blinkingStaminaAlpha, blinkingStaminaAlpha, staminaSpriteAlpha });
	STBarFrameSPRITE->SetColor({ 1.0f, 1.0f, 1.0f, staminaSpriteAlpha });

	if (staminaSpriteAlpha <= 0.0f)
	{
		staminaSpriteAlpha = 0.0f;
	}

	if (input->PushKey(DIK_LSHIFT) && playerFBX->stamina > 0.0f || input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && playerFBX->stamina > 0.0f)
	{
		ParticleCreation(playerFBX->GetPosition().x, playerFBX->GetPosition().y, playerFBX->GetPosition().z, 10, -1.0f, 1.0f);
	}
#pragma endregion

#pragma region minimapUpdates
	baseAreaMinimapPlayerSPRITE->SetPosition({ playerFBX->GetPosition().x * 0.3f + 165.0f, playerFBX->GetPosition().z * 0.3f + 545.0f});
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaMinimapEnemySPRITE[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x * 0.3f + 165.0f, baseAreaEnemyFBX[i]->GetPosition().z * 0.3f + 545.0f });
	}
#pragma endregion

#pragma region missionTracker
	missionTracker << enemyDefeated << " / 5"
		<< std::fixed << std::setprecision(0)
		<< std::setw(2) << std::setfill('0');
	if (!playerFBX->baseAreaOpeningCutscene && xSet && ySet && sizeSet)
	{
		debugText->Print(missionTracker.str(), 1173.0f, 160.0f, 1.0f);
	}
#pragma endregion

#pragma region healTracker
	healTracker << playerFBX->healRemaining
		<< std::fixed << std::setprecision(0)
		<< std::setw(2) << std::setfill('0');
	debugText->Print(healTracker.str(), 1182.0f, 614.0f, 1.0f);
#pragma endregion

	//std::array<Vector2, 4> enemyHPPosition;
	//std::array<Vector2, 4> enemyHPPositionFrame;

	//for (int i = 0; i < numberOfEnemiesTotal; i++)
	//{
	//	/*camera->Update();
	//	baseAreaEnemyHPBarSPRITE[i]->SetPosition(enemyHPPosition[i].Convert(baseAreaEnemyFBX[i]->GetPosition(),
	//		camera->GetViewProjectionMatrix(), camera->GetProjectionMatrix(), 1280, 720,
	//		-1400.0f + playerFBX->GetPosition().x, -20.0f));
	//	baseAreaEnemyHPBarSPRITE[i]->SetPosition({ baseAreaEnemyHPBarSPRITE[i]->GetPosition().x * -1.0f, baseAreaEnemyHPBarSPRITE[i]->GetPosition().y });
	//	baseAreaEnemyHPBarSPRITE[i]->SetSize({ baseAreaEnemyFBX[i]->HP * 10.0f, 10.0f });
	//	XMFLOAT2 testPosition = baseAreaEnemyHPBarSPRITE[i]->GetPosition();
	//	baseAreaEnemyHPBarFrameSPRITE[i]->SetPosition(enemyHPPositionFrame[i].Convert(baseAreaEnemyFBX[i]->GetPosition(),
	//		camera->GetViewProjectionMatrix(), camera->GetProjectionMatrix(), 1280, 720,
	//		-1400.0f + playerFBX->GetPosition().x, -20.0f));
	//	baseAreaEnemyHPBarFrameSPRITE[i]->SetPosition({ baseAreaEnemyHPBarFrameSPRITE[i]->GetPosition().x * -1.0f, baseAreaEnemyHPBarFrameSPRITE[i]->GetPosition().y });
	//	baseAreaEnemyHPBarFrameSPRITE[i]->SetSize({ 50.0f, 10.0f });*/
	//	// Use until above code is fixed
	//	baseAreaEnemyHPBarSPRITE[i]->SetPosition({1100.0f, 225.0f + (70.0f * i)});
	//	baseAreaEnemyHPBarSPRITE[i]->SetSize({ baseAreaEnemyFBX[i]->HP * 30.0f, 30.0f });
	//	baseAreaEnemyHPBarFrameSPRITE[i]->SetPosition({ 1100.0f, 225.0f + (70.0f * i) });
	//	baseAreaEnemyHPBarFrameSPRITE[i]->SetSize({ 150.0f, 30.0f });
	//}

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (baseAreaEnemyFBX[i]->GetPosition().y < -9.0f)
		{
			baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, 25.0f, baseAreaEnemyFBX[i]->GetPosition().z });
			baseAreaEnemyFBX[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, 25.0f, baseAreaEnemyFBX[i]->GetPosition().z });
		}
	}
#pragma region updates
	playerFBX->Update();
	playerPositionOBJ->Update();
	attackRangeOBJ[0]->Update();
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaEnemyFBX[i]->Update();
		if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::LANDINGATTACK)
		{
			landingPositionOBJ[i]->Update();
		}
		// Disable gravity during sky attack
		if (baseAreaEnemyFBX[i]->landed)
		{
			baseAreaEnemyPositionOBJ[i]->notLandingAttack = true;
		}
		else
		{
			baseAreaEnemyPositionOBJ[i]->notLandingAttack = false;
		}
		baseAreaEnemyPositionOBJ[i]->Update();
		if (baseAreaEnemyAliveBOOL[i])
		{
			enemyVisionRangeOBJ[i]->Update();
			attackRangeOBJ[i + 1]->Update();
		}
	}
	skydomeOBJ->Update();
	extendedGroundOBJ->SetPosition({ playerFBX->GetPosition().x, -11, playerFBX->GetPosition().z });
	extendedGroundOBJ->Update();
	groundOBJ->Update();
	tutorialGroundOBJ->Update();
	collisionManager->CheckAllCollisions();
#pragma endregion

#pragma region dontStackOnTop
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::CHARGEATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::JETSTREAMATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::LANDINGATTACK && baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::FLEE)
		{
			if (FBXCollisionDetection(baseAreaEnemyFBX[i]->GetPosition(), playerFBX->GetPosition(), 4.0f, 4.0f))
			{
				float x = (playerFBX->GetPosition().x - baseAreaEnemyFBX[i]->GetPosition().x);
				float z = (playerFBX->GetPosition().z - baseAreaEnemyFBX[i]->GetPosition().z);

				baseAreaEnemyFBX[i]->SetPosition({ (baseAreaEnemyFBX[i]->GetPosition().x - (9.0f - x)), (baseAreaEnemyFBX[i]->GetPosition().y + 0.5f), (baseAreaEnemyFBX[i]->GetPosition().z - (9.0f - z)) });
			}

			for (int j = 0; j < numberOfEnemiesTotal; j++)
			{
				if (i == j)
				{
					continue;
				}

				if (FBXCollisionDetection(baseAreaEnemyFBX[i]->GetPosition(), baseAreaEnemyFBX[j]->GetPosition(), 4.0f, 4.0f)
					&& baseAreaEnemyFBX[j]->enumStatus != EnemyHuman::DEAD)
				{
					float x = (baseAreaEnemyFBX[i]->GetPosition().x - 2.0f - baseAreaEnemyFBX[j]->GetPosition().x);
					float z = (baseAreaEnemyFBX[i]->GetPosition().z - 2.0f - baseAreaEnemyFBX[j]->GetPosition().z);

					baseAreaEnemyFBX[j]->SetPosition({ (baseAreaEnemyFBX[j]->GetPosition().x - (9.0f - x)), (baseAreaEnemyFBX[j]->GetPosition().y + 0.5f), (baseAreaEnemyFBX[j]->GetPosition().z - (9.0f - z)) });
				}
			}
		}
	}

	playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });
	playerFBX->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::JETSTREAMATTACK && baseAreaEnemyFBX[i]->jetStreamAttackStage == 0)
		{
			baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
			baseAreaEnemyFBX[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
		}
		else if (baseAreaEnemyFBX[i]->enumStatus != EnemyHuman::LANDINGATTACK)
		{
			baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyPositionOBJ[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
			baseAreaEnemyFBX[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyPositionOBJ[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
		}
		else if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::LANDINGATTACK)
		{
			baseAreaEnemyPositionOBJ[i]->SetPosition(baseAreaEnemyFBX[i]->landingAttackPosition);
		}
	}
#pragma endregion

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (i % 2 == 1)
		{
			baseAreaEnemyFBX[i]->SetPatrolPosition(baseAreaEnemyFBX[i - 1]->frontPatrolPosition);
		}
	}

	if (oddEven == 1)
	{
		oddEven = 0;
	}
	else
	{
		oddEven = 1;
	}
	
#pragma region debugTestStrings
	//Debug Start
	//char msgbuf[256];
	//char msgbuf2[256];
	//char msgbuf3[256];
	//sprintf_s(msgbuf, 256, "Y: %f\n", playerFBX->GetPosition().y);
	//sprintf_s(msgbuf2, 256, "DT: %f\n", degreeTransfer);
	//sprintf_s(msgbuf3, 256, "Z: %f\n", camera->GetEye().z);
	//OutputDebugStringA(msgbuf);
	//OutputDebugStringA(msgbuf2);
	//OutputDebugStringA(msgbuf3);
	//Debug End
#pragma endregion
}

void BaseArea::Draw()
{
	// Setting command list
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region backgroundSpriteDrawing
	// Background sprite drawing preprocessing
	//Sprite::PreDraw(cmdList);
	// Background sprite drawing
	//spriteBG->Draw();

	/// <summary>
	/// Background sprite drawing processes
	/// </summary>

	// Sprite drawing post-processing
	//Sprite::PostDraw();
	// Clearing depth buffer
	//dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3DDrawing
	Object3d::PreDraw(cmdList);

	// 3D Object Drawing
	playerFBX->Draw(cmdList);

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaEnemyFBX[i]->Draw(cmdList);
	}

	// Debug only
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		//attackRangeOBJ[i + 1]->Draw();
		/*if (baseAreaEnemyFBX[i]->enumStatus == EnemyHuman::LANDINGATTACK)
		{
			landingPositionOBJ[i]->Draw();
		}*/
		//enemyVisionRangeOBJ[i]->Draw();
	}
	// End Debug

	skydomeOBJ->Draw();
	//extendedGroundOBJ->Draw();
	groundOBJ->Draw();
	tutorialGroundOBJ->Draw();

	// Particle drawing
	particleMan->Draw(cmdList);

	Object3d::PostDraw();
#pragma endregion

#pragma region foregroundSpriteDrawing
	Sprite::PreDraw(cmdList);

	// Foreground sprite drawing
	if (damageOverlayDisplay)
	{
		baseAreaDamageOverlaySPRITE->Draw();
	}
	HPBarSPRITE->Draw();
	HPBarFrameSPRITE->Draw();
	STBarSPRITE->Draw();
	STBarFrameSPRITE->Draw();
	baseAreaMinimapSPRITE->Draw();
	baseAreaMinimapPlayerSPRITE->Draw();
	healSPRITE->Draw();
	if (keyOrMouse == 0)
	{
		healKeyboardSPRITE->Draw();
	}
	else
	{
		healControllerSPRITE->Draw();
	}
	/*for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (Distance(playerFBX->GetPosition(), baseAreaEnemyFBX[i]->GetPosition()) < 160.0f)
		{
			baseAreaEnemyHPBarFrameSPRITE[i]->Draw();
			baseAreaEnemyHPBarSPRITE[i]->Draw();
		}
	}*/
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (!baseAreaEnemyFBX[i]->dead)
		{
			baseAreaMinimapEnemySPRITE[i]->Draw();
		}
	}
	baseAreaMissionSPRITE->Draw();

	// Debug text drawing
	debugText->DrawAll(cmdList);

	if (fadeSpriteALPHA > 0.0f)
	{
		fadeSPRITE->Draw();
	}

	// Sprite post draw
	Sprite::PostDraw();
#pragma endregion
}

int BaseArea::intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH)
{
	XMFLOAT2 circleDistance;

	circleDistance.x = abs(player.x - wall.x);
	circleDistance.y = abs(player.z - wall.z);

	if (circleDistance.x > (rectW / 2.0f + circleR)) { return false; }
	if (circleDistance.y > (rectH / 2.0f + circleR)) { return false; }

	if (circleDistance.x <= (rectW / 2.0f)) { return true; }
	if (circleDistance.y <= (rectH / 2.0f)) { return true; }

	float cornerDistance_sq = ((circleDistance.x - rectW / 2.0f) * (circleDistance.x - rectW / 2.0f)) + ((circleDistance.y - rectH / 2.0f) * (circleDistance.y - rectH / 2.0f));

	return (cornerDistance_sq <= (circleR * circleR));
}

bool BaseArea::FBXCollisionDetection(XMFLOAT3 FBX1, XMFLOAT3 FBX2, float FBX1R, float FBX2R)
{
	float distX = FBX1.x - FBX2.x;
	float distZ = FBX1.z - FBX2.z;
	float distance = sqrtf((distX * distX) + (distZ * distZ));

	if (distance <= FBX1R + FBX2R)
	{
		return true;
	}
	return false;
}

void BaseArea::ParticleCreation(float x, float y, float z, int life, float offset, float start_scale)
{
	for (int i = 0; i < 10; i++) {
		// X,Y,Z‘S‚Ä[-5.0f,+5.0f]‚Åƒ‰ƒ“ƒ_ƒ€‚É•ª•z
		const float rnd_pos = 5.0f;
		XMFLOAT3 pos{};
		pos.x = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + x;
		pos.y = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + y + offset;
		pos.z = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + z;

		const float rnd_vel = 0.1f; // 0.1f
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		XMFLOAT3 acc{};
		const float rnd_acc = 0.001f; // 0.001f
		acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		// ’Ç‰Á
		particleMan->Add(life, pos, vel, acc, start_scale, 0.0f);
	}
}

void BaseArea::ParticleCreationHeal(float x, float y, float z, int life, float offset, float start_scale)
{
	for (int i = 0; i < 10; i++) {
		XMFLOAT3 pos{};
		pos.x = x;
		pos.y = y;
		pos.z = z + offset;

		const float rnd_vel = 0.1f; // 0.1f
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		XMFLOAT3 acc{};
		const float rnd_acc = 0.001f; // 0.001f
		acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		// ’Ç‰Á
		particleMan->Add(life, pos, vel, acc, start_scale, 0.0f);
	}
}

void BaseArea::ParticleCreationEdge(float x, float y, float z, int life, float offset, float start_scale)
{
	for (int i = 0; i < 10; i++) {
		XMFLOAT3 pos{};
		pos.x = x;
		pos.y = y + offset;
		pos.z = z;

		XMFLOAT3 vel{};
		vel.x = vel.y = vel.z = 0.0f;

		XMFLOAT3 acc{};
		acc.y = 0.0f;

		// ’Ç‰Á
		particleMan->Add(life, pos, vel, acc, start_scale, 0.0f);
	}
}

float BaseArea::Distance(XMFLOAT3 player, XMFLOAT3 center)
{
	float d = sqrtf(pow(center.x - player.x, 2.0f) + pow(center.y - player.y, 2.0f) + pow(center.z - player.z, 2.0f));
	return d;
}

XMFLOAT3 BaseArea::ScreenShake(XMFLOAT3 playerPosition)
{
	XMFLOAT3 shakePosition;

	shakePosition.x = playerPosition.x + (rand() % 3 - 1);
	shakePosition.y = playerPosition.y + (rand() % 3 - 1);
	shakePosition.z = playerPosition.z + (rand() % 3 - 1);

	return shakePosition;
}

void BaseArea::thread1()
{
	// Loading debug text
	if (!Sprite::LoadTexture(debugTextTexNumber, "debugfont.png")) { assert(0); return; }

	// Debug text initialization
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);

	// Sprite texture loading
	if (!Sprite::LoadTexture(1, "HPBar.png")) { assert(0); return; } // HP bar texture
	if (!Sprite::LoadTexture(2, "HPBarFrame.png")) { assert(0); return; } // HP bar frame texture
	if (!Sprite::LoadTexture(3, "STBar.png")) { assert(0); return; } // ST bar texture
	if (!Sprite::LoadTexture(4, "STBarFrame.png")) { assert(0); return; } // ST bar frame texture
	if (!Sprite::LoadTexture(5, "Mission1.png")) { assert(0); return; } // Base mission texture
	if (!Sprite::LoadTexture(6, "BaseAreaMinimap.png")) { assert(0); return; } // Minimap texture
	if (!Sprite::LoadTexture(7, "PlayerMinimapSprite.png")) { assert(0); return; } // Player minimap texture
	if (!Sprite::LoadTexture(8, "EnemyMinimapSprite.png")) { assert(0); return; } // Enemy minimap texture
	if (!Sprite::LoadTexture(10, "BlackScreen.png")) { assert(0); return; } // Black Screen
	if (!Sprite::LoadTexture(11, "EnemyHumanHPBar.png")) { assert(0); return; } // Enemy HP Bar
	if (!Sprite::LoadTexture(12, "EnemyHumanHPBarFrame.png")) { assert(0); return; } // Enemy HP Bar Frame
	if (!Sprite::LoadTexture(13, "DamageOverlay.png")) { assert(0); return; } // Damage Overlay
	if (!Sprite::LoadTexture(14, "Heal.png")) { assert(0); return; } // Heal Graphic
	if (!Sprite::LoadTexture(15, "HealK.png")) { assert(0); return; } // Heal Graphic
	if (!Sprite::LoadTexture(16, "HealC.png")) { assert(0); return; } // Heal Graphic

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);

	// Sprite generation
	HPBarSPRITE = Sprite::Create(1, { 25.0f, 25.0f });
	HPBarFrameSPRITE = Sprite::Create(2, { 25.0f, 25.0f });
	STBarSPRITE = Sprite::Create(3, { 25.0f, 55.0f });
	STBarFrameSPRITE = Sprite::Create(4, { 25.0f, 55.0f });
	baseAreaMissionSPRITE = Sprite::Create(5, { 1150.0f, 100.0f });
	baseAreaMinimapSPRITE = Sprite::Create(6, { 50.0f , 430.0f });
	baseAreaMinimapPlayerSPRITE = Sprite::Create(7, { 0.0f, 0.0f });
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaMinimapEnemySPRITE[i] = Sprite::Create(8, { 0.0f, 0.0f });
		/*baseAreaEnemyHPBarSPRITE[i] = Sprite::Create(11, { 0.0f, 0.0f });
		baseAreaEnemyHPBarFrameSPRITE[i] = Sprite::Create(12, { 0.0f, 0.0f });*/
	}
	fadeSPRITE = Sprite::Create(10, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
	baseAreaDamageOverlaySPRITE = Sprite::Create(13, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
	healSPRITE = Sprite::Create(14, { 1102.0f, 542.0f });
	healKeyboardSPRITE = Sprite::Create(15, { 1102.0f, 542.0f });
	healControllerSPRITE = Sprite::Create(16, { 1102.0f, 542.0f });
	// Resizing mission sprite
	baseAreaMissionSPRITE->SetSize({ 100.0f, 80.0f });

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);
}

void BaseArea::thread2()
{
	// 3D Object creation
	skydomeOBJ = Object3d::Create();
	for (int i = 0; i < 11; i++) { attackRangeOBJ[i] = Object3d::Create(); }
	for (int i = 0; i < 10; i++) { landingPositionOBJ[i] = Object3d::Create(); }
	for (int i = 0; i < 10; i++) { enemyVisionRangeOBJ[i] = Object3d::Create(); }
	playerPositionOBJ = PlayerPositionObject::Create();
	for (int i = 0; i < 10; i++) { baseAreaEnemyPositionOBJ[i] = PlayerPositionObject::Create(); }

	// Player initialization
	playerFBX = new Player;
	playerFBX->Initialize();

	// Enemy initialization
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaEnemyFBX[i] = new EnemyHuman;
		baseAreaEnemyFBX[i]->Initialize();
		baseAreaEnemyFBX[i]->SetPosition(baseAreaEnemySpawnXMFLOAT3[i]);
		baseAreaEnemyFBX[i]->SetHomePosition({ baseAreaEnemySpawnXMFLOAT3[i].x, baseAreaEnemySpawnXMFLOAT3[i].z });
	}

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);

	// Camera initial values
	camera->SetTarget(playerFBX->GetPosition());
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(48.0f);

	// Attack range initial values
	for (int i = 0; i < numberOfEnemiesTotal + 1; i++)
	{
		if (i == 0)
		{
			attackRangeOBJ[i]->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + 0.5f, playerFBX->GetPosition().z + 5.0f });
		}
		else
		{
			attackRangeOBJ[i]->SetPosition({ baseAreaEnemyFBX[i - 1]->GetPosition().x, baseAreaEnemyFBX[i - 1]->GetPosition().y + 0.5f, baseAreaEnemyFBX[i - 1]->GetPosition().z + 5.0f });
		}
		attackRangeOBJ[i]->SetScale({ 15, 1, 15 });
	}

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);

	// Vision range initial values
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		enemyVisionRangeOBJ[i]->SetScale({ 90, 1, 90 });
	}

	// Skydome scale
	skydomeOBJ->SetScale({ 6,6,6 });

	// Position Object initial positions
	playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, 20.0f, playerFBX->GetPosition().z });
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, 30.0f, baseAreaEnemyFBX[i]->GetPosition().z });
	}

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);
}

void BaseArea::thread3()
{
	// Model creation
	skydomeMODEL = Model::CreateFromOBJ("skydome");
	groundMODEL = Model::CreateFromOBJ("Landscape2");
	tutorialGroundMODEL = Model::CreateFromOBJ("TutorialStage");
	extendedGroundMODEL = Model::CreateFromOBJ("ground");
	positionMODEL = Model::CreateFromOBJ("yuka");
	attackRangeMODEL = Model::CreateFromOBJ("yuka");
	visionRangeMODEL = Model::CreateFromOBJ("yuka");

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);

	// Setting 3D model
	skydomeOBJ->SetModel(skydomeMODEL);
	for (int i = 0; i < numberOfEnemiesTotal + 1; i++) { attackRangeOBJ[i]->SetModel(attackRangeMODEL); }
	for (int i = 0; i < numberOfEnemiesTotal; i++) { landingPositionOBJ[i]->SetModel(attackRangeMODEL); landingPositionOBJ[i]->SetScale({ 24.0f, 1.0f, 24.0f }); }
	for (int i = 0; i < numberOfEnemiesTotal; i++) { enemyVisionRangeOBJ[i]->SetModel(visionRangeMODEL); }
	playerPositionOBJ->SetModel(positionMODEL);
	for (int i = 0; i < numberOfEnemiesTotal; i++) { baseAreaEnemyPositionOBJ[i]->SetModel(positionMODEL); }

	// Touchable object creation
	groundOBJ = TouchableObject::Create(groundMODEL);
	tutorialGroundOBJ = TouchableObject::Create(tutorialGroundMODEL);
	extendedGroundOBJ = TouchableObject::Create(extendedGroundMODEL);

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);

	// Ground scale
	groundOBJ->SetScale({ 2,0.5f,2 });
	tutorialGroundOBJ->SetScale({ 50, 10, 50 });
	extendedGroundOBJ->SetScale({ 1000, 1, 1000 });

	// Ground positions
	groundOBJ->SetPosition({ 0, -15, 0 });
	tutorialGroundOBJ->SetPosition({ 0.0f, 18.0f, -1450.0f });
	extendedGroundOBJ->SetPosition({ 0, -10, 0 });

	srand((unsigned int)time(NULL));

	loadingProgress += 10.0f;
	//Loading::addLoadingPercent(10.0f);
}

void BaseArea::thread4()
{

}