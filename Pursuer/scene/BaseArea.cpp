#include "BaseArea.h"

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;
extern float loadingProgress;
extern std::atomic<int> loadingPercent;

extern int keyOrMouse;
extern float degreeTransfer = 0.0f;
extern bool lockOnActive;

extern int agroodEnemies = 0;
extern int debugJetStream = 0;
extern int participatingJetStream = 0;

extern HWND hwndE;

BaseArea::BaseArea()
{
}

BaseArea::~BaseArea()
{
	// Safe Delete
	safe_delete(camera);
	collisionManager = nullptr;
	particleMan = nullptr;
	particleManExplosion = nullptr;
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
	for (int i = 0; i < numberOfEnemiesTotal + 1; i++) { safe_delete(attackRangeOBJ[i]); }
	for (int i = 0; i < numberOfEnemiesTotal; i++) { safe_delete(enemyVisionRangeOBJ[i]); }
	safe_delete(playerPositionOBJ);
	for (int i = 0; i < numberOfEnemiesTotal; i++) { safe_delete(baseAreaEnemyPositionOBJ[i]); }
	safe_delete(groundOBJ);
	safe_delete(extendedGroundOBJ);
	safe_delete(playerFBX);
	for (int i = 0; i < numberOfEnemiesTotal; i++) { safe_delete(baseAreaEnemyFBX[i]); }
	safe_delete(baseAreaMinimapSPRITE);
	safe_delete(baseAreaMinimapPlayerSPRITE);
	for (int i = 0; i < numberOfEnemiesTotal; i++) { safe_delete(baseAreaMinimapEnemySPRITE[i]); }
	for (int i = 0; i < numberOfEnemiesTotal; i++) { safe_delete(baseAreaEnemyHPBarSPRITE[i]); }
	for (int i = 0; i < numberOfEnemiesTotal; i++) { safe_delete(baseAreaEnemyHPBarFrameSPRITE[i]); }
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
	particleMan = ParticleManager::Create(dxCommon->GetDevice(), camera, L"Resources/Sprite/effect1.png");
	particleManExplosion = ParticleManager::Create(dxCommon->GetDevice(), camera, L"Resources/Sprite/effect2.png");

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

	loadingPercent++;

	std::thread th1(&BaseArea::thread1, this); // 2D Initialization
	th1.detach();
	std::thread th2(&BaseArea::thread2, this); // 3D Initialization (other than touchable objects)
	th2.detach();
	std::thread th3(&BaseArea::thread3, this); // Model and Touchable Object Initialization
	th3.detach();
	//th1.join();
	//th2.join();
	//th3.join();

	while (loadingPercent.load() < 10)
	{

	}

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
	// Debugging only
	/*for (int i = 0; i < 10; i++)
	{
		baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::STAND);
		baseAreaEnemyFBX[i]->timer = 0.0f;
	}*/

	lightGroup->Update();
	particleMan->Update();
	particleManExplosion->Update();

	// Debug Text string
	std::ostringstream missionTracker;
	std::ostringstream healTracker;

	if (!BaseAreaConditionals::IsScreenShaking(screenShake))
	{
		if (!camera->IsCameraLockedOn() && !playerFBX->IsPlayerDodging())
		{
			camera->SetTarget({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + playerFBX->playerCameraYOffset, playerFBX->GetPosition().z });
		}
		else if (playerFBX->IsPlayerDodging())
		{
			camera->SetTarget({ playerFBX->dodgePosition.x, playerFBX->dodgePosition.y + playerFBX->playerCameraYOffset, playerFBX->dodgePosition.z });
		}
		camera->Update();
	}

#pragma region openingCutscene
	if (!BaseAreaConditionals::IsGameStarting(gameStart) && BaseAreaConditionals::IsInitializationFinished(initializeFinished))
	{
		if (BaseAreaConditionals::ShouldFadeInBegin(startTimer, startTimerLimit))
		{
			fadeSpriteALPHA -= fadeSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

			fadeSPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, fadeSpriteALPHA });

			if (BaseAreaConditionals::IsFadeSpriteAlphaBelowZero(fadeSpriteALPHA, minAlpha))
			{
				fadeSpriteALPHA = minAlpha;
				fadeSPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, fadeSpriteALPHA });
				gameStart = true;
			}
		}
		else
		{
			startTimer += startTimerIntegral * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}
	}

	if (BaseAreaConditionals::IsBaseAreaOpeningCutscenePlaying(playerFBX->baseAreaOpeningCutscene, initializeFinished, startTimer, startTimerLimit))
	{
		playerFBX->SetEnumStatus(Player::WALK);
		playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, playerFBX->GetPosition().z + deltaTimeOneSecond * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) });
		if (playerFBX->MissionSpriteTriggerPositionReached(movementStartZPosition))
		{
			startMissionSpriteMovement = true;
		}
		if (BaseAreaConditionals::IsOpeningCutsceneFinished(playerFBX->GetPosition().z, mapBorder))
		{
			playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, -mapBorder });
			playerFBX->baseAreaOpeningCutscene = false;
		}
	}
#pragma endregion

	objectPosition = playerFBX->GetPosition();
	objectRotation = playerFBX->GetRotation();

	skydomeOBJ->SetPosition(objectPosition);

#pragma region DebugAttackRange
	attackRangeOBJ[0]->SetPosition({ (objectPosition.x + (sinf(XMConvertToRadians(objectRotation.y)) * attackRangeRadius)), objectPosition.y + 0.5f, (objectPosition.z + (cosf(XMConvertToRadians(objectRotation.y)) * attackRangeRadius)) });
	attackRangeOBJ[0]->SetRotation(objectRotation);

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		attackRangeOBJ[i + 1]->SetPosition({ (baseAreaEnemyFBX[i]->GetPosition().x + (sinf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * attackRangeRadius)), baseAreaEnemyFBX[i]->GetPosition().y + attackRangeYOffset, (baseAreaEnemyFBX[i]->GetPosition().z + (cosf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * attackRangeRadius)) });
		attackRangeOBJ[i + 1]->SetRotation(baseAreaEnemyFBX[i]->GetRotation());
		landingPositionOBJ[i]->SetPosition(baseAreaEnemyFBX[i]->landingAttackPosition);
	}
#pragma endregion

#pragma region VisionRange
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		enemyVisionRangeOBJ[i]->SetPosition({ (baseAreaEnemyFBX[i]->GetPosition().x + (sinf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * visionRangeRadius)), baseAreaEnemyFBX[i]->GetPosition().y + visionRangeYOffset, (baseAreaEnemyFBX[i]->GetPosition().z + (cosf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * visionRangeRadius)) });
		enemyVisionRangeOBJ[i]->SetRotation(baseAreaEnemyFBX[i]->GetRotation());
	}
#pragma endregion

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (!firstRun)
		{
			continue;
		}

		if (baseAreaEnemyFBX[i]->IsBackPatrolPosition())
		{
			baseAreaEnemyFBX[i]->SetTimer(119.0f);
		}
	}

	if (firstRun)
	{
		firstRun = false;
	}

	// This code aggros the patrol's partner if one of them starts combat with the player
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		bool isWanderingOrStanding = baseAreaEnemyFBX[i]->IsEnemyWandering() || baseAreaEnemyFBX[i]->IsEnemyStanding();
		
		if (!isWanderingOrStanding)
		{
			continue;
		}

		int partnerIndex = (i % 2 == 0 || i == 0) ? i + 1 : i - 1;

		bool isPartnerInCombat = baseAreaEnemyFBX[partnerIndex]->IsEnemyAggro() || baseAreaEnemyFBX[partnerIndex]->IsEnemyAttacking() || baseAreaEnemyFBX[partnerIndex]->IsEnemyCoolingDown();
		
		if (!isPartnerInCombat)
		{
			continue;
		}

		baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
	}

