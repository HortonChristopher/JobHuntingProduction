#include "TitleScreen.h"

using namespace DirectX;

extern DeltaTime* deltaTime;

extern int keyOrMouse;

TitleScreen::TitleScreen()
{
}

TitleScreen::~TitleScreen()
{
	safe_delete(skydomeMODEL);
	safe_delete(groundMODEL);
	safe_delete(extendedGroundMODEL);
	safe_delete(skydomeOBJ);
	safe_delete(groundOBJ);
	safe_delete(extendedGroundOBJ);
	safe_delete(titleSPRITE);
}

void TitleScreen::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->audio = audio;
	this->input = input;

	audio->PlayWave("Title.wav", audio->titleVolume, true);

	camera = new DebugCamera(WinApp::window_width, WinApp::window_height, input);

	// Particle Manager initialization/generation
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	// Light Group Creation
	lightGroup = LightGroup::Create();

	// Setting camera
	Object3d::SetCamera(camera);

	// Loading debug text
	if (!Sprite::LoadTexture(debugTextTexNumber, "debugfont.png")) { assert(0); return; }

	if (!Sprite::LoadTexture(8, "Title.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(9, "TitleStartStop.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(10, "BlackScreen.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(99, "TitleTutorialSelection_1.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(100, "TitleTutorialSelection_2.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(101, "TitleTutorialSelection_k.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(102, "TitleTutorialSelection_c.png")) { assert(0); return; }
	titleSPRITE = Sprite::Create(8, { 0,0 }, { 1.0f, 1.0f, 1.0f, titleSpriteALPHA });
	titleStartStopSPRITE = Sprite::Create(9, { 0,0 });
	fadeSPRITE = Sprite::Create(10, { 0,0 }, { 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });
	for (int i = 0; i < 2; i++)
	{
		tutorialSelectionSPRITE[i] = Sprite::Create(i + 99, { 390.0f, 300.0f });
		keyboardControllerSelectionSPRITE[i] = Sprite::Create(i + 101, { 390.0f, 300.0f });
	}

	// Debug text initialization
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	// Model Initialization
	skydomeMODEL = Model::CreateFromOBJ("skydome");
	groundMODEL = Model::CreateFromOBJ("Landscape2");
	extendedGroundMODEL = Model::CreateFromOBJ("ground");

	// 3D object creation
	skydomeOBJ = Object3d::Create();
	skydomeOBJ->SetModel(skydomeMODEL);
	groundOBJ = TouchableObject::Create(groundMODEL);
	extendedGroundOBJ = TouchableObject::Create(extendedGroundMODEL);

	// Ground scale
	groundOBJ->SetScale({ 2, 0.5f, 1 });
	extendedGroundOBJ->SetScale({ 1000, 1, 1000 });

	// Ground position
	groundOBJ->SetPosition({ 0, -35, 0 });
	extendedGroundOBJ->SetPosition({ 0, -40, 0 });

	skydomeOBJ->SetPosition({ 0, -25, 0 });
	// Skydome scale
	skydomeOBJ->SetScale({ 5,5,5 });

	// Camera initial values
	camera->SetTarget({ 0.0f, 30.0f, 0.0f });
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(48.0f);

	camera->title = true;

	degrees = 0;
}

void TitleScreen::Update()
{
	lightGroup->Update();
	particleMan->Update();
	camera->Update();

	degrees += 10.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);

	if (keyboardContrllerSelectionBOOL && !selectionBOOL)
	{
		if (selection == 0)
		{
			if (input->TriggerKey(DIK_S) || input->TriggerLStickDown())
			{
				selection = 1;
			}
		}
		else
		{
			if (input->TriggerKey(DIK_W) || input->TriggerLStickUp())
			{
				selection = 0;
			}
		}

		if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
		{
			if (selection == 0)
			{
				keyOrMouse = 1;
			}
			else
			{
				keyOrMouse = 0;
			}

			selectionBOOL = true;
		}

		if (input->TriggerKey(DIK_X) || input->TriggerControllerButton(XINPUT_GAMEPAD_B))
		{
			selection = 0;
			keyboardContrllerSelectionBOOL = false;
		}
	}

	if (tutorialSelectionBOOL && !selectionBOOL)
	{
		if (selection == 0)
		{
			if (input->TriggerKey(DIK_S) || input->TriggerLStickDown())
			{
				selection = 1;
			}
		}
		else
		{
			if (input->TriggerKey(DIK_W) || input->TriggerLStickUp())
			{
				selection = 0;
			}
		}

		if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
		{
			if (selection == 0)
			{
				tutorialSelectionYesBOOL = true;
			}
			else
			{
				tutorialSelectionYesBOOL = false;
			}

			tutorialSelectionBOOL = false;
			keyboardContrllerSelectionBOOL = true;
		}

		if (input->TriggerKey(DIK_X) || input->TriggerControllerButton(XINPUT_GAMEPAD_B))
		{
			selection = 0;
			tutorialSelectionBOOL = false;
		}
	}

	if (titleSpriteALPHA < 1.0f)
	{
		if (!firstRun)
		{
			titleSpriteALPHA += 0.5f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}
		else
		{
			firstRun = false;
		}
	}
	else
	{
		titleSpriteALPHA = 1.0f;
		if (!selectionBOOL && !particleMovement && !tutorialSelectionBOOL && !keyboardContrllerSelectionBOOL)
		{
			if (selection == 0)
			{
				if (input->TriggerKey(DIK_S) || input->TriggerLStickDown())
				{
					particleMovement = true;
				}
				else if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
				{
					selection = 0;
					tutorialSelectionBOOL = true;
				}
			}
			else if (selection == 2)
			{
				if (input->TriggerKey(DIK_W) || input->TriggerLStickUp())
				{
					particleMovement = true;
				}
				else if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
				{
					selectionBOOL = true;
				}
			}
		}
	}

	if (particleMovement)
	{
		if (selection == 0)
		{
			particleYPosition -= 0.5f;
			if (particleYPosition <= 4.0f)
			{
				particleYPosition = 4.0f;
				particleMovement = false;
				selection = 2;
			}
		}
		else if (selection == 2)
		{
			particleYPosition += 0.5f;
			if (particleYPosition >= 23.0f)
			{
				particleYPosition = 23.0f;
				particleMovement = false;
				selection = 0;
			}
		}
	}

	if (selectionBOOL)
	{
		fadeSpriteALPHA += 0.4f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		
		if (fadeSpriteALPHA >= 1.0f)
		{
			audio->StopWave("Title.wav");
			gameStart = true;
		}
	}

	titleSPRITE->SetColor({ 1.0f, 1.0f, 1.0f, titleSpriteALPHA });
	fadeSPRITE->SetColor({ 1.0f, 1.0f, 1.0f, fadeSpriteALPHA });

	camera->SetTarget({ sinf(XMConvertToRadians(degrees)) * 10.0f, 30.0f, cosf(XMConvertToRadians(degrees)) * 10.0f });
	ParticleCreation(sinf(XMConvertToRadians(degrees)) * 10.0f, particleYPosition, cosf(XMConvertToRadians(degrees)) * 10.0f, 20, 0, 2.0f);

	skydomeOBJ->Update();
	extendedGroundOBJ->Update();
	groundOBJ->Update();
}

void TitleScreen::Draw()
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

	skydomeOBJ->Draw();
	//extendedGroundOBJ->Draw();
	groundOBJ->Draw();

	if (titleSpriteALPHA >= 1.0f)
	{
		// Particle drawing
		particleMan->Draw(cmdList);
	}

	Object3d::PostDraw();
#pragma endregion

#pragma region foregroundSpriteDrawing
	Sprite::PreDraw(cmdList);

	// Foreground sprite drawing
	titleSPRITE->Draw();
	if (titleSpriteALPHA >= 1.0f)
	{
		titleStartStopSPRITE->Draw();
	}

	if (tutorialSelectionBOOL)
	{
		if (selection == 0)
		{
			tutorialSelectionSPRITE[0]->Draw();
		}
		else
		{
			tutorialSelectionSPRITE[1]->Draw();
		}
	}

	if (keyboardContrllerSelectionBOOL)
	{
		if (selection == 0)
		{
			keyboardControllerSelectionSPRITE[1]->Draw();
		}
		else
		{
			keyboardControllerSelectionSPRITE[0]->Draw();
		}
	}

	if (selectionBOOL)
	{
		fadeSPRITE->Draw();
	}

	// Debug text drawing
	debugText->DrawAll(cmdList);

	// Sprite post draw
	Sprite::PostDraw();
#pragma endregion
}

void TitleScreen::ParticleCreation(float x, float y, float z, int life, float offset, float start_scale)
{
	for (int i = 0; i < 10; i++) {
		// X,Y,Z‘S‚Ä[-5.0f,+5.0f]‚Åƒ‰ƒ“ƒ_ƒ€‚É•ª•z
		const float rnd_pos = 5.0f;
		const float rnd_pos2 = 15.0f;
		XMFLOAT3 pos{};
		pos.x = ((float)rand() / RAND_MAX * rnd_pos2 - rnd_pos2 / 2.0f) + x;
		pos.y = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + y + offset;
		pos.z = ((float)rand() / RAND_MAX * rnd_pos2 - rnd_pos2 / 2.0f) + z;

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