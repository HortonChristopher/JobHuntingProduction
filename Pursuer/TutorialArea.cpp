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
	for (int i = 0; i < 5; i++) { safe_delete(tutorialTextSPRITE[i]); }
	for (int i = 0; i < 1; i++) { safe_delete(tutorialMissionSPRITE[i]); }
	safe_delete(playerFBX);
	safe_delete(playerPositionOBJ);
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
	EnemyHuman::SetDevice(dxCommon->GetDevice());
	TutorialPlayer::SetDevice(dxCommon->GetDevice());

	// Setting camera
	Object3d::SetCamera(camera);
	FBXGeneration::SetCamera(camera);
	EnemyHuman::SetCamera(camera);
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

	if (!Sprite::LoadTexture(99, L"Resources/PlayerMinimapSprite.png")) { assert(0); return; } // Player minimap texture
	if (!Sprite::LoadTexture(98, L"Resources/EnemyMinimapSprite.png")) { assert(0); return; } // Enemy minimap texture
	if (!Sprite::LoadTexture(97, L"Resources/MinimapTutorialMask.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(96, L"Resources/TutorialMinimap.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(95, L"Resources/StaminaTutorialMask.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(94, L"Resources/TutorialMission1.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(93, L"Resources/TutorialMission2.png")) { assert(0); return; }

	if (!Sprite::LoadTexture(85, L"Resources/LoadingBar.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(86, L"Resources/LoadingBarFrame.png")) { assert(0); return; }

	if (!Sprite::LoadTexture(87, L"Resources/HPBar.png")) { assert(0); return; } // HP bar texture
	if (!Sprite::LoadTexture(88, L"Resources/HPBarFrame.png")) { assert(0); return; } // HP bar frame texture
	if (!Sprite::LoadTexture(89, L"Resources/STBar.png")) { assert(0); return; } // ST bar texture
	if (!Sprite::LoadTexture(90, L"Resources/STBarFrame.png")) { assert(0); return; } // ST bar frame texture

	tutorialTextFrameSPRITE = Sprite::Create(1, { 390.0f, 300.0f });
	for (int i = 0; i < 13; i++)
	{
		tutorialTextSPRITE[i] = Sprite::Create((i + 2), { 390.0f, 300.0f });
	}
	for (int i = 94; i > 92; i--)
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

	// 3D Object Create
	skydomeOBJ = Object3d::Create();

	// Model Creation
	groundMODEL = Model::CreateFromOBJ("TutorialStage");
	skydomeMODEL = Model::CreateFromOBJ("skydome");
	
	// Touchable Objection Creation
	groundOBJ = TouchableObject::Create(groundMODEL);
	skydomeOBJ->SetModel(skydomeMODEL);
	skydomeOBJ->SetScale({ 5,5,5 });

	positionMODEL = Model::CreateFromOBJ("yuka");

	// Player initialization
	playerFBX = new TutorialPlayer;
	playerFBX->Initialize();
	playerPositionOBJ = PlayerPositionObject::Create();
	playerPositionOBJ->SetModel(positionMODEL);
	playerFBX->SetPosition({ 0.0f, 0.0f, -700.0f });

	// Ground Aspects
	groundOBJ->SetPosition({ 0, 0, 0 });
	groundOBJ->SetScale({ 50, 10, 50 });
	playerPositionOBJ->SetScale({ 10, 10, 10 });

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
		//tutorialActive = false;
		//playerFBX->tutorialPart = 2;
		if (playerFBX->GetPosition().z >= 500.0f)
		{
			deletion = true;
		}
		break;
	}

	if (tutorialStatus != INTROCUTSCENE && tutorialStatus != MOVEMENTTUTORIAL)
	{
		HPBarSPRITE->SetSize({ playerFBX->hp * 20.0f, 20.0f });
		STBarSPRITE->SetSize({ playerFBX->stamina * 2.0f, 20.0f });

		tutorialMinimapPlayerSPRITE->SetPosition({ playerFBX->GetPosition().x * 0.24f + 165.0f, playerFBX->GetPosition().z * 0.24f + 545.0f });
		//tutorialMinimapEnemySPRITE->SetPosition({ tutorialEnemyFBX[i]->GetPosition().x * 0.24f + 165.0f, tutorialEnemyFBX[i]->GetPosition().z * 0.24f + 545.0f });

		if (input->PushKey(DIK_LSHIFT) && playerFBX->stamina > 0.0f || input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && playerFBX->stamina > 0.0f)
		{
			ParticleCreation(playerFBX->GetPosition().x, playerFBX->GetPosition().y, playerFBX->GetPosition().z, 10, -1.0f, 1.0f);
		}
	}

	playerFBX->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });
	playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });

#pragma region updates
	groundOBJ->Update();
	skydomeOBJ->Update();
	playerFBX->Update();
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
	//playerPositionOBJ->Draw();
	groundOBJ->Draw();
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
				break;
			}
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
			break;
		}
	}

	if (tutorialStatus != INTROCUTSCENE && tutorialStatus != MOVEMENTTUTORIAL)
	{
		STBarSPRITE->Draw();
		STBarFrameSPRITE->Draw();
	}

	if (tutorialStatus != INTROCUTSCENE && tutorialStatus != MOVEMENTTUTORIAL && tutorialStatus != STAMINATUTORIAL)
	{
		tutorialMinimapSPRITE->Draw();
		tutorialMinimapEnemySPRITE->Draw();
		tutorialMinimapPlayerSPRITE->Draw();
		HPBarSPRITE->Draw();
		HPBarFrameSPRITE->Draw();
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