#pragma region EnemyAggro
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (baseAreaEnemyFBX[i]->ShouldEnemyFlee(fleeHP, enemyKnockback))
		{
			BaseAreaConditionals::HandleEnemyFleeTarget(baseAreaEnemyFBX[i], i, numberOfEnemiesTotal, closestEnemyDefaultNumber, baseAreaEnemyFBX);

			BaseAreaConditionals::RunToAlly(baseAreaEnemyFBX, i, fleeSpeed, yRotationOffset);
			
			if (BaseAreaConditionals::IsEnemyCloseEnoughToCall(Distance(baseAreaEnemyFBX[i]->GetPosition(), baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->GetPosition()), fleeHelpCallRange, baseAreaEnemyFBX[i]->helpCall))
			{
				BaseAreaConditionals::CallAlly(baseAreaEnemyFBX, i);
			}
		}
		else if (BaseAreaConditionals::ShouldEnemyDoJetStreamAttack(agroodEnemies, baseAreaEnemyFBX[i]->jetStreamAttackRequiredEnemyNumber, baseAreaEnemyFBX[i]->debugJetAttacked, baseAreaEnemyFBX[i]->jetStreamCounted, baseAreaEnemyFBX[i]->enumStatus))
		{
			debugJetStream++;
			baseAreaEnemyFBX[i]->jetStreamCounted = true;
		}
		else if (baseAreaEnemyFBX[i]->CanSeePlayer(intersect(playerFBX->GetPosition(), enemyVisionRangeOBJ[i]->GetPosition(), playerInteresectSize, enemyAggroVisionRange, enemyAggroVisionRange), enemyKnockback, baseAreaEnemyAliveBOOL[i]))
		{
			BaseAreaConditionals::EnemyAggro(baseAreaEnemyFBX, playerFBX, i, maxChargeDistance, minChargeDistance, enemyKnockback, baseAreaEnemyAliveBOOL, agroodEnemies);
		}

		if (baseAreaEnemyFBX[i]->CurrentlyParticleAttackingNoStage())
		{
			ParticleCreation(baseAreaEnemyFBX[i]->particleAttackPosition.x, baseAreaEnemyFBX[i]->particleAttackPosition.y, baseAreaEnemyFBX[i]->particleAttackPosition.z, particleLifeStandard, particleAttackOffset, particleAttackStartScale);
		}

		if (baseAreaEnemyFBX[i]->HasHitGroundDuringLandingAttack())
		{
			ParticleCreationExplosion(baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z, particleLifeStandard, landingAttackOffset, landingAttackStartScale);
		}

		if (baseAreaEnemyFBX[i]->IsChargeAttacking() && baseAreaEnemyFBX[i]->IsJetStreamAttacking())
		{
			ParticleCreation(baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z, particleLifeHalf, chargeAttackOffset, chargeAttackScale);
		}

		if (!baseAreaEnemyFBX[i]->IsSurroundAttacking())
		{
			continue;
		}

		bool createParticles = false;

		if (baseAreaEnemyFBX[i]->IsFrontPatrolPosition())
		{
			createParticles = BaseAreaConditionals::CreateParticlesAtFrontPatrolPosition(baseAreaEnemyFBX[i]->nextDegree, baseAreaEnemyFBX[i]->initialDegree);
		}
		else
		{
			createParticles = BaseAreaConditionals::CreateParticlesAtBackPatrolPosition(baseAreaEnemyFBX[i]->nextDegree, baseAreaEnemyFBX[i]->initialDegree);
		}

		if (!createParticles)
		{
			continue;
		}

		auto position = baseAreaEnemyFBX[i]->GetPosition();

		if (baseAreaEnemyFBX[i]->IsFrontPatrolPosition())
		{
			ParticleCreationExplosion(position.x, position.y, position.z, particleLifeStandard, chargeAttackOffset, chargeAttackScale);
		}
		else
		{
			ParticleCreation(position.x, position.y, position.z, particleLifeHalf, chargeAttackOffset, chargeAttackScale);
		}
	}
#pragma endregion

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (!baseAreaEnemyFBX[i]->IsEnemyStanding() || !baseAreaEnemyFBX[i]->IsEnemyWandering())
		{
			continue;
		}

		BaseAreaConditionals::EnemySeeEnemyAggroCondition(baseAreaEnemyFBX, i, enemyVisionRangeOBJ, playerInteresectSize, enemyAggroVisionRange, baseAreaEnemyAliveBOOL, numberOfEnemiesTotal);
	}

	if (!BaseAreaConditionals::CanJetStreamAttackBeDone(debugJetStream, baseAreaEnemyFBX[0]->jetStreamAttackRequiredEnemyNumber, enemyDefeated, clearCondition))
	{
		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			if (!BaseAreaConditionals::CanJetStreamAttackBeDone(debugJetStream, baseAreaEnemyFBX[0]->jetStreamAttackRequiredEnemyNumber, enemyDefeated, clearCondition))
			{
				continue;
			}

			if (BaseAreaConditionals::CanEnemyBeJetStreamAttacked(baseAreaEnemyFBX[i]))
			{
				baseAreaEnemyFBX[i]->debugJetAttacked = true;
				baseAreaEnemyFBX[i]->jetStreamAttackStage = baseAreaEnemyFBX[i]->jetStreamAttackStageReset;
				baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::JETSTREAMATTACK);
			}
		}
	}

