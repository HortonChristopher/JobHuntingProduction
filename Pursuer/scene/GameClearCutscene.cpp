#include "GameClearCutscene.h"

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;

GameClearCutscene::GameClearCutscene()
{
}

GameClearCutscene::~GameClearCutscene()
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
	for (int i = 0; i < 8; i++) { safe_delete(cutsceneEnemyFBX[i]); safe_delete(cutsceneEnemyPositionOBJ[i]); }
	safe_delete(fadeSPRITE);
	safe_delete(gameClearSPRITE);
}

void GameClearCutscene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// Checking for nullptr
	assert(dxCommon);
	assert(input);
	assert(audio);

	// Assigning variables to this
	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	audio->PlayWave("GameClear.wav", audio->gameClearVolume, true);

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

	// Texture reading
	if (!Sprite::LoadTexture(10, "BlackScreen.png")) { assert(0); return; } // Black Screen
	if (!Sprite::LoadTexture(201, "p4.png")) { assert(0); return; }

	// Sprite Generation
	fadeSPRITE = Sprite::Create(10, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
	gameClearSPRITE = Sprite::Create(201, { 0,0 });

	skydomeOBJ = Object3d::Create();
	skydomeMODEL = Model::CreateFromOBJ("skydome");
	skydomeOBJ->SetModel(skydomeMODEL);
	skydomeOBJ->SetPosition({ 0.0f, 0.0f, 0.0f });
	skydomeOBJ->SetScale({ 5,5,5 });

	groundMODEL = Model::CreateFromOBJ("Landscape2");
	groundOBJ = TouchableObject::Create(groundMODEL);
	groundOBJ->SetScale({ 2,0.5f,2 });
	groundOBJ->SetPosition({ 0, -15, 0 });

	srand((unsigned int)time(NULL));

	for (int i = 0; i < 8; i++)
	{
		cutsceneEnemyFBX[i] = new EnemyHuman;
		cutsceneEnemyFBX[i]->Initialize();
		if (i == 0)
		{
			cutsceneEnemyFBX[i]->SetPosition({ 0.0f, 15.0f, 0.0f });
			cutsceneEnemyFBX[i]->SetRotation({ 0.0f, 180.0f, 0.0f });
		}
		else
		{
			cutsceneEnemyFBX[i]->SetPosition({ rand() % 400 - 200 * 1.0f, 18.0f, rand() % 300 - 100 * 1.0f });
			cutsceneEnemyFBX[i]->SetRotation({ 0.0f, rand() % 360 * 1.0f, 0.0f });
		}
		cutsceneEnemyFBX[i]->SetHomePosition({0.0f, 0.0f});
		cutsceneEnemyFBX[i]->HP = 0.0f;
		cutsceneEnemyFBX[i]->modelChange = true;
		cutsceneEnemyFBX[i]->SetEnumStatus(EnemyHuman::DEAD);
	}

	positionMODEL = Model::CreateFromOBJ("yuka");
	for (int i = 0; i < 8; i++)
	{
		cutsceneEnemyPositionOBJ[i] = PlayerPositionObject::Create();
		cutsceneEnemyPositionOBJ[i]->SetModel(positionMODEL);
		cutsceneEnemyPositionOBJ[i]->SetPosition(cutsceneEnemyFBX[i]->GetPosition());
	}

	// Camera initial values
	camera->SetTarget({ cutsceneEnemyFBX[0]->GetPosition().x, 20.0f, cutsceneEnemyFBX[0]->GetPosition().z });
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(48.0f);
	camera->cutsceneActive = true;
}

