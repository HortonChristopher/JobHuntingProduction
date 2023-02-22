#include "GameOverCutscene.h"

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;

GameOverCutscene::GameOverCutscene()
{
}

GameOverCutscene::~GameOverCutscene()
{
	safe_delete(camera);
	collisionManager = nullptr;
	particleMan = nullptr;
	debugText = nullptr;
	safe_delete(skydomeMODEL);
	safe_delete(groundMODEL);
	safe_delete(positionMODEL);
	safe_delete(skydomeOBJ);
	safe_delete(groundOBJ);
	safe_delete(cutsceneEnemyFBX); 
	safe_delete(cutsceneEnemyPositionOBJ);
	safe_delete(cutsceneDamageOverlaySPRITE);
	safe_delete(fadeSPRITE);
	safe_delete(gameOverSPRITE);
}

void GameOverCutscene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// Checking for nullptr
	assert(dxCommon);
	assert(input);
	assert(audio);

	// Assigning variables to this
	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	audio->PlayWave("GameOver.wav", audio->gameOverVolume, true);

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

	// Loading debug text
	if (!Sprite::LoadTexture(debugTextTexNumber, "debugfont.png")) { assert(0); return; }

	// Debug text initialization
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	if (!Sprite::LoadTexture(10, "BlackScreen.png")) { assert(0); return; } // Black Screen
	if (!Sprite::LoadTexture(13, "DamageOverlay.png")) { assert(0); return; } // Damage Overlay
	if (!Sprite::LoadTexture(200, "p3.png")) { assert(0); return; }

	fadeSPRITE = Sprite::Create(10, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
	cutsceneDamageOverlaySPRITE = Sprite::Create(13, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, damageOverlaySpriteALPHA });
	gameOverSPRITE = Sprite::Create(200, { 0,0 });

	skydomeOBJ = Object3d::Create();
	skydomeMODEL = Model::CreateFromOBJ("skydome");
	skydomeOBJ->SetModel(skydomeMODEL);
	skydomeOBJ->SetPosition({ 0.0f, 0.0f, 0.0f });
	skydomeOBJ->SetScale({ 5,5,5 });
	
	groundMODEL = Model::CreateFromOBJ("Landscape2");
	groundOBJ = TouchableObject::Create(groundMODEL);
	groundOBJ->SetScale({ 2,0.5f,2 });
	groundOBJ->SetPosition({ 0, -15, 0 });

	cutsceneEnemyFBX = new EnemyHuman;
	cutsceneEnemyFBX->Initialize();
	cutsceneEnemyFBX->SetPosition({0.0f, 8.0f, 0.0f});
	cutsceneEnemyFBX->SetRotation({ 0.0f, 180.0f, 0.0f });
	cutsceneEnemyFBX->SetHomePosition({ 0.0f, 0.0f });

	positionMODEL = Model::CreateFromOBJ("yuka");
	cutsceneEnemyPositionOBJ = PlayerPositionObject::Create();
	cutsceneEnemyPositionOBJ->SetModel(positionMODEL);
	cutsceneEnemyPositionOBJ->SetPosition(cutsceneEnemyFBX->GetPosition());

	// Camera initial values
	camera->SetTarget(cutsceneEnemyFBX->GetPosition());
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(24.0f);
	camera->cutsceneActive = true;
}

void GameOverCutscene::Update()
{
	lightGroup->Update();
	particleMan->Update();

	cutsceneEnemyPositionOBJ->SetPosition(cutsceneEnemyFBX->GetPosition());

	camera->Update();

	switch (cutsceneStatus)
	{
	case PAUSE:
		cutsceneEnemyFBX->SetEnumStatus(EnemyHuman::STAND);

		degrees += 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

		if (degrees >= 20.0f)
		{
			degrees = 0.0f;
			cutsceneStatus = ENEMYATTACK;
			break;
		}
		break;
	case ENEMYATTACK:
		cutsceneEnemyFBX->SetEnumStatus(EnemyHuman::ATTACK);
		if (cutsceneEnemyFBX->attackTimer >= 58.3f)
		{
			screenShake = true;
			damageOverlayDisplay = true;
			cutsceneStatus = PLAYERDAMAGE;
			break;
		}
		break;
	case PLAYERDAMAGE:
		if (cutsceneEnemyFBX->enumStatus == EnemyHuman::ATTACK)
		{
			cutsceneEnemyFBX->SetEnumStatus(EnemyHuman::ATTACK);
		}
		else
		{
			cutsceneEnemyFBX->SetEnumStatus(EnemyHuman::STAND);
		}

		if (screenShake)
		{
			camera->SetTarget(ScreenShake(cutsceneEnemyFBX->GetPosition()));
			shakeTimer += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
			if (shakeTimer >= 15.0f)
			{
				shakeTimer = 0.0f;
				camera->SetTarget(cutsceneEnemyFBX->GetPosition());
				cameraTargetPosition = cutsceneEnemyFBX->GetPosition();
				screenShake = false;
			}
		}
		else if (!screenShake)
		{
			camera->cutscene = true;
			cutsceneStatus = CAMERAMOVE;
			break;
		}
		break;
	case CAMERAMOVE:
		if (degrees <= 30.0f)
		{
			cutsceneEnemyFBX->SetEnumStatus(EnemyHuman::STAND);
		}
		else
		{
			cutsceneEnemyFBX->SetEnumStatus(EnemyHuman::WANDER);
		}

		degrees += 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		camera->SetTarget({ 0.0f, sinf(XMConvertToRadians(degrees)) * 24.0f, -sinf(XMConvertToRadians(degrees)) * 24.0f });

		if (degrees >= 90.0f)
		{
			camera->cutscene = false;
			cutsceneStatus = GRAYSCREEN;
			break;
		}

		break;
	case GRAYSCREEN:
		fadeSpriteALPHA += 0.4f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		fadeSPRITE->SetColor({ 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
		if (fadeSpriteALPHA >= 1.0f)
		{
			fadeSpriteALPHA = 1.0f;
			cutsceneStatus = GAMEOVERSCREEN;
			break;
		}
		break;
	case GAMEOVERSCREEN:
		cutsceneEnemyFBX->SetEnumStatus(EnemyHuman::STAND);

		if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
		{
			audio->StopWave("GameOver.wav");
		}

		break;
	}

#pragma region updates
	cutsceneEnemyFBX->Update();
	cutsceneEnemyPositionOBJ->Update();
	skydomeOBJ->Update();
	groundOBJ->Update();
	collisionManager->CheckAllCollisions();
#pragma endregion
}

void GameOverCutscene::Draw()
{
	// Setting command list
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 3DDrawing
	Object3d::PreDraw(cmdList);

	// 3D Object Drawing
	cutsceneEnemyFBX->Draw(cmdList);

	skydomeOBJ->Draw();
	groundOBJ->Draw();

	// Particle drawing
	particleMan->Draw(cmdList);

	Object3d::PostDraw();
#pragma endregion

#pragma region foregroundSpriteDrawing
	Sprite::PreDraw(cmdList);

	// Foreground sprite drawing
	if (damageOverlayDisplay)
	{
		cutsceneDamageOverlaySPRITE->Draw();
	}

	// Debug text drawing
	debugText->DrawAll(cmdList);

	if (fadeSpriteALPHA > 0.0f)
	{
		fadeSPRITE->Draw();
	}

	if (cutsceneStatus == GAMEOVERSCREEN)
	{
		gameOverSPRITE->Draw();
	}

	// Sprite post draw
	Sprite::PostDraw();
#pragma endregion
}

XMFLOAT3 GameOverCutscene::ScreenShake(XMFLOAT3 playerPosition)
{
	XMFLOAT3 shakePosition;

	shakePosition.x = playerPosition.x + (rand() % 3 - 1);
	shakePosition.y = playerPosition.y + (rand() % 3 - 1);
	shakePosition.z = playerPosition.z + (rand() % 3 - 1);

	return shakePosition;
}