#pragma region playerHPDamage
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (!baseAreaEnemyFBX[i]->IsEnemyAttacking())
		{
			continue;
		}

		if (baseAreaEnemyFBX[i]->DidEnemyHitPlayerWithNormalAttack(intersect(attackRangeOBJ[i + 1]->GetPosition(), playerFBX->GetPosition(), playerInteresectSize, baseAttackRange, baseAttackRange), baseAreaEnemyFBX[i]->attackDamagePossible) && BaseAreaConditionals::IsEnemyAliveBOOLIAN(baseAreaEnemyAliveBOOL[i]) && BaseAreaConditionals::CanEnemyDamagePlayer(baseAreaEnemyFBX[i]->ableToDamage))
		{
			baseAreaEnemyFBX[i]->ableToDamage = false;
			damageOverlaySpriteALPHA = maxAlpha;
			damageOverlayDisplay = true;
			screenShake = true;
			baseAreaDamageOverlaySPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, damageOverlaySpriteALPHA });
			playerFBX->hp -= baseAttackDamage;
			playerFBX->damageAdvantage = true;
			playerFBX->SetEnumStatus(Player::DAMAGED);
		}

		if (baseAreaEnemyFBX[i]->DidEnemyHitPlayerWithParticleAttack(intersect(baseAreaEnemyFBX[i]->particleAttackPosition, playerFBX->GetPosition(), playerInteresectSize, particleAttackRange, particleAttackRange), baseAreaEnemyFBX[i]->particleAttackActive) && BaseAreaConditionals::CanEnemyDamagePlayer(baseAreaEnemyFBX[i]->ableToDamage))
		{
			baseAreaEnemyFBX[i]->ableToDamage = false;
			damageOverlaySpriteALPHA = maxAlpha;
			damageOverlayDisplay = true;
			screenShake = true;
			baseAreaDamageOverlaySPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, damageOverlaySpriteALPHA });
			playerFBX->hp -= particleAttackDamage;
			playerFBX->damageAdvantage = true;
			playerFBX->SetEnumStatus(Player::DAMAGED);
			knockback = true;
		}

		if (baseAreaEnemyFBX[i]->DidEnemyHitPlayerDuringSurroundAttack(intersect(baseAreaEnemyFBX[i]->GetPosition(), playerFBX->GetPosition(), playerInteresectSize, twoEnemySurroundRange, twoEnemySurroundRange), baseAreaEnemyFBX[i]->enumStatus, baseAreaEnemyFBX[i]->twoEnemySurroundStage) && BaseAreaConditionals::CanEnemyDamagePlayer(baseAreaEnemyFBX[i]->ableToDamage))
		{
			baseAreaEnemyFBX[i]->ableToDamage = false;
			damageOverlaySpriteALPHA = maxAlpha;
			damageOverlayDisplay = true;
			screenShake = true;
			baseAreaDamageOverlaySPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, damageOverlaySpriteALPHA });
			playerFBX->hp -= twoEnemySurroundDamage;
			playerFBX->damageAdvantage = true;
			playerFBX->SetEnumStatus(Player::DAMAGED);
			knockback = true;
		}

		if (baseAreaEnemyFBX[i]->DidEnemyHitPlayerDuringLandingAttack(intersect(baseAreaEnemyFBX[i]->landingAttackPosition, playerFBX->GetPosition(), playerInteresectSize, landingAttackRange, landingAttackRange), baseAreaEnemyFBX[i]->landingParticles) && BaseAreaConditionals::CanEnemyDamagePlayer(baseAreaEnemyFBX[i]->ableToDamage))
		{
			baseAreaEnemyFBX[i]->ableToDamage = false;
			damageOverlaySpriteALPHA = maxAlpha;
			damageOverlayDisplay = true;
			screenShake = true;
			baseAreaDamageOverlaySPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, damageOverlaySpriteALPHA });
			playerFBX->hp -= landingAttackDamage;
			playerFBX->damageAdvantage = true;
			playerFBX->SetEnumStatus(Player::DAMAGED);
			knockback = true;
		}

		if (baseAreaEnemyFBX[i]->DidEnemyHitPlayerWhileCharging(intersect(baseAreaEnemyFBX[i]->GetPosition(), playerFBX->GetPosition(), playerInteresectSize, chargeAttackRange, chargeAttackRange), baseAreaEnemyFBX[i]->enumStatus, baseAreaEnemyFBX[i]->chargeAttackStage, baseAreaEnemyFBX[i]->jetStreamAttackStage) && BaseAreaConditionals::CanEnemyDamagePlayer(baseAreaEnemyFBX[i]->ableToDamage))
		{
			baseAreaEnemyFBX[i]->ableToDamage = false;
			damageOverlaySpriteALPHA = maxAlpha;
			damageOverlayDisplay = true;
			screenShake = true;
			baseAreaDamageOverlaySPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, damageOverlaySpriteALPHA });
			playerFBX->hp -= chargeAttackDamage;
			playerFBX->damageAdvantage = true;
			playerFBX->SetEnumStatus(Player::DAMAGED);
			knockback = true;
		}

		if (BaseAreaConditionals::IsEnemyOrPlayerBeingKnockbacked(knockback)) // This case is the player getting knockbacked
		{
			XMFLOAT3 xyz = baseAreaEnemyFBX[i]->GetPosition() - playerFBX->GetPosition();
			XMFLOAT3 knockbackPrevPosition = playerFBX->GetPosition();
			float hypotenuse = sqrt((xyz.x * xyz.x) + (xyz.z * xyz.z));
			switch (baseAreaEnemyFBX[i]->enumStatus)
			{
			case EnemyHuman::PARTICLEATTACK:
				playerFBX->SetPosition({
					knockbackPrevPosition.x -= particleAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += knockbackYOffset,
					knockbackPrevPosition.z -= particleAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
				break;
			case EnemyHuman::LANDINGATTACK:
				playerFBX->SetPosition({
					knockbackPrevPosition.x -= landingAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += knockbackYOffset,
					knockbackPrevPosition.z -= landingAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
				break;
			case EnemyHuman::CHARGEATTACK:
				playerFBX->SetPosition({
					knockbackPrevPosition.x += chargeAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += knockbackYOffset,
					knockbackPrevPosition.z += chargeAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
				break;
			case EnemyHuman::JETSTREAMATTACK:
				playerFBX->SetPosition({
					knockbackPrevPosition.x += chargeAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += knockbackYOffset,
					knockbackPrevPosition.z += chargeAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
				break;
			case EnemyHuman::TWOENEMYSURROUND:
				playerFBX->SetPosition({
					knockbackPrevPosition.x += twoEnemySurroundKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += knockbackYOffset,
					knockbackPrevPosition.z += twoEnemySurroundKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
				break;
			}

			knockbackTime += knockbackInterval * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

			if (BaseAreaConditionals::HasKnockbackFinished(knockbackTime, knockbackMaxTime))
			{
				knockbackTime = knockbackTimeReset;
				knockback = false;
			}
		}

		baseAreaEnemyFBX[i]->landingParticles = false;
	}

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (BaseAreaConditionals::ShouldEnemyAttackToTakeAdvantageOfPlayerTakingDamage(playerFBX->damageAdvantage) && BaseAreaConditionals::IsEnemyReadyToTakeAdvantage(baseAreaEnemyFBX[i]))
		{
			BaseAreaConditionals::DecideAndExecuteAttack(baseAreaEnemyFBX[i], playerFBX, damageAdvantageShortLongRangeBorder);
		}
	}

	playerFBX->damageAdvantage = false;

	if (playerFBX->HasPlayerHPReachedZero())
	{
		playerFBX->hp = playerFBX->defeatHP;
		playerFBX->SetEnumStatus(Player::DEAD);
	}
	
	if (playerFBX->IsPlayerDead())
	{
		fadeSpriteALPHA += fadeSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		fadeSPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, fadeSpriteALPHA });
		if (BaseAreaConditionals::IsFadeSpriteAlphaAboveOne(fadeSpriteALPHA, maxAlpha))
		{
			fadeSpriteALPHA = maxAlpha;
			audio->StopWave("MainArea.wav");
			result = defeat;
			deletion = true;
		}
	}

	if (BaseAreaConditionals::IsScreenShaking(screenShake))
	{
		camera->SetTarget(ScreenShake({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + playerFBX->playerCameraYOffset, playerFBX->GetPosition().z }));
		camera->Update();
		shakeTimer += shakeTimerInterval * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		if (BaseAreaConditionals::HasScreenShakingFinished(shakeTimer, maxShakeTimer))
		{
			shakeTimer = shakeTimerReset;
			screenShake = false;
		}
	}

	// Damage overlay display
	if (BaseAreaConditionals::ShouldTheDamageOverlayDisplay(damageOverlayDisplay))
	{
		if (!playerFBX->IsPlayerDead())
		{
			damageOverlaySpriteALPHA -= fadeSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}

		baseAreaDamageOverlaySPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, damageOverlaySpriteALPHA });

		if (BaseAreaConditionals::IsFadeSpriteAlphaBelowZero(damageOverlaySpriteALPHA, minAlpha))
		{
			damageOverlayDisplay = false;
		}
	}
#pragma endregion

#pragma region playerAttack
	if (playerFBX->IsPlayerParrying()) // Player parry
	{
		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			if (!BaseAreaConditionals::AreBothPlayerAndEnemyInParryRange(intersect(playerFBX->GetPosition(), enemyVisionRangeOBJ[i]->GetPosition(), playerInteresectSize, enemyAggroVisionRange, enemyAggroVisionRange), intersect(attackRangeOBJ[i + 1]->GetPosition(), playerFBX->GetPosition(), playerInteresectSize, baseAttackRange, baseAttackRange)))
			{
				continue;
			}

			if (!BaseAreaConditionals::IsEnemyAliveBOOLIAN(baseAreaEnemyAliveBOOL[i]))
			{
				continue;
			}

			if (!BaseAreaConditionals::IsPlayerParryTimingCorrect(baseAreaEnemyFBX[i]->attackTimer, baseAreaEnemyFBX[i]->parryPossibleMin, baseAreaEnemyFBX[i]->parryPossibleMax))
			{
				continue;
			}
			
			baseAreaEnemyFBX[i]->PlayerSuccessfulParry();
			ParticleCreationExplosion((playerFBX->GetPosition().x + baseAreaEnemyFBX[i]->GetPosition().x) / 2.0f, (playerFBX->GetPosition().y + baseAreaEnemyFBX[i]->GetPosition().y) / 2.0f, (playerFBX->GetPosition().z + baseAreaEnemyFBX[i]->GetPosition().z) / 2.0f, particleLifeStandard, parryParticleOffset, parryParticleStartScale);
			playerFBX->parryActive = true;
		}
	}

	if (BaseAreaConditionals::IsPlayerAttacking(playerFBX->enumStatus))
	{
		if (BaseAreaConditionals::IsPlayerAttackTimingCorrect(playerFBX->timer, playerFirstAttackStartTimer, playerFirstAttackEndTimer, playerSecondAttackStartTimer, playerSecondAttackEndTimer, playerThirdAttackStartTimer, playerThirdAttackEndTimer)
			&& BaseAreaConditionals::CanPlayerDamageEnemy(playerFBX->ableToDamage))
		{
			for (int i = 0; i < numberOfEnemiesTotal; i++)
			{
				if (!BaseAreaConditionals::WillPlayerAttackHit(intersect(attackRangeOBJ[0]->GetPosition(), baseAreaEnemyFBX[i]->GetPosition(), playerInteresectSize, playerAttackRange, playerAttackRange)) && BaseAreaConditionals::IsEnemyAliveBOOLIAN(baseAreaEnemyAliveBOOL[i]) && BaseAreaConditionals::WillPlayerAttackHitBasedOnYPosition(abs(baseAreaEnemyFBX[i]->GetPosition().y - playerFBX->GetPosition().y), 4.0f))
				{
					continue;
				}

				baseAreaEnemyFBX[i]->AttackedAndDamaged();

				if (BaseAreaConditionals::DoesPlayerKnockbackAttackHit(playerFBX->timer, playerThirdAttackStartTimer, playerThirdAttackEndTimer))
				{
					enemyKnockbackTime = knockbackTimeReset;
					enemyKnockback = true;
				}

				if (!BaseAreaConditionals::IsEnemyAliveHP(baseAreaEnemyFBX[i]->HP, baseAreaEnemyFBX[i]->minAliveHP))
				{
					baseAreaEnemyAliveBOOL[i] = false;
					enemyDefeated++;
					agroodEnemies--;
					baseAreaEnemyFBX[i]->dead = true;
					baseAreaEnemyFBX[i]->modelChange = true;
					//baseAreaEnemyRespawnTimerFLOAT[i] = 600;
					baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::DEAD);
				}
				else
				{
					baseAreaEnemyFBX[i]->EnemyDamaged();
				}

				if (BaseAreaConditionals::DoesPlayerKnockbackAttackHit(playerFBX->timer, playerThirdAttackStartTimer, playerThirdAttackEndTimer))
				{
					ParticleCreation(baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z, particleLifeStandardPlusHalf, playerKnockbackAttackOffset, playerKnockbackAttackStartScale * 2.0f);
				}
				else
				{
					ParticleCreation(baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z, particleLifeStandard, playerKnockbackAttackOffset, playerKnockbackAttackStartScale);
				}

				playerFBX->ableToDamage = false;
			}
		}

		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			if (!BaseAreaConditionals::IsEnemyOrPlayerBeingKnockbacked(enemyKnockback))
			{
				continue;
			}

			XMFLOAT3 xyz = playerFBX->GetPosition() - baseAreaEnemyFBX[i]->GetPosition();
			XMFLOAT3 knockbackPrevPosition = baseAreaEnemyFBX[i]->GetPosition();

			float hypotenuse = sqrt((xyz.x * xyz.x) + (xyz.z * xyz.z));

			baseAreaEnemyFBX[i]->SetPosition({
					knockbackPrevPosition.x -= playerAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
					knockbackPrevPosition.y += knockbackYOffset,
					knockbackPrevPosition.z -= playerAttackKnockbackDistance * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });

			enemyKnockbackTime += playerAttackKnockbackInterval * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

			if (BaseAreaConditionals::HasKnockbackFinished(enemyKnockbackTime, playerAttackKnockbackMaxtime))
			{
				enemyKnockbackTime = knockbackTimeReset;
				enemyKnockback = false;
			}
		}

		if (BaseAreaConditionals::ShouldPlayerAbilityToDamageReset(playerFBX->timer, playerFirstAttackEndTimer, playerSecondAttackStartTimer, playerSecondAttackEndTimer, playerThirdAttackStartTimer))
		{
			playerFBX->ableToDamage = true;
		}
	}
	else
	{
		playerFBX->ResetPlayerAttackCombo();
	}
	
#pragma endregion

#pragma region playerHeal
	if (playerFBX->IsPlayerHealing())
	{
		if (playerFBX->ShouldHealParticleRingSpawn(healFirstParticleRingTimer))
		{
			for (int i = 0; i < healNumberOfParticles; i++)
			{
				ParticleCreationHeal(playerFBX->healParticlePosition[0].x + cosf(XMConvertToRadians(i * 2.0f)) * healParticleRadius,
					playerFBX->healParticlePosition[0].y,
					playerFBX->healParticlePosition[0].z + sinf(XMConvertToRadians(i * 2.0f)) * healParticleRadius, particleHealLife, healParticleOffset, healParticleStartScale);
			}
		}
		if (playerFBX->ShouldHealParticleRingSpawn(healSecondParticleRingTimer))
		{
			for (int i = 0; i < healNumberOfParticles; i++)
			{
				ParticleCreationHeal(playerFBX->healParticlePosition[1].x + cosf(XMConvertToRadians(i * 2.0f)) * healParticleRadius,
					playerFBX->healParticlePosition[1].y,
					playerFBX->healParticlePosition[1].z + sinf(XMConvertToRadians(i * 2.0f)) * healParticleRadius, particleHealLife, healParticleOffset, healParticleStartScale);
			}
		}
		if (playerFBX->ShouldHealParticleRingSpawn(healThirdParticleRingTimer))
		{
			for (int i = 0; i < healNumberOfParticles; i++)
			{
				ParticleCreationHeal(playerFBX->healParticlePosition[2].x + cosf(XMConvertToRadians(i * 2.0f)) * healParticleRadius,
					playerFBX->healParticlePosition[2].y,
					playerFBX->healParticlePosition[2].z + sinf(XMConvertToRadians(i * 2.0f)) * healParticleRadius, particleHealLife, healParticleOffset, healParticleStartScale);
			}
		}
	}
#pragma endregion

#pragma region aggroEveryEnemyWhenOneKillAwayFromWinning
	if (BaseAreaConditionals::ShouldEveryEnemyAggro(enemyDefeated, clearCondition))
	{
		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			if (baseAreaEnemyFBX[i]->IsEnemyAlive())
			{
				continue;
			}

			if (!baseAreaEnemyFBX[i]->IsEnemyStanding() && !baseAreaEnemyFBX[i]->IsEnemyWandering())
			{
				continue;
			}

			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
		}
	}
