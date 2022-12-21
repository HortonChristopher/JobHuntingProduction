#include "TutorialArea.h"

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;

extern int keyOrMouse;

TutorialArea::TutorialArea()
{
}

TutorialArea::~TutorialArea()
{
	safe_delete(camera);
	collisionManager = nullptr;
	particleMan = nullptr;
	debugText = nullptr;
	safe_delete(groundOBJ);
	safe_delete(groundMODEL);
	safe_delete(skydomeOBJ);
	safe_delete(skydomeMODEL);
	safe_delete(tutorialTextFrameSPRITE);
	for (int i = 0; i < 16; i++) { safe_delete(tutorialTextSPRITE[i]); }
	for (int i = 0; i < 2; i++) { safe_delete(tutorialMissionSPRITE[i]); }
	safe_delete(playerFBX);
	safe_delete(playerPositionOBJ);
	safe_delete(enemyFBX);
	safe_delete(enemyPositionOBJ);
	safe_delete(playerAttackRangeOBJ);
	safe_delete(enemyAttackRangeOBJ);
	safe_delete(positionMODEL);
	safe_delete(missionBarSPRITE);
	safe_delete(missionBarFrameSPRITE);
	safe_delete(HPBarSPRITE);
	safe_delete(HPBarFrameSPRITE);
	safe_delete(STBarSPRITE);
	safe_delete(STBarFrameSPRITE);
	safe_delete(staminaTutorialMaskSPRITE);
	safe_delete(minimapTutorialMaskSPRITE);
	safe_delete(tutorialMinimapSPRITE);
	safe_delete(tutorialMinimapPlayerSPRITE);
	safe_delete(tutorialMinimapEnemySPRITE);
}