void GameClearCutscene::Update()
{
	for (int i = 0; i < 8; i++)
	{
		cutsceneEnemyFBX[i]->SetEnumStatus(EnemyHuman::DEAD);
		cutsceneEnemyFBX[i]->HP = 0.0f;
		cutsceneEnemyFBX[i]->SetPosition(cutsceneEnemyPositionOBJ[i]->GetPosition());
	}

	lightGroup->Update();
	particleMan->Update();

	camera->Update();

	switch (cutsceneStatus)
	{
	case PLAYERATTACK:
		degrees += 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

		if (degrees >= 120.0f)
		{
			camera->wCutscene = true;
			degrees = 0.0f;
			cutsceneStatus = LOOKRIGHT;
			break;
		}
		break;
	case LOOKRIGHT:
		degrees += 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		camera->SetTarget({ sinf(XMConvertToRadians(degrees)) * 48.0f, 20.0f, cosf(XMConvertToRadians(degrees)) * 48.0f - 48.0f });

		if (degrees >= 45.0f)
		{
			camera->wCutscenePart = 1;
			cutsceneStatus = LOOKLEFT;
			break;
		}
		break;
	case LOOKLEFT:
		degrees -= 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		camera->SetTarget({ sinf(XMConvertToRadians(degrees)) * 48.0f, 20.0f, cosf(XMConvertToRadians(degrees)) * 48.0f - 48.0f });

		if (degrees <= -45.0f)
		{
			camera->wCutscenePart = 0;
			cutsceneStatus = RETURNCENTER;
			break;
		}
		break;
	case RETURNCENTER:
		degrees += 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		camera->SetTarget({ sinf(XMConvertToRadians(degrees)) * 48.0f, 20.0f, cosf(XMConvertToRadians(degrees)) * 48.0f - 48.0f });

		if (degrees >= 0.0f)
		{
			degrees = 0.0f;
			camera->SetTarget({ sinf(XMConvertToRadians(degrees)) * 48.0f, 20.0f, cosf(XMConvertToRadians(degrees)) * 48.0f - 48.0f });
			camera->wCutscenePart = 2;
			cutsceneStatus = PAUSE;
			break;
		}
		break;
	case PAUSE:
		degrees += 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

		if (degrees >= 40.0f)
		{
			degrees = 0.0f;
			camera->wCutscenePart = 3;
			cutsceneStatus = LOOKUP;
			break;
		}
		break;
	case LOOKUP:
		degrees += 20.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		camera->SetTarget({ cutsceneEnemyFBX[0]->GetPosition().x, sinf(XMConvertToRadians(degrees)) * 48.0f + 20.0f, -sinf(XMConvertToRadians(degrees)) * 48.0f });

		if (degrees >= 90.0f)
		{
			camera->wCutscenePart = 2;
			cutsceneStatus = GREYSCREEN;
			break;
		}
		break;
	case GREYSCREEN:
		fadeSpriteALPHA += 0.4f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		fadeSPRITE->SetColor({ 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
		if (fadeSpriteALPHA >= 1.0f)
		{
			fadeSpriteALPHA = 1.0f;
			cutsceneStatus = GAMECLEARSCREEN;
			break;
		}
		break;
	case GAMECLEARSCREEN:
		break;
	}

#pragma region updates
	for (int i = 0; i < 8; i++)
	{
		cutsceneEnemyFBX[i]->Update();
		cutsceneEnemyPositionOBJ[i]->Update();
	}
	skydomeOBJ->Update();
	groundOBJ->Update();
	collisionManager->CheckAllCollisions();
#pragma endregion
}

void GameClearCutscene::Draw()
{
	// Setting command list
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 3DDrawing
	Object3d::PreDraw(cmdList);

	// 3D Object Drawing
	for (int i = 0; i < 8; i++)
	{
		cutsceneEnemyFBX[i]->Draw(cmdList);
	}

	skydomeOBJ->Draw();
	groundOBJ->Draw();

	// Particle drawing
	//particleMan->Draw(cmdList);

	Object3d::PostDraw();
#pragma endregion

#pragma region foregroundSpriteDrawing
	Sprite::PreDraw(cmdList);

	// Foreground sprite drawing

	// Debug text drawing
	debugText->DrawAll(cmdList);

	if (fadeSpriteALPHA > 0.0f)
	{
		fadeSPRITE->Draw();
	}

	if (cutsceneStatus == GAMECLEARSCREEN)
	{
		gameClearSPRITE->Draw();
	}

	// Sprite post draw
	Sprite::PostDraw();
#pragma endregion
}