#pragma endregion

#pragma region clearCondition
	if (BaseAreaConditionals::HasPlayerReachedGameClearThreshold(enemyDefeated, clearCondition))
	{
		enemyDefeated = clearCondition;

		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::DEAD);
		}

		fadeSpriteALPHA += fadeSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		fadeSPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, fadeSpriteALPHA });

		if (BaseAreaConditionals::IsFadeSpriteAlphaAboveOne(fadeSpriteALPHA, maxAlpha))
		{
			fadeSpriteALPHA = maxAlpha;
			audio->StopWave("MainArea.wav");
			result = victory;
			deletion = true;
		}
	}
#pragma endregion

#pragma region areaBoundaryLimits
	if (!BaseAreaConditionals::BaseAreaOpeningCutscenePlayingCheck(playerFBX->baseAreaOpeningCutscene))
	{
		if (BaseAreaConditionals::HasPlayerReachedMapBoundaries(playerFBX->GetPosition().x, mapBorder))
		{
			playerFBX->SetPosition({ mapBorder, playerFBX->GetPosition().y, playerFBX->GetPosition().z });
			camera->SetTarget({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + playerFBX->playerCameraYOffset, playerFBX->GetPosition().z });
		}
		else if (BaseAreaConditionals::HasPlayerReachedMapBoundaries(playerFBX->GetPosition().x, -mapBorder))
		{
			playerFBX->SetPosition({ -mapBorder, playerFBX->GetPosition().y, playerFBX->GetPosition().z });
			camera->SetTarget({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + playerFBX->playerCameraYOffset, playerFBX->GetPosition().z });
		}

		if (BaseAreaConditionals::HasPlayerReachedMapBoundaries(playerFBX->GetPosition().z, mapBorder))
		{
			playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, mapBorder });
			camera->SetTarget({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + playerFBX->playerCameraYOffset, playerFBX->GetPosition().z });
		}
		else if (BaseAreaConditionals::HasPlayerReachedMapBoundaries(playerFBX->GetPosition().z, -mapBorder))
		{
			playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, -mapBorder });
			camera->SetTarget({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + playerFBX->playerCameraYOffset, playerFBX->GetPosition().z });
		}

		for (float i = 0.0f; i < mapBorderParticles; i++)
		{
			if (BaseAreaConditionals::ShouldMapBoundaryDotsSpawn(Distance(playerFBX->GetPosition(), { -mapBorderParticlePosition + (8 * i), playerFBX->GetPosition().y, -mapBorderParticlePosition }), mapBorderVisibleDistance))
			{
				ParticleCreationEdge(-mapBorderParticlePosition + (8 * i), playerFBX->GetPosition().y, -mapBorderParticlePosition, particleLifeHalf, mapBorderParticleOffset, mapBorderParticleStartScale);
			}

			if (BaseAreaConditionals::ShouldMapBoundaryDotsSpawn(Distance(playerFBX->GetPosition(), { -mapBorderParticlePosition + (8 * i), playerFBX->GetPosition().y, mapBorderParticlePosition }), mapBorderVisibleDistance))
			{
				ParticleCreationEdge(-mapBorderParticlePosition + (8 * i), playerFBX->GetPosition().y, mapBorderParticlePosition, particleLifeHalf, mapBorderParticleOffset, mapBorderParticleStartScale);
			}

			if (BaseAreaConditionals::ShouldMapBoundaryDotsSpawn(Distance(playerFBX->GetPosition(), { -mapBorderParticlePosition, playerFBX->GetPosition().y, -mapBorderParticlePosition + (8 * i) }), mapBorderVisibleDistance))
			{
				ParticleCreationEdge(-mapBorderParticlePosition, playerFBX->GetPosition().y, -mapBorderParticlePosition + (8 * i), particleLifeHalf, mapBorderParticleOffset, mapBorderParticleStartScale);
			}

			if (BaseAreaConditionals::ShouldMapBoundaryDotsSpawn(Distance(playerFBX->GetPosition(), { mapBorderParticlePosition, playerFBX->GetPosition().y, -mapBorderParticlePosition + (8 * i) }), mapBorderVisibleDistance))
			{
				ParticleCreationEdge(mapBorderParticlePosition, playerFBX->GetPosition().y, -mapBorderParticlePosition + (8 * i), particleLifeHalf, mapBorderParticleOffset, mapBorderParticleStartScale);
			}
		}
	}
#pragma endregion

#pragma region HPSTUpdates
	if (!BaseAreaConditionals::ShouldMissionSpriteMovementStart(startMissionSpriteMovement))
	{
		baseAreaMissionSPRITE->SetPosition({ initialBaseAreaMissionSpritePosition.x, initialBaseAreaMissionSpritePosition.y });
		baseAreaMissionSPRITE->SetSize({ initialBaseAreaMissionSpriteSize.x, initialBaseAreaMissionSpriteSize.y });
	}
	else
	{
		if (!BaseAreaConditionals::HasMissionSpriteReachedDesiredPosition(xSet) || !BaseAreaConditionals::HasMissionSpriteReachedDesiredPosition(ySet))
		{
			float x = (targetBaseAreaMissionSpritePosition.x - baseAreaMissionSPRITE->GetPosition().x);
			float y = (targetBaseAreaMissionSpritePosition.y - baseAreaMissionSPRITE->GetPosition().y);
			float hypotenuse = sqrtf((x * x) + (y * y));
			
			XMFLOAT2 newMissionPosition = baseAreaMissionSPRITE->GetPosition();
			if (BaseAreaConditionals::ShouldMissionSpriteMove(baseAreaMissionSPRITE->GetPosition().x, targetBaseAreaMissionSpritePosition.x))
			{
				newMissionPosition.x += baseAreaMissionSpriteMovementSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (x / hypotenuse);
			}
			if (BaseAreaConditionals::ShouldMissionSpriteMove(baseAreaMissionSPRITE->GetPosition().y, targetBaseAreaMissionSpritePosition.y))
			{
				newMissionPosition.y += baseAreaMissionSpriteMovementSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (y / hypotenuse);
			}
			baseAreaMissionSPRITE->SetPosition(newMissionPosition);
		}
		else
		{
			baseAreaMissionSPRITE->SetPosition({ targetBaseAreaMissionSpritePosition.x, targetBaseAreaMissionSpritePosition.y });
		}

		if (!BaseAreaConditionals::HasMissionSpriteReachedDesiredSize(sizeSet))
		{
			XMFLOAT2 missionSpriteSize = baseAreaMissionSPRITE->GetSize();
			if (BaseAreaConditionals::ShouldMissionSpriteSizeChange(baseAreaMissionSPRITE->GetSize().x, targetBaseAreaMissionSpriteSize.x))
			{
				missionSpriteSize.x -= baseAreaMissionSpriteSizeSpeed.x * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			if (BaseAreaConditionals::ShouldMissionSpriteSizeChange(baseAreaMissionSPRITE->GetSize().y, targetBaseAreaMissionSpriteSize.y))
			{
				missionSpriteSize.y -= baseAreaMissionSpriteSizeSpeed.y * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}
			baseAreaMissionSPRITE->SetSize(missionSpriteSize);
		}
		else
		{
			baseAreaMissionSPRITE->SetSize({ targetBaseAreaMissionSpriteSize.x, targetBaseAreaMissionSpriteSize.y });
		}

		if (BaseAreaConditionals::ShouldMissionSpritePositionBeSet(baseAreaMissionSPRITE->GetPosition().x, targetBaseAreaMissionSpritePosition.x))
		{
			baseAreaMissionSPRITE->SetPosition({ targetBaseAreaMissionSpritePosition.x, baseAreaMissionSPRITE->GetPosition().y });
			xSet = true;
		}
		if (BaseAreaConditionals::ShouldMissionSpritePositionBeSet(baseAreaMissionSPRITE->GetPosition().y, targetBaseAreaMissionSpritePosition.y))
		{
			baseAreaMissionSPRITE->SetPosition({ baseAreaMissionSPRITE->GetPosition().x, targetBaseAreaMissionSpritePosition.y });
			ySet = true;
		}

		if (BaseAreaConditionals::ShouldMissionSpriteSizeBeSet(baseAreaMissionSPRITE->GetSize().x, targetBaseAreaMissionSpriteSize.x))
		{
			baseAreaMissionSPRITE->SetSize({ targetBaseAreaMissionSpriteSize.x, baseAreaMissionSPRITE->GetSize().y });
		}
		if (BaseAreaConditionals::ShouldMissionSpriteSizeBeSet(baseAreaMissionSPRITE->GetSize().y, targetBaseAreaMissionSpriteSize.y))
		{
			baseAreaMissionSPRITE->SetSize({ baseAreaMissionSPRITE->GetSize().x, targetBaseAreaMissionSpriteSize.y });
		}

		if (BaseAreaConditionals::ShouldMissionSpriteSizeBeSet(baseAreaMissionSPRITE->GetSize().x, targetBaseAreaMissionSpriteSize.x) && BaseAreaConditionals::ShouldMissionSpriteSizeBeSet(baseAreaMissionSPRITE->GetSize().y, targetBaseAreaMissionSpriteSize.y))
		{
			sizeSet = true;
		}
	}
	HPBarSPRITE->SetSize({ playerFBX->hp * HPBarSpriteMultiplier, HPBarSpriteSize.y });
	HPBarFrameSPRITE->SetSize(HPBarSpriteSize);
	STBarSPRITE->SetSize({ playerFBX->stamina / 2.0f, STBarSpriteSize.y });
	STBarFrameSPRITE->SetSize(STBarSpriteSize);
	slowMotionBarSPRITE->SetSize({ playerFBX->powerRemaining / 2.0f, slowMotionSpriteBarSize.y });
	slowMotionBarFrameSPRITE->SetSize(slowMotionSpriteBarSize);
	if (camera->IsCameraLockedOn())
	{
		STBarSPRITE->SetPosition(lockOnSTBarPosition);
		STBarFrameSPRITE->SetPosition(lockOnSTBarPosition);
		staminaWarningSPRITE->SetPosition(lockOnSTWarningPosition);
		slowMotionBarSPRITE->SetPosition(lockOnSlowMotionSpritePosition);
		slowMotionBarFrameSPRITE->SetPosition(lockOnSlowMotionSpritePosition);
	}
	else
	{
		STBarSPRITE->SetPosition(standardSTBarPosition);
		STBarFrameSPRITE->SetPosition(standardSTBarPosition);
		staminaWarningSPRITE->SetPosition(standardSTWarningPosition);
		slowMotionBarSPRITE->SetPosition(standardSlowMotionSpritePosition);
		slowMotionBarFrameSPRITE->SetPosition(standardSlowMotionSpritePosition);
	}
	STBarSPRITE->SetRotation(STBarRotation);
	STBarFrameSPRITE->SetRotation(STBarRotation);
	slowMotionBarSPRITE->SetRotation(lockOnRotation);
	slowMotionBarFrameSPRITE->SetRotation(lockOnRotation);

	if (BaseAreaConditionals::ShouldStaminaBarAlphaBeFull(playerFBX->stamina, playerFBX->staminaMaximum, playerFBX->dodgeStaminaNeeded)) 
	{
		staminaSpriteAlpha = maxAlpha;
		blinkingStaminaAlpha = maxAlpha;
	}
	else if (BaseAreaConditionals::ShouldStaminaBarBeginFading(playerFBX->stamina, playerFBX->staminaMaximum)) 
	{
		BaseAreaConditionals::UpdateStaminaSpriteAlpha(staminaSpriteAlpha, staminaSpriteInteger);
	}
	else
	{
		BaseAreaConditionals::HandleBlinkingStamina(blinkingStaminaAlpha, staminaBlinkingEffect, minAlpha, maxAlpha, blinkingStaminaSpriteInteger);
	}

	if (BaseAreaConditionals::ShouldPowerBarAlphaBeFull(playerFBX->powerRemaining, playerFBX->staminaMaximum))
	{
		slowMotionSpriteALPHA = maxAlpha;
	}
	else
	{
		slowMotionSpriteALPHA -= staminaSpriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	}

	STBarSPRITE->SetColor({ maxAlpha, blinkingStaminaAlpha, blinkingStaminaAlpha, staminaSpriteAlpha });
	STBarFrameSPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, staminaSpriteAlpha });
	slowMotionBarSPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, slowMotionSpriteALPHA });
	slowMotionBarFrameSPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, slowMotionSpriteALPHA });

	if (BaseAreaConditionals::ShouldBarAlphaBeZero(staminaSpriteAlpha, minAlpha))
	{
		staminaSpriteAlpha = minAlpha;
	}

	if (BaseAreaConditionals::ShouldBarAlphaBeZero(slowMotionSpriteALPHA, minAlpha))
	{
		slowMotionSpriteALPHA = minAlpha;
	}

	staminaWarningSPRITE->SetColor({ maxAlpha, maxAlpha, maxAlpha, playerFBX->staminaWarningSpriteAlpha });

	if (BaseAreaConditionals::ShouldSprintParticlesBeCreated(input->PushKey(DIK_LSHIFT), input->PushControllerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER), playerFBX->stamina, playerFBX->staminaMinimum))
	{
		ParticleCreation(playerFBX->GetPosition().x, playerFBX->GetPosition().y, playerFBX->GetPosition().z, particleSprintLife, -sprintParticleOffset, sprintParticleStartScale);
	}

	if (BaseAreaConditionals::CanSlowMotionBeActivated(input->PushKey(DIK_CAPSLOCK), input->PushControllerButton(XINPUT_GAMEPAD_LEFT_THUMB), playerFBX->powerRemaining, playerFBX->powerMinimum))
	{
		playerFBX->slowMotion = true;

		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			baseAreaEnemyFBX[i]->slowMotion = true;
		}
	}
	else
	{
		playerFBX->slowMotion = false;

		for (int i = 0; i < numberOfEnemiesTotal; i++)
		{
			baseAreaEnemyFBX[i]->slowMotion = false;
		}
	}