void TutorialArea::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// Checking for nullptr
	assert(dxCommon);
	assert(input);
	assert(audio);

	// Assigning variables to this
	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

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
	TutorialEnemy::SetDevice(dxCommon->GetDevice());
	TutorialPlayer::SetDevice(dxCommon->GetDevice());

	// Setting camera
	Object3d::SetCamera(camera);
	FBXGeneration::SetCamera(camera);
	TutorialEnemy::SetCamera(camera);
	TutorialPlayer::SetCamera(camera);
	PlayerPositionObject::SetCamera(camera);

	// Sprite Generation
	if (!Sprite::LoadTexture(1, L"Resources/TutorialTextFrame.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(2, L"Resources/Tutorial1_1.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(3, L"Resources/Tutorial1_2.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(4, L"Resources/Tutorial1_3.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(5, L"Resources/Tutorial1_3_k.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(6, L"Resources/Tutorial1_3_c.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(7, L"Resources/Tutorial1_4.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(8, L"Resources/Tutorial2_1.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(9, L"Resources/Tutorial2_2.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(10, L"Resources/Tutorial2_3.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(11, L"Resources/Tutorial2_3_k.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(12, L"Resources/Tutorial2_3_c.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(13, L"Resources/Tutorial2_4.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(14, L"Resources/Tutorial3_1.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(15, L"Resources/Tutorial3_2.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(16, L"Resources/Tutorial3_2_k.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(17, L"Resources/Tutorial3_2_c.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(18, L"Resources/Tutorial3_3.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(25, L"Resources/Tutorial3_4.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(19, L"Resources/Tutorial4_1.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(20, L"Resources/Tutorial4_2.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(21, L"Resources/Tutorial4_3.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(22, L"Resources/Tutorial4_3_k.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(23, L"Resources/Tutorial4_3_c.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(24, L"Resources/Tutorial4_4.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(26, L"Resources/Tutorial4_5.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(27, L"Resources/Tutorial4_6.png")) { assert(0); return; }

	if (!Sprite::LoadTexture(28, L"Resources/DamageOverlay.png")) { assert(0); return; } // Damage Overlay

	if (!Sprite::LoadTexture(99, L"Resources/PlayerMinimapSprite.png")) { assert(0); return; } // Player minimap texture
	if (!Sprite::LoadTexture(98, L"Resources/EnemyMinimapSprite.png")) { assert(0); return; } // Enemy minimap texture
	if (!Sprite::LoadTexture(97, L"Resources/MinimapTutorialMask.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(96, L"Resources/TutorialMinimap.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(95, L"Resources/StaminaTutorialMask.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(94, L"Resources/TutorialMission1.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(93, L"Resources/TutorialMission2.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(92, L"Resources/TutorialMission3.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(91, L"Resources/TutorialMission4.png")) { assert(0); return; }

	if (!Sprite::LoadTexture(85, L"Resources/LoadingBar.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(86, L"Resources/LoadingBarFrame.png")) { assert(0); return; }

	if (!Sprite::LoadTexture(87, L"Resources/HPBar.png")) { assert(0); return; } // HP bar texture
	if (!Sprite::LoadTexture(88, L"Resources/HPBarFrame.png")) { assert(0); return; } // HP bar frame texture
	if (!Sprite::LoadTexture(89, L"Resources/STBar.png")) { assert(0); return; } // ST bar texture
	if (!Sprite::LoadTexture(90, L"Resources/STBarFrame.png")) { assert(0); return; } // ST bar frame texture

	tutorialTextFrameSPRITE = Sprite::Create(1, { 390.0f, 300.0f });
	for (int i = 0; i < 26; i++)
	{
		tutorialTextSPRITE[i] = Sprite::Create((i + 2), { 390.0f, 300.0f });
	}
	for (int i = 94; i > 90; i--)
	{
		tutorialMissionSPRITE[94 - i] = Sprite::Create((94 - (94 - i)), { 1150.0f, 100.0f });
		tutorialMissionSPRITE[94 - i]->SetSize({ 100.0f, 80.0f });
	}
	missionBarSPRITE = Sprite::Create(85, { 1150.0f, 150.0f });
	missionBarFrameSPRITE = Sprite::Create(86, { 1150.0f, 150.0f });
	missionBarSPRITE->SetSize({ 100.0f, 30.0f });
	missionBarFrameSPRITE->SetSize({ 100.0f, 30.0f });
	HPBarSPRITE = Sprite::Create(87, { 25.0f, 25.0f });
	HPBarFrameSPRITE = Sprite::Create(88, { 25.0f, 25.0f });
	STBarSPRITE = Sprite::Create(89, { 25.0f, 55.0f });
	STBarFrameSPRITE = Sprite::Create(90, { 25.0f, 55.0f });
	staminaTutorialMaskSPRITE = Sprite::Create(95, { 0.0f, 0.0f });
	tutorialMinimapSPRITE = Sprite::Create(96, { 50.0f , 430.0f });
	minimapTutorialMaskSPRITE = Sprite::Create(97, { 0.0f, 0.0f });
	tutorialMinimapEnemySPRITE = Sprite::Create(98, { 0.0f, 0.0f });
	tutorialMinimapPlayerSPRITE = Sprite::Create(99, { 0.0f, 0.0f });

	tutorialAreaDamageOverlaySPRITE = Sprite::Create(28, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });

	// 3D Object Create
	skydomeOBJ = Object3d::Create();
	playerAttackRangeOBJ = Object3d::Create();
	enemyAttackRangeOBJ = Object3d::Create();

	// Model Creation
	groundMODEL = Model::CreateFromOBJ("TutorialStage");
	skydomeMODEL = Model::CreateFromOBJ("skydome");
	positionMODEL = Model::CreateFromOBJ("yuka");
	outsideGroundMODEL = Model::CreateFromOBJ("Landscape2");
	
	// Touchable Objection Creation
	groundOBJ = TouchableObject::Create(groundMODEL);
	outsideGroundOBJ = TouchableObject::Create(outsideGroundMODEL);
	skydomeOBJ->SetModel(skydomeMODEL);
	skydomeOBJ->SetScale({ 5,5,5 });
	playerAttackRangeOBJ->SetModel(positionMODEL);
	enemyAttackRangeOBJ->SetModel(positionMODEL);

	// Player initialization
	playerFBX = new TutorialPlayer;
	playerFBX->Initialize();
	playerPositionOBJ = PlayerPositionObject::Create();
	playerPositionOBJ->SetModel(positionMODEL);
	playerFBX->SetPosition({ 0.0f, 0.0f, -700.0f });

	// Enemy initialization
	enemyFBX = new TutorialEnemy;
	enemyFBX->Initialize();
	enemyPositionOBJ = PlayerPositionObject::Create();
	playerPositionOBJ->SetModel(positionMODEL);
	enemyFBX->SetPosition({ 0.0f, 0.0f, 0.0f });

	// Ground Aspects
	groundOBJ->SetPosition({ 0, 0, 0 });
	groundOBJ->SetScale({ 50, 10, 50 });
	outsideGroundOBJ->SetPosition({ 0.0f, -33.0f, 1150.0f });
	outsideGroundOBJ->SetScale({ 400, 200, 400 });
	playerPositionOBJ->SetScale({ 10, 10, 10 });
	enemyPositionOBJ->SetScale({ 10, 10, 10 });

	// Camera initial values
	camera->SetTarget(playerFBX->GetPosition());
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(48.0f);

	srand((unsigned int)time(NULL));
}

void TutorialArea::Update()
{
	lightGroup->Update();
	particleMan->Update();
	camera->SetTarget(playerFBX->GetPosition());
	camera->Update();

	skydomeOBJ->SetPosition(playerFBX->GetPosition());

	switch (tutorialStatus)
	{
	case INTROCUTSCENE:
		playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, playerFBX->GetPosition().z + 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) });
		if (playerFBX->GetPosition().z >= -450.0f)
		{
			tutorialStatus = MOVEMENTTUTORIAL;
			playerFBX->SetEnumStatus(TutorialPlayer::STAND);
			break;
		}
		break;
	case MOVEMENTTUTORIAL:
		if (input->TriggerKey(DIK_SPACE) && tutorialActive == true || input->TriggerControllerButton(XINPUT_GAMEPAD_A) && tutorialActive == true)
		{
			if (tutorialPage < 2)
			{
				tutorialPage++;
			}
			else if (tutorialPage == 2)
			{
				tutorialActive = false;
				playerFBX->tutorialPart = 1;
			}
			else if (tutorialPage == 3)
			{
				progress = 0.0f;
				tutorialPage = 0;
				tutorialStatus = STAMINATUTORIAL;
			}
		}

		if (!tutorialActive)
		{
			if (input->PushKey(DIK_A) || input->PushKey(DIK_D) || input->PushKey(DIK_S) || input->PushKey(DIK_W) ||
				input->PushLStickLeft() || input->PushLStickRight() || input->PushLStickDown() || input->PushLStickUp())
			{
				progress += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			}

			missionBarSPRITE->SetSize({ progress + 0.1f, 30.0f });
			
			if (progress >= 100.0f)
			{
				tutorialPage = 3;
				playerFBX->tutorialPart = 0;
				playerFBX->SetEnumStatus(TutorialPlayer::STAND);
				tutorialActive = true;
			}
		}
		break;
	case STAMINATUTORIAL:
		if (input->TriggerKey(DIK_SPACE) && tutorialActive == true || input->TriggerControllerButton(XINPUT_GAMEPAD_A) && tutorialActive == true)
		{
			if (tutorialPage < 2)
			{
				tutorialPage++;
			}
			else if (tutorialPage == 2)
			{
				tutorialActive = false;
				playerFBX->tutorialPart = 2;
			}
			else if (tutorialPage == 3)
			{
				progress = 0.0f;
				tutorialPage = 0;
				tutorialStatus = ATTACKTUTORIAL;
			}
		}

		if (!tutorialActive)
		{
			progress = (100.0f - playerFBX->stamina);
			missionBarSPRITE->SetSize({ progress + 0.1f, 30.0f });

			if (progress >= 100.0f)
			{
				tutorialPage = 3;
				playerFBX->tutorialPart = 0;
				playerFBX->SetEnumStatus(TutorialPlayer::STAND);
				tutorialActive = true;
			}
		}
		break;
	case ATTACKTUTORIAL:
		if (input->TriggerKey(DIK_SPACE) && tutorialActive == true || input->TriggerControllerButton(XINPUT_GAMEPAD_A) && tutorialActive == true)
		{
			if (tutorialPage < 1)
			{
				tutorialPage++;
			}
			else if (tutorialPage == 1)
			{
				tutorialActive = false;
				playerFBX->tutorialPart = 3;
			}
			else if (tutorialPage == 2)
			{
				progress = 0.0f;
				tutorialPage = 0;
				tutorialStatus = DODGETUTORIAL;
			}
		}

		if (!tutorialActive)
		{
			if (playerFBX->enumStatus == TutorialPlayer::ATTACK)
			{
				if (playerFBX->timer >= 54.2f && playerFBX->timer <= 62.2f ||
					playerFBX->timer >= 90.35f && playerFBX->timer <= 98.3f ||
					playerFBX->timer >= 152.6f && playerFBX->timer <= 160.64f)
				{
					if (playerFBX->enumStatus == TutorialPlayer::ATTACK && playerFBX->ableToDamage)
					{
						if (intersect(playerAttackRangeOBJ->GetPosition(), enemyFBX->GetPosition(), 3.0f, 25.0f, 25.0f))
						{
							if (playerFBX->timer >= 152.6f && playerFBX->timer <= 160.64f)
							{
								enemyKnockbackTime = 0.0f;
								enemyKnockback = true;
							}

							enemyFBX->timer = 0.0f;
							enemyFBX->modelChange = true;
							enemyFBX->SetEnumStatus(TutorialEnemy::DAMAGED);
							progress += 15.0f;

							if (playerFBX->timer >= 152.6f && playerFBX->timer <= 160.64f)
							{
								ParticleCreation(enemyFBX->GetPosition().x, enemyFBX->GetPosition().y, enemyFBX->GetPosition().z, 90, 5.0f, 20.0f);
							}
							else
							{
								ParticleCreation(enemyFBX->GetPosition().x, enemyFBX->GetPosition().y, enemyFBX->GetPosition().z, 60, 5.0f, 10.0f);
							}
							playerFBX->ableToDamage = false;
						}
					}
				}

				if (enemyKnockback)
				{
					XMFLOAT3 xyz = playerFBX->GetPosition() - enemyFBX->GetPosition();
					XMFLOAT3 knockbackPrevPosition = enemyFBX->GetPosition();
					float hypotenuse = sqrt((xyz.x * xyz.x) + (xyz.z * xyz.z));
					enemyFBX->SetPosition({
							knockbackPrevPosition.x -= 120.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
							knockbackPrevPosition.y,
							knockbackPrevPosition.z -= 120.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
					enemyKnockbackTime += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
					if (enemyKnockbackTime >= 30.0f)
					{
						enemyKnockbackTime = 0.0f;
						enemyKnockback = false;
					}
				}

				if (playerFBX->timer > 62.2f && playerFBX->timer < 90.35f ||
					playerFBX->timer > 98.3f && playerFBX->timer < 152.6f)
				{
					playerFBX->ableToDamage = true;
				}
			}
			else
			{
				playerFBX->ableToDamage = true;
				playerFBX->attackCombo = 0;
			}

			missionBarSPRITE->SetSize({ progress + 0.1f, 30.0f });

			if (progress >= 100.0f)
			{
				progress = 100.0f;
				tutorialPage = 2;
				playerFBX->tutorialPart = 0;
				playerFBX->SetEnumStatus(TutorialPlayer::STAND);
				tutorialActive = true;
			}
		}
		break;
	case DODGETUTORIAL:
		if (input->TriggerKey(DIK_SPACE) && tutorialActive == true || input->TriggerControllerButton(XINPUT_GAMEPAD_A) && tutorialActive == true)
		{
			if (tutorialPage < 2 || tutorialPage > 2 && tutorialPage < 5)
			{
				tutorialPage++;
			}
			else if (tutorialPage == 2)
			{
				tutorialActive = false;
				enemyFBX->SetEnumStatus(TutorialEnemy::AGGRO);
				playerFBX->tutorialPart = 4;
			}
			else if (tutorialPage == 5)
			{
				progress = 0.0f;
				tutorialPage = 0;
				tutorialStatus = TUTORIALEND;
			}
		}

		if (!tutorialActive)
		{
			objectPosition = playerFBX->GetPosition();

			float distance = sqrt((enemyFBX->GetPosition().x - playerFBX->GetPosition().x) * (enemyFBX->GetPosition().x - playerFBX->GetPosition().x) + (enemyFBX->GetPosition().z - playerFBX->GetPosition().z) * (enemyFBX->GetPosition().z - playerFBX->GetPosition().z));
			if (distance < 8.0f)
			{
				//baseAreaEnemyFBX[i]->SetAttack(true);
				if (enemyFBX->enumStatus != TutorialEnemy::DAMAGED && enemyFBX->enumStatus != TutorialEnemy::PARTICLEATTACK && enemyFBX->enumStatus != TutorialEnemy::ATTACK && enemyFBX->enumStatus != TutorialEnemy::COOLDOWN)
				{
					if (progress < 80.0f)
					{
						enemyFBX->SetEnumStatus(TutorialEnemy::ATTACK);
					}
					else
					{
						enemyFBX->particleAttackStage = 0;
						enemyFBX->modelChange = true;
						enemyFBX->SetEnumStatus(TutorialEnemy::PARTICLEATTACK);
					}
				}
			}
			else
			{
				if (enemyFBX->enumStatus != TutorialEnemy::DAMAGED && enemyFBX->enumStatus != TutorialEnemy::COOLDOWN && enemyFBX->enumStatus != TutorialEnemy::PARTICLEATTACK && enemyFBX->enumStatus != TutorialEnemy::ATTACK)
				{
					if (enemyFBX->enumStatus != TutorialEnemy::AGGRO)
					{
						enemyFBX->SetAggroSwitch(true);
						enemyFBX->SetEnumStatus(TutorialEnemy::AGGRO);
					}
				}
			}

			if (enemyFBX->enumStatus == TutorialEnemy::ATTACK || enemyFBX->enumStatus == TutorialEnemy::PARTICLEATTACK && enemyFBX->particleAttackStage == 1)
			{
				if (playerFBX->enumStatus == TutorialPlayer::DODGE && playerFBX->dodgedAttack == false)
				{
					progress += 19.9f;
					playerFBX->dodgedAttack = true;
				}
			}
			else
			{
				if (playerFBX->dodgedAttack == true)
				{
					progress += 0.1f;
				}
				playerFBX->dodgedAttack = false;
			}

			if (enemyFBX->particleAttackActive)
			{
				ParticleCreation(enemyFBX->particleAttackPosition.x, enemyFBX->particleAttackPosition.y, enemyFBX->particleAttackPosition.z, 60, 0.0f, 10.0f);
			}

			if (intersect(enemyAttackRangeOBJ->GetPosition(), playerFBX->GetPosition(), 3.0f, 15.0f, 15.0f) && enemyFBX->attackTimer > 70.0f && enemyFBX->attackTimer < 85.0f && enemyFBX->ableToDamage)
			{
				enemyFBX->ableToDamage = false;
				damageOverlaySpriteALPHA = 1.0f;
				damageOverlayDisplay = true;
				screenShake = true;
				tutorialAreaDamageOverlaySPRITE->SetColor({ 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
				if (playerFBX->hp >= 2.0f)
				{
					playerFBX->hp -= 1.0f;
				}
				else
				{
					playerFBX->hp = 1.0f;
				}
				playerFBX->SetEnumStatus(TutorialPlayer::DAMAGED);
			}

			if (intersect(enemyFBX->particleAttackPosition, playerFBX->GetPosition(), 3.0f, 12.0f, 12.0f) && enemyFBX->particleAttackActive && enemyFBX->ableToDamage)
			{
				enemyFBX->ableToDamage = false;
				damageOverlaySpriteALPHA = 1.0f;
				damageOverlayDisplay = true;
				screenShake = true;
				tutorialAreaDamageOverlaySPRITE->SetColor({ 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
				if (playerFBX->hp >= 4.0f)
				{
					playerFBX->hp -= 3.0f;
				}
				else
				{
					playerFBX->hp = 1.0f;
				}
				playerFBX->SetEnumStatus(TutorialPlayer::DAMAGED);
				knockback = true;
			}

			if (knockback && enemyFBX->enumStatus == TutorialEnemy::PARTICLEATTACK)
			{
				XMFLOAT3 xyz = enemyFBX->GetPosition() - playerFBX->GetPosition();
				XMFLOAT3 knockbackPrevPosition = playerFBX->GetPosition();
				float hypotenuse = sqrt((xyz.x * xyz.x) + (xyz.z * xyz.z));
				playerFBX->SetPosition({
						knockbackPrevPosition.x -= 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.x / hypotenuse),
						knockbackPrevPosition.y,
						knockbackPrevPosition.z -= 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (xyz.z / hypotenuse) });
				knockbackTime += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				if (knockbackTime >= 30.0f)
				{
					knockbackTime = 0.0f;
					knockback = false;
				}
			}

			// Damage overlay display
			if (damageOverlayDisplay)
			{
				if (playerFBX->enumStatus != TutorialPlayer::DEAD)
				{
					damageOverlaySpriteALPHA -= 0.4f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				}
				tutorialAreaDamageOverlaySPRITE->SetColor({ 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
				if (damageOverlaySpriteALPHA <= 0.0f)
				{
					damageOverlayDisplay = false;
				}
			}

			// Screen Shake Effect
			if (screenShake)
			{
				camera->SetTarget(ScreenShake(playerFBX->GetPosition()));
				camera->Update();
				shakeTimer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
				if (shakeTimer >= 15.0f)
				{
					shakeTimer = 0.0f;
					screenShake = false;
				}
			}
		}

		missionBarSPRITE->SetSize({ progress + 0.1f, 30.0f });

		if (progress >= 100.0f)
		{
			if (enemyFBX->enumStatus != TutorialEnemy::DEAD)
			{
				enemyFBX->modelChange = true;
				enemyFBX->SetEnumStatus(TutorialEnemy::DEAD);
			}

			if (tutorialPage == 2)
			{
				progress = 100.0f;
				tutorialPage = 3;
				playerFBX->tutorialPart = 0;
				playerFBX->SetEnumStatus(TutorialPlayer::STAND);
				tutorialActive = true;
			}
		}

		break;
	case TUTORIALEND:
		if (playerFBX->enumStatus == TutorialPlayer::HEAL)
		{
			if (playerFBX->timer >= 80.0f)
			{
				for (int i = 0; i < 180; i++)
				{
					ParticleCreationHeal(playerFBX->healParticlePosition[0].x + cosf(XMConvertToRadians(i * 2.0f)) * 10.0f,
						playerFBX->healParticlePosition[0].y,
						playerFBX->healParticlePosition[0].z + sinf(XMConvertToRadians(i * 2.0f)) * 10.0f, 10, 0.0f, 1.0f);
				}
			}
			if (playerFBX->timer >= 90.0f)
			{
				for (int i = 0; i < 180; i++)
				{
					ParticleCreationHeal(playerFBX->healParticlePosition[1].x + cosf(XMConvertToRadians(i * 2.0f)) * 10.0f,
						playerFBX->healParticlePosition[1].y,
						playerFBX->healParticlePosition[1].z + sinf(XMConvertToRadians(i * 2.0f)) * 10.0f, 10, 0.0f, 1.0f);
				}
			}
			if (playerFBX->timer >= 100.0f)
			{
				for (int i = 0; i < 180; i++)
				{
					ParticleCreationHeal(playerFBX->healParticlePosition[2].x + cosf(XMConvertToRadians(i * 2.0f)) * 10.0f,
						playerFBX->healParticlePosition[2].y,
						playerFBX->healParticlePosition[2].z + sinf(XMConvertToRadians(i * 2.0f)) * 10.0f, 10, 0.0f, 1.0f);
				}
			}
		}

		tutorialActive = false;
		playerFBX->tutorialPart = 5;
		if (playerFBX->GetPosition().z >= 500.0f)
		{
			deletion = true;
		}
		break;
	}

	if (distance(playerFBX->GetPosition(), groundOBJ->GetPosition()) >= 490.0f && tutorialStatus != INTROCUTSCENE && tutorialStatus != TUTORIALEND)
	{
		if (!arenaClamp)
		{
			fromOriginToObject = playerFBX->GetPosition() - groundOBJ->GetPosition();
			fromOriginToObject* (490.0f / distance(playerFBX->GetPosition(), groundOBJ->GetPosition()));
		}
		playerFBX->SetPosition(groundOBJ->GetPosition() + fromOriginToObject);
		arenaClamp = true;
	}
	else
	{
		arenaClamp = false;
	}

	if (tutorialStatus != INTROCUTSCENE)
	{
		for (float i = 0.0f; i < 360.0f; i++)
		{
			if (distance(playerFBX->GetPosition(), { cosf(XMConvertToRadians(i)) * 495.0f, 0.0f, sinf(XMConvertToRadians(i)) * 495.0f }) <= 20.0f)
			{
				ParticleCreationEdge(cosf(XMConvertToRadians(i)) * 495.0f, 0.0f, sinf(XMConvertToRadians(i)) * 495.0f, 30, 5.0f, 3.0f);
			}
		}
	}

	if (tutorialStatus > 2)
	{
		HPBarSPRITE->SetSize({ playerFBX->hp * 20.0f, 20.0f });
		STBarSPRITE->SetSize({ playerFBX->stamina * 2.0f, 20.0f });

		tutorialMinimapPlayerSPRITE->SetPosition({ playerFBX->GetPosition().x * 0.24f + 165.0f, playerFBX->GetPosition().z * 0.24f + 545.0f });
		tutorialMinimapEnemySPRITE->SetPosition({ enemyFBX->GetPosition().x * 0.24f + 165.0f, enemyFBX->GetPosition().z * 0.24f + 545.0f });

		if (input->PushKey(DIK_LSHIFT) && playerFBX->stamina > 0.0f || input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && playerFBX->stamina > 0.0f)
		{
			ParticleCreation(playerFBX->GetPosition().x, playerFBX->GetPosition().y, playerFBX->GetPosition().z, 10, -1.0f, 1.0f);
		}
	}

	playerAttackRangeOBJ->SetPosition({ (playerFBX->GetPosition().x + (sinf(XMConvertToRadians(playerFBX->GetRotation().y)) * 15)), playerFBX->GetPosition().y + 0.5f, (playerFBX->GetPosition().z + (cosf(XMConvertToRadians(playerFBX->GetRotation().y)) * 15))});
	playerAttackRangeOBJ->SetRotation(playerFBX->GetRotation());

	if (tutorialStatus > 3)
	{
		enemyAttackRangeOBJ->SetPosition({ (enemyFBX->GetPosition().x + (sinf(XMConvertToRadians(enemyFBX->GetRotation().y)) * 15)), enemyFBX->GetPosition().y + 0.5f, (enemyFBX->GetPosition().z + (cosf(XMConvertToRadians(enemyFBX->GetRotation().y)) * 15)) });
		enemyAttackRangeOBJ->SetRotation(enemyFBX->GetRotation());
	}

	playerFBX->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });
	playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });

	//Debug Start
	//char msgbuf[256];
	//char msgbuf2[256];
	//char msgbuf3[256];

	//sprintf_s(msgbuf, 256, "X: %f\n", distance(playerFBX->GetPosition(), groundOBJ->GetPosition()));
	//sprintf_s(msgbuf2, 256, "Z: %f\n", object1->GetPosition().z);
	//sprintf_s(msgbuf3, 256, "Z: %f\n", objectPosition.z);
	//OutputDebugStringA(msgbuf);
	//OutputDebugStringA(msgbuf2);
	//OutputDebugStringA(msgbuf3);
	//Debug End

#pragma region updates
	groundOBJ->Update();
	outsideGroundOBJ->Update();
	skydomeOBJ->Update();
	playerFBX->Update();
	playerAttackRangeOBJ->Update();
	if (tutorialStatus > 3)
	{
		enemyFBX->Update();
		enemyAttackRangeOBJ->Update();
		enemyPositionOBJ->Update();
	}
	playerPositionOBJ->Update();
	collisionManager->CheckAllCollisions();
#pragma endregion
}

void TutorialArea::Draw()
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
	//playerAttackRangeOBJ->Draw();
	if (tutorialStatus > 3)
	{
		enemyFBX->Draw(cmdList);
		//enemyAttackRangeOBJ->Draw();
	}
	groundOBJ->Draw();
	outsideGroundOBJ->Draw();
	skydomeOBJ->Draw();

	// Particle drawing
	particleMan->Draw(cmdList);

	Object3d::PostDraw();
#pragma endregion

#pragma region foregroundSpriteDrawing
	Sprite::PreDraw(cmdList);

	// Foreground sprite drawing
	if (tutorialActive)
	{
		switch (tutorialStatus)
		{
		case INTROCUTSCENE:
			break;
		case MOVEMENTTUTORIAL:
			tutorialTextFrameSPRITE->Draw();
			switch (tutorialPage)
			{
			case 0:
				tutorialTextSPRITE[0]->Draw();
				break;
			case 1:
				tutorialTextSPRITE[1]->Draw();
				break;
			case 2:
				tutorialTextSPRITE[2]->Draw();
				if (keyOrMouse == 0)
				{
					tutorialTextSPRITE[3]->Draw();
				}
				else if (keyOrMouse == 1)
				{
					tutorialTextSPRITE[4]->Draw();
				}
				break;
			case 3:
				tutorialTextSPRITE[5]->Draw();
				break;
			}
			break;
		case STAMINATUTORIAL:
			switch (tutorialPage)
			{
			case 0:
				staminaTutorialMaskSPRITE->Draw();
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[6]->Draw();
				break;
			case 1:
				staminaTutorialMaskSPRITE->Draw();
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[7]->Draw();
				break;
			case 2:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[8]->Draw();
				if (keyOrMouse == 0)
				{
					tutorialTextSPRITE[9]->Draw();
				}
				else if (keyOrMouse == 1)
				{
					tutorialTextSPRITE[10]->Draw();
				}
				break;
			case 3:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[11]->Draw();
				break;
			}
			break;
		case ATTACKTUTORIAL:
			switch (tutorialPage)
			{
			case 0:
				minimapTutorialMaskSPRITE->Draw();
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[12]->Draw();
				break;
			case 1:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[13]->Draw();
				if (keyOrMouse == 0)
				{
					tutorialTextSPRITE[14]->Draw();
				}
				else if (keyOrMouse == 1)
				{
					tutorialTextSPRITE[15]->Draw();
				}
				break;
			case 2:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[16]->Draw();
				break;
			case 3:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[23]->Draw();
				break;
			}
			break;
		case DODGETUTORIAL:
			switch (tutorialPage)
			{
			case 0:
				staminaTutorialMaskSPRITE->Draw();
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[17]->Draw();
				break;
			case 1:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[18]->Draw();
				break;
			case 2:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[19]->Draw();
				if (keyOrMouse == 0)
				{
					tutorialTextSPRITE[20]->Draw();
				}
				else if (keyOrMouse == 1)
				{
					tutorialTextSPRITE[21]->Draw();
				}
				break;
			case 3:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[22]->Draw();
				break;
			case 4:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[24]->Draw();
				break;
			case 5:
				tutorialTextFrameSPRITE->Draw();
				tutorialTextSPRITE[25]->Draw();
				break;
			}
			break;
		case TUTORIALEND:
			break;
		}
	}
	else if (!tutorialActive)
	{
		switch (tutorialStatus)
		{
		case INTROCUTSCENE:
			break;
		case MOVEMENTTUTORIAL:
			tutorialMissionSPRITE[0]->Draw();
			missionBarSPRITE->Draw();
			break;
		case STAMINATUTORIAL:
			tutorialMissionSPRITE[1]->Draw();
			missionBarSPRITE->Draw();
			break;
		case ATTACKTUTORIAL:
			tutorialMissionSPRITE[2]->Draw();
			missionBarSPRITE->Draw();
			break;
		case DODGETUTORIAL:
			tutorialMissionSPRITE[3]->Draw();
			missionBarSPRITE->Draw();
			break;
		case TUTORIALEND:
			break;
		}
	}

	if (tutorialStatus > 2)
	{
		STBarSPRITE->Draw();
		STBarFrameSPRITE->Draw();
	}

	if (tutorialStatus > 3)
	{
		tutorialMinimapSPRITE->Draw();
		tutorialMinimapEnemySPRITE->Draw();
		tutorialMinimapPlayerSPRITE->Draw();
		HPBarSPRITE->Draw();
		HPBarFrameSPRITE->Draw();
	}

	if (damageOverlayDisplay)
	{
		tutorialAreaDamageOverlaySPRITE->Draw();
	}

	// Sprite post draw
	Sprite::PostDraw();
#pragma endregion
}

int TutorialArea::intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH)
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

void TutorialArea::ParticleCreationHeal(float x, float y, float z, int life, float offset, float start_scale)
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

void TutorialArea::ParticleCreationEdge(float x, float y, float z, int life, float offset, float start_scale)
{
	for (int i = 0; i < 10; i++) {
		XMFLOAT3 pos{};
		pos.x = x;
		pos.y = y + offset;
		pos.z = z;

		const float rnd_vel = 0.1f; // 0.1f
		XMFLOAT3 vel{};
		vel.x = 0.0f;
		vel.y = 0.0f;
		vel.z = 0.0f;

		XMFLOAT3 acc{};
		const float rnd_acc = 0.001f; // 0.001f
		acc.y = 0.0f;

		// ’Ç‰Á
		particleMan->Add(life, pos, vel, acc, start_scale, 0.0f);
	}
}

float TutorialArea::distance(XMFLOAT3 player, XMFLOAT3 center)
{
	float d = sqrtf(pow(center.x - player.x, 2.0f) + pow(center.y - player.y, 2.0f) + pow(center.z - player.z, 2.0f));
	return d;
}

XMFLOAT3 TutorialArea::ScreenShake(XMFLOAT3 playerPosition)
{
	XMFLOAT3 shakePosition;

	shakePosition.x = playerPosition.x + (rand() % 3 - 1);
	shakePosition.y = playerPosition.y + (rand() % 3 - 1);
	shakePosition.z = playerPosition.z + (rand() % 3 - 1);

	return shakePosition;
}

void TutorialArea::ParticleCreation(float x, float y, float z, int life, float offset, float start_scale)
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