#pragma endregion

#pragma region minimapUpdates
	baseAreaMinimapPlayerSPRITE->SetPosition({ playerFBX->GetPosition().x * minimapSpriteMultiplier + minimapSpriteOffset.x, playerFBX->GetPosition().z * minimapSpriteMultiplier + minimapSpriteOffset.y });
	
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaMinimapEnemySPRITE[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x * minimapSpriteMultiplier + minimapSpriteOffset.x, baseAreaEnemyFBX[i]->GetPosition().z * minimapSpriteMultiplier + minimapSpriteOffset.y });
	}
#pragma endregion

#pragma region missionTracker
	missionTracker << enemyDefeated << " / 5"
		<< std::fixed << std::setprecision(0)
		<< std::setw(2) << std::setfill('0');
	if (!playerFBX->baseAreaOpeningCutscene && xSet && ySet && sizeSet)
	{
		debugText->Print(missionTracker.str(), missionTrackerPosition.x, missionTrackerPosition.y, 1.0f);
	}
#pragma endregion

#pragma region healTracker
	healTracker << playerFBX->healRemaining
		<< std::fixed << std::setprecision(0)
		<< std::setw(2) << std::setfill('0');
	debugText->Print(healTracker.str(), healTrackerPosition.x, healTrackerPosition.y, 1.0f);
#pragma endregion

#pragma region enemyHPBar
	std::array<float, 10> hpBarX;
	std::array<float, 10> hpBarY;

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		//RECT構造体へのポインタ
		RECT rect;

		//ウィンドウの外側のサイズを取得
		GetClientRect(hwndE, &rect);

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		//// Compute the enemy's position relative to the camera
		//XMFLOAT3 relEnemy(baseAreaEnemyFBX[i]->GetPosition().x - camera->GetEye().x,
		//				  baseAreaEnemyFBX[i]->GetPosition().y - camera->GetEye().y,
		//				  baseAreaEnemyFBX[i]->GetPosition().z - camera->GetEye().z);

		//// Compute the distance between the camera and the enemy
		//float dist = sqrtf(relEnemy.x * relEnemy.x + relEnemy.y * relEnemy.y + relEnemy.z * relEnemy.z);

		//// Compute the angles between the camera's forward vector and the vector from the camera to the enemy
		//float relEnemyPhi = acosf(relEnemy.z / dist);
		//float relEnemyTheta = atan2f(relEnemy.y, relEnemy.x);

		//// Adjust the angles based on the camera's orientation
		//relEnemyTheta += camera->GetTheta();
		//relEnemyPhi += camera->GetPhi();

		//// Compute the position of the enemy in screen space
		//float halfWidth = static_cast<float>(width) / 2.0f;
		//float halfHeight = static_cast<float>(height) / 2.0f;
		//float x = halfWidth * (1.0f + relEnemyPhi / XM_PI);
		//float y = halfHeight * (1.0f - relEnemyTheta / XM_PI) + 100.0f;

		//// Adjust for top-left origin
		//x -= halfWidth;
		//y -= halfHeight;

		//// Compute the position of the enemy's HP bar on the screen
		//hpBarX[i] = x;
		//hpBarY[i] = y;

		//// Check if the enemy is behind the camera and don't render the HP bar if it is
		//XMFLOAT3 forward(camera->GetTarget().x - camera->GetEye().x, camera->GetTarget().y - camera->GetEye().y, camera->GetTarget().z - camera->GetEye().z);
		//XMVECTOR forwardVector = XMLoadFloat3(&forward);
		//XMVECTOR relEnemyVector = XMLoadFloat3(&relEnemy);
		//float dot = XMVectorGetX(XMVector3Dot(forwardVector, relEnemyVector));

		// Calculate the viewport
		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)width;
		viewport.Height = (float)height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		camera->UpdateAspectRatio((float)width, (float)height);
		camera->UpdateViewMatrix();
		camera->UpdateProjectionMatrix();

		// Convert the world coordinates to a vector
		XMVECTOR worldPosVec = XMLoadFloat3(&baseAreaEnemyFBX[i]->GetPosition());

		// Multiply the resulting view space coordinates by the view projection matrix
		XMVECTOR clipPos = XMVector4Transform(worldPosVec, camera->GetViewProjectionMatrix() * camera->GetBillboardMatrix());

		// Convert from homogeneous clip space to normalized device coordinates (NDC)
		XMVECTOR ndcPos = XMVectorDivide(clipPos, XMVectorSplatW(clipPos));

		// Multiply the resulting NDC coordinates by the viewport matrix to map them to the screen space
		float x = XMVectorGetX(ndcPos);
		float y = XMVectorGetY(ndcPos);
		float z = XMVectorGetZ(ndcPos);

		hpBarX[i] = viewport.TopLeftX + (1.0f - x / camera->GetAspectRatio()) * viewport.Width / 2.0f;
		hpBarY[i] = viewport.TopLeftY + (1.0f + y) * viewport.Height / 2.0f - 100.0f;
		float screenZ = viewport.MinDepth + z * (viewport.MaxDepth - viewport.MinDepth);
		
		//if (dot < 0.0f) {
		//	isInFront[i] = false;
		//}
		//else {
		//	isInFront[i] = true;
		//}*/

		baseAreaEnemyHPBarSPRITE[i]->SetPosition({ hpBarX[i], hpBarY[i] });
		baseAreaEnemyHPBarSPRITE[i]->SetSize({ baseAreaEnemyFBX[i]->HP * 10.0f, 10.0f });
		baseAreaEnemyHPBarFrameSPRITE[i]->SetPosition({ hpBarX[i], hpBarY[i] });
		baseAreaEnemyHPBarFrameSPRITE[i]->SetSize({ 50.0f, 10.0f });
	}
#pragma endregion

#pragma region InCaseFallingThroughWorld
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (BaseAreaConditionals::HasPlayerOrEnemyFallenThroughGround(baseAreaEnemyFBX[i]->GetPosition().y, -9.0f))
		{
			baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, 25.0f, baseAreaEnemyFBX[i]->GetPosition().z });
			baseAreaEnemyFBX[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, 25.0f, baseAreaEnemyFBX[i]->GetPosition().z });
		}
	}

	if (BaseAreaConditionals::HasPlayerOrEnemyFallenThroughGround(playerFBX->GetPosition().y, -9.0f))
	{
		playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, 18.0f, playerFBX->GetPosition().z });
		playerFBX->SetPosition({ playerFBX->GetPosition().x, 18.0f, playerFBX->GetPosition().z });
	}
#pragma endregion

#pragma region LockOn
	if (BaseAreaConditionals::CameraLockOn(input->PushKey(DIK_SPACE), input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER)))
	{
		if (!camera->IsCameraLockedOn())
		{
			float min = FLT_MAX;
			closestEnemy = 20;
			for (int i = 0; i < 10; i++)
			{
				if (!baseAreaEnemyFBX[i]->dead)
				{
					float x = baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x;
					float y = baseAreaEnemyFBX[i]->GetPosition().z - playerFBX->GetPosition().z;
					if (abs(sqrt(x * x + y * y)) < min)
					{
						min = abs(sqrt(x * x + y * y));
						closestEnemy = i;
					}
				}
			}
			camera->lockOn = true;
		}
		float x = baseAreaEnemyFBX[closestEnemy]->GetPosition().x - playerFBX->GetPosition().x;
		float z = baseAreaEnemyFBX[closestEnemy]->GetPosition().z - playerFBX->GetPosition().z;
		camera->playerRadius = abs(sqrt(x * x + z * z));
		//float x2 = baseAreaEnemyFBX[closestEnemy]->GetPosition().x - playerFBX->GetPosition().x;
		//float y2 = baseAreaEnemyFBX[closestEnemy]->GetPosition().z - playerFBX->GetPosition().z;
		float radians = atan2(z, x);
		float degrees = XMConvertToDegrees(radians);
		playerFBX->SetRotation({ playerFBX->GetRotation().x, -degrees + 90.0f, playerFBX->GetRotation().z });
		camera->resetPhi = degrees += 180.0f;
		objectRotation = playerFBX->GetRotation();
		lockOnActive = true;
		camera->playerPos = playerFBX->GetPosition();
		//camera->SetTarget(baseAreaEnemyFBX[closestEnemy]->GetPosition());
		camera->lockOnEnemyPos = baseAreaEnemyFBX[closestEnemy]->GetPosition();
		//camera->lockOn = true;
		camera->Update();
	}
	else
	{
		camera->lockOn = false;
		if (BaseAreaConditionals::ResetCameraAfterLockon(input->UpKey(DIK_SPACE), input->UpControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER)))
		{
			// Assigns the camera's current degree of rotation
			camera->resetPhi *= 3.141592654f / 180.0f;
			camera->SetPhi(camera->resetPhi);

			playerFBX->cameraResetActive = false;
			camera->resetting = true;
		}
		lockOnActive = false;
		camera->SetTarget({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + 15.0f, playerFBX->GetPosition().z });
		camera->SetDistance(48.0f);
		camera->Update();
	}
#pragma endregion

#pragma region updates
	playerFBX->Update();
	playerPositionOBJ->Update();
	attackRangeOBJ[0]->Update();
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaEnemyFBX[i]->Update();
		if (BaseAreaConditionals::IsEnemyLandingAttack(baseAreaEnemyFBX[i]->enumStatus))
		{
			landingPositionOBJ[i]->Update();
		}
		// Disable gravity during sky attack
		if (BaseAreaConditionals::ShouldGravityBeDisabled(baseAreaEnemyFBX[i]->landed))
		{
			baseAreaEnemyPositionOBJ[i]->notLandingAttack = true;
		}
		else
		{
			baseAreaEnemyPositionOBJ[i]->notLandingAttack = false;
		}
		baseAreaEnemyPositionOBJ[i]->Update();
		if (BaseAreaConditionals::IsEnemyAliveBOOLIAN(baseAreaEnemyAliveBOOL[i]))
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
		if (!BaseAreaConditionals::IsEnemyChargingNOSTAGE(baseAreaEnemyFBX[i]->enumStatus) && !BaseAreaConditionals::IsEnemyJetStreamAttackingNOSTAGE(baseAreaEnemyFBX[i]->enumStatus) && !BaseAreaConditionals::IsEnemyLandingAttack(baseAreaEnemyFBX[i]->enumStatus) && !BaseAreaConditionals::IsEnemyFleeing(baseAreaEnemyFBX[i]->enumStatus) && !BaseAreaConditionals::IsEnemySurroundAttackingNOSTAGE(baseAreaEnemyFBX[i]->enumStatus))
		{
			if (BaseAreaConditionals::AreFBXModelsColliding(DontStackOnTop(baseAreaEnemyFBX[i]->GetPosition(), playerFBX->GetPosition(), 3.0f)))
			{
				float dx = playerFBX->GetPosition().x - baseAreaEnemyFBX[i]->GetPosition().x;
				float dy = playerFBX->GetPosition().y - baseAreaEnemyFBX[i]->GetPosition().y;
				float dz = playerFBX->GetPosition().z - baseAreaEnemyFBX[i]->GetPosition().z;

				float length = sqrtf(powf(dx, 2) + powf(dy, 2) + powf(dz, 2));

				if (length > 0.0f)
				{
					dx /= length;
					dy /= length;
					dz /= length;
				}

				float newX = playerFBX->GetPosition().x;
				float newY = playerFBX->GetPosition().y;
				float newZ = playerFBX->GetPosition().z;

				float elapsedTime = 0.0f;

				while (BaseAreaConditionals::AreFBXModelsColliding(DontStackOnTop(playerFBX->GetPosition(), baseAreaEnemyFBX[i]->GetPosition(), 3.0f)))
				{
					playerFBX->MoveTowards(newX, playerFBX->GetPosition().x + dx, 1.0f, elapsedTime);
					playerFBX->MoveTowards(newY, playerFBX->GetPosition().y + dy, 1.0f, elapsedTime);
					playerFBX->MoveTowards(newZ, playerFBX->GetPosition().z + dz, 1.0f, elapsedTime);
					playerFBX->SetPosition({ newX, newY, newZ });
					elapsedTime += 0.1f;
				}
			}

			for (int j = 0; j < numberOfEnemiesTotal; j++)
			{
				if (i == j)
				{
					continue;
				}

				if (BaseAreaConditionals::AreFBXModelsColliding(DontStackOnTop(baseAreaEnemyFBX[j]->GetPosition(), baseAreaEnemyFBX[i]->GetPosition(), 3.0f)) && BaseAreaConditionals::IsEnemyAlive(baseAreaEnemyFBX[j]->enumStatus) && BaseAreaConditionals::IsEnemyAlive(baseAreaEnemyFBX[i]->enumStatus))
				{
					float dx = baseAreaEnemyFBX[j]->GetPosition().x - baseAreaEnemyFBX[i]->GetPosition().x;
					float dy = baseAreaEnemyFBX[j]->GetPosition().y - baseAreaEnemyFBX[i]->GetPosition().y;
					float dz = baseAreaEnemyFBX[j]->GetPosition().z - baseAreaEnemyFBX[i]->GetPosition().z;

					float length = sqrtf(powf(dx, 2.0f) + powf(dy, 2.0f) + powf(dz, 2.0f));

					if (length > 0.0f)
					{
						dx /= length;
						dy /= length;
						dz /= length;
					}

					float newX = baseAreaEnemyFBX[j]->GetPosition().x;
					float newY = baseAreaEnemyFBX[j]->GetPosition().y;
					float newZ = baseAreaEnemyFBX[j]->GetPosition().z;

					float elapsedTime = 0.0f;

					while (BaseAreaConditionals::AreFBXModelsColliding(DontStackOnTop(baseAreaEnemyFBX[j]->GetPosition(), baseAreaEnemyFBX[i]->GetPosition(), 3.0f)))
					{
						baseAreaEnemyFBX[j]->MoveTowards(newX, baseAreaEnemyFBX[j]->GetPosition().x + dx, 1.0f, elapsedTime);
						baseAreaEnemyFBX[j]->MoveTowards(newY, baseAreaEnemyFBX[j]->GetPosition().y + dy, 1.0f, elapsedTime);
						baseAreaEnemyFBX[j]->MoveTowards(newZ, baseAreaEnemyFBX[j]->GetPosition().z + dz, 1.0f, elapsedTime);
						baseAreaEnemyFBX[j]->SetPosition({ newX, newY, newZ });
						elapsedTime += 0.1f;
					}
				}
			}
		}
	}

	playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });
	playerFBX->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });

	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (BaseAreaConditionals::IsEnemyJetStreamAttackingNOSTAGE(baseAreaEnemyFBX[i]->enumStatus) && baseAreaEnemyFBX[i]->jetStreamAttackStage == 0)
		{
			baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
			baseAreaEnemyFBX[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
		}
		else if (!BaseAreaConditionals::IsEnemyLandingAttack(baseAreaEnemyFBX[i]->enumStatus))
		{
			baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyPositionOBJ[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
			baseAreaEnemyFBX[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyPositionOBJ[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
		}
		else if (BaseAreaConditionals::IsEnemyLandingAttack(baseAreaEnemyFBX[i]->enumStatus))
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
	/*char msgbuf[256];
	char msgbuf2[256];
	char msgbuf3[256];
	sprintf_s(msgbuf, 256, "X: %f\n", playerFBX->GetPosition().x);
	sprintf_s(msgbuf2, 256, "Y: %f\n", playerFBX->GetPosition().y);
	sprintf_s(msgbuf3, 256, "Z: %f\n", playerFBX->GetPosition().z);
	OutputDebugStringA(msgbuf);
	OutputDebugStringA(msgbuf2);
	OutputDebugStringA(msgbuf3);*/
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
	particleManExplosion->Draw(cmdList);

	Object3d::PostDraw();
#pragma endregion

#pragma region foregroundSpriteDrawing
	Sprite::PreDraw(cmdList);

	// Foreground sprite drawing
	if (BaseAreaConditionals::ShouldTheDamageOverlayDisplay(damageOverlayDisplay))
	{
		baseAreaDamageOverlaySPRITE->Draw();
	}
	HPBarSPRITE->Draw();
	HPBarFrameSPRITE->Draw();
	STBarSPRITE->Draw();
	STBarFrameSPRITE->Draw();
	slowMotionBarSPRITE->Draw();
	slowMotionBarFrameSPRITE->Draw();
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
		if (Distance(playerFBX->GetPosition(), baseAreaEnemyFBX[i]->GetPosition()) < 300.0f)
		{
			if (isInFront[i] = true)
			{
				baseAreaEnemyHPBarFrameSPRITE[i]->Draw();
				baseAreaEnemyHPBarSPRITE[i]->Draw();
			}
		}
	}*/
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		if (!BaseAreaConditionals::IsEnemyAliveBOOLIAN(baseAreaEnemyFBX[i]->dead))
		{
			baseAreaMinimapEnemySPRITE[i]->Draw();
		}
	}
	staminaWarningSPRITE->Draw();
	baseAreaMissionSPRITE->Draw();

	if (BaseAreaConditionals::IsPlayerInSlowMotion(playerFBX->slowMotion))
	{
		slowMotionSPRITE->Draw();
	}

	// Debug text drawing
	debugText->DrawAll(cmdList);

	if (BaseAreaConditionals::IsFadeSpriteVisible(fadeSpriteALPHA))
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
		// X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
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

		// 追加
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

		// 追加
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

		// 追加
		particleMan->Add(life, pos, vel, acc, start_scale, 0.0f);
	}
}

void BaseArea::ParticleCreationExplosion(float x, float y, float z, int life, float offset, float start_scale)
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

		// 追加
		particleManExplosion->Add(life, pos, vel, acc, start_scale, 0.0f);
	}
}

float BaseArea::Distance(XMFLOAT3 player, XMFLOAT3 center)
{
	float d = sqrtf(pow(center.x - player.x, 2.0f) + pow(center.y - player.y, 2.0f) + pow(center.z - player.z, 2.0f));
	return d;
}

bool BaseArea::DontStackOnTop(XMFLOAT3 enemyPos, XMFLOAT3 playerPos, float collisionRadius)
{
	float dx = playerPos.x - enemyPos.x;
	float dy = playerPos.y - enemyPos.y;
	float dz = playerPos.z - enemyPos.z;

	float distance = sqrtf(powf(dx, 2) + powf(dy, 2) + powf(dz, 2));

	return distance <= powf(collisionRadius, 2.0f);

	return false;
}

void BaseArea::MoveTowards(float& current, float target, float speed, float elapsedTime)
{
	float delta = target - current;
	float step = speed * elapsedTime;

	if (step > abs(delta))
	{
		current = target;
	}
	else
	{
		current += step * (delta < 0 ? -1 : 1);
	}
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

	loadingPercent++;

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
	if (!Sprite::LoadTexture(17, "LowStaminaWarning.png")) { assert(0); return; } // Low Stamina Warning Graphic
	if (!Sprite::LoadTexture(18, "SlowMotion.png")) { assert(0); return; } // Slow Motion Graphic
	if (!Sprite::LoadTexture(19, "LoadingBar.png")) { assert(0); return; } // Slow Motion Graphic
	if (!Sprite::LoadTexture(20, "LoadingBarFrame.png")) { assert(0); return; } // Slow Motion Graphic

	loadingPercent++;

	// Sprite generation
	HPBarSPRITE = Sprite::Create(1, { 25.0f, 25.0f });
	HPBarFrameSPRITE = Sprite::Create(2, { 25.0f, 25.0f });
	STBarSPRITE = Sprite::Create(3, { 25.0f, 55.0f });
	STBarFrameSPRITE = Sprite::Create(4, { 25.0f, 55.0f });
	slowMotionBarSPRITE = Sprite::Create(19, { 25.0f, 25.0f });
	slowMotionBarFrameSPRITE = Sprite::Create(4, { 25.0f, 25.0f });
	baseAreaMissionSPRITE = Sprite::Create(5, { 1150.0f, 100.0f });
	baseAreaMinimapSPRITE = Sprite::Create(6, { 50.0f , 430.0f });
	baseAreaMinimapPlayerSPRITE = Sprite::Create(7, { 0.0f, 0.0f });
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaMinimapEnemySPRITE[i] = Sprite::Create(8, { 0.0f, 0.0f });
		baseAreaEnemyHPBarSPRITE[i] = Sprite::Create(11, { 0.0f, 0.0f });
		baseAreaEnemyHPBarFrameSPRITE[i] = Sprite::Create(12, { 0.0f, 0.0f });
		isInFront[i] = false;
	}
	fadeSPRITE = Sprite::Create(10, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
	baseAreaDamageOverlaySPRITE = Sprite::Create(13, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
	healSPRITE = Sprite::Create(14, { 1102.0f, 542.0f });
	healKeyboardSPRITE = Sprite::Create(15, { 1102.0f, 542.0f });
	healControllerSPRITE = Sprite::Create(16, { 1102.0f, 542.0f });
	staminaWarningSPRITE = Sprite::Create(17, { 720.0f, 390.0f }, { 1.0f, 1.0f, 1.0f, 0.0f });
	slowMotionSPRITE = Sprite::Create(18, { 0.0f, 0.0f });
	// Resizing mission sprite
	baseAreaMissionSPRITE->SetSize({ 100.0f, 80.0f });
	staminaWarningSPRITE->SetSize({ 75.0f, 42.0f });

	loadingPercent++;
}

void BaseArea::thread2()
{
	// 3D Object creation
	skydomeOBJ = Object3d::Create();
	for (int i = 0; i < numberOfEnemiesTotal + 1; i++) { attackRangeOBJ[i] = Object3d::Create(); }
	for (int i = 0; i < numberOfEnemiesTotal; i++) { landingPositionOBJ[i] = Object3d::Create(); }
	for (int i = 0; i < numberOfEnemiesTotal; i++) { enemyVisionRangeOBJ[i] = Object3d::Create(); }
	playerPositionOBJ = PlayerPositionObject::Create();
	for (int i = 0; i < numberOfEnemiesTotal; i++) { baseAreaEnemyPositionOBJ[i] = PlayerPositionObject::Create(); }

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

	loadingPercent++;

	// Camera initial values
	camera->SetTarget({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + cameraYOffset, playerFBX->GetPosition().z });
	camera->SetUp(standardCameraUp);
	camera->SetDistance(standardCameraDistance);

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
		attackRangeOBJ[i]->SetScale(attackRangeOBJScale);
	}

	loadingPercent++;

	// Vision range initial values
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		enemyVisionRangeOBJ[i]->SetScale(enemyVisionRangeOBJScale);
	}

	// Skydome scale
	skydomeOBJ->SetScale(skydomeScale);

	// Position Object initial positions
	playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, initialPlayerYPosition, playerFBX->GetPosition().z });
	for (int i = 0; i < numberOfEnemiesTotal; i++)
	{
		baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, initialEnemyYPosition, baseAreaEnemyFBX[i]->GetPosition().z });
	}

	loadingPercent++;
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

	loadingPercent++;

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

	loadingPercent++;

	// Ground scale
	groundOBJ->SetScale(groundOBJScale);
	tutorialGroundOBJ->SetScale(tutorialGroundOBJScale);
	extendedGroundOBJ->SetScale(extendedGroundOBJScale);

	// Ground positions
	groundOBJ->SetPosition(groundOBJPosition);
	tutorialGroundOBJ->SetPosition(tutorialGroundOBJPosition);
	extendedGroundOBJ->SetPosition(extendedGroundOBJPosition);

	srand((unsigned int)time(NULL));

	loadingPercent++;
}

void BaseArea::thread4()
{

}