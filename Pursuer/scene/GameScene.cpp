#include "GameScene.h"

#include <cassert>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <time.h>
#include <thread>

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;
extern float loadingProgress = 0.0f;
extern std::atomic<float> loadingPercent{0.0f};
extern bool change = false;

extern int keyOrMouse = 0; // 0 = keyboard, 1 = mouse

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	safe_delete(titleScreen);
	safe_delete(tutorialArea);
	safe_delete(baseArea);
	safe_delete(gameOverCutscene);
	safe_delete(gameClearCutscene);
	audio->Finalize();
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio * audio)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	audio->Initialize();
	audio->LoadWave("Title.wav");
	audio->LoadWave("MainArea.wav");
	audio->LoadWave("GameOver.wav");
	audio->LoadWave("GameClear.wav");

	titleScreen = new TitleScreen;
	titleScreen->Initialize(dxCommon, input, audio);

	if (!Sprite::LoadTexture(115, "BlackScreen.png")) { assert(0); return; } // Black Screen
	fadeSPRITE = Sprite::Create(115, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, fadeSpriteAlpha });
}

void GameScene::Update()
{
	switch (page)
	{
	case 0:
		gameOverCutscene = nullptr;
		titleScreen->Update();
		loadingProgress = 0.1f;

		if (titleScreen->gameStart)
		{
			if (loadingScreen == nullptr)
			{
				loadingScreen = new Loading;
				loadingScreen->Initialize(dxCommon, input, audio);
			}
			showLoading = true;

			if (titleScreen->tutorialSelectionYesBOOL == true && titleScreen->keyboardContrllerSelectionBOOL)
			{
				tutorialOrBase = 0;
				titleScreen->~TitleScreen();
				titleScreen = nullptr;
				page = 1;
				break;
			}
			else if (titleScreen->tutorialSelectionYesBOOL == false && titleScreen->keyboardContrllerSelectionBOOL)
			{
				tutorialOrBase = 1;
				titleScreen->~TitleScreen();
				titleScreen = nullptr;
				page = 1;
				break;
			}
			else if (titleScreen->selection == 2)
			{
				mainLoop = false;
				break;
			}
		}
		break;
	case 1:
		if (tutorialOrBase == 0)
		{
			std::thread th1(&GameScene::thread1, this);
			std::thread th3(&GameScene::thread3, this);
			th3.join();
			th1.join();
			showLoading = false;
		}
		else if (tutorialOrBase == 1)
		{
			std::thread th2(&GameScene::thread2, this);
			std::thread th3(&GameScene::thread3, this);
			th3.join();
			th2.join();
			showLoading = false;
		}

		if (tutorialArea != nullptr)
		{
			page = 2;
			break;
		}
		else if (baseArea != nullptr)
		{
			page = 3;
			break;
		}
		break;
	case 2:
		if (tutorialArea != nullptr)
		{
			tutorialArea->Update();
		}

		if (loadingProgress > 0.0f)
		{
			loadingProgress = 0.0f;
		}

		if (tutorialArea->deletion)
		{
			tutorialArea->~TutorialArea();
			page = 1;
			tutorialOrBase = 1;
			tutorialArea->deletion = false;
			break;
		}
		break;
	case 3:
		if (baseArea != nullptr)
		{
			baseArea->Update();
		}

		if (loadingProgress > 0.0f)
		{
			loadingProgress = 0.0f;
		}

		if (baseArea->deletion)
		{
			baseArea->~BaseArea();
			if (baseArea->result == 1)
			{
				gameOverCutscene = new GameOverCutscene;
				gameOverCutscene->Initialize(dxCommon, input, audio);
				page = 4;
			}
			else if (baseArea->result == 2)
			{
				gameClearCutscene = new GameClearCutscene;
				gameClearCutscene->Initialize(dxCommon, input, audio);
				page = 5;
			}
			baseArea->deletion = false;
			baseArea = nullptr;
			break;
		}
		break;
	case 4:
		if (gameOverCutscene != nullptr)
		{
			gameOverCutscene->Update();
		}

		if (gameOverCutscene->cutsceneStatus == GameOverCutscene::GAMEOVERSCREEN)
		{
			if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
			{
				titleScreen = new TitleScreen;
				titleScreen->Initialize(dxCommon, input, audio);
				gameOverCutscene->~GameOverCutscene();
				page = 0;
				break;
			}
		}
		break;
	case 5:
		if (gameClearCutscene != nullptr)
		{
			gameClearCutscene->Update();
		}

		if (gameClearCutscene->cutsceneStatus == GameClearCutscene::GAMECLEARSCREEN)
		{
			if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
			{
				titleScreen = new TitleScreen;
				titleScreen->Initialize(dxCommon, input, audio);
				gameClearCutscene->~GameClearCutscene();
				page = 0;
				break;
			}
		}
		break;
	}
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	
	switch (page)
	{
	case 0:
		if (showLoading)
		{
			loadingScreen->Draw();
		}
		else
		{
			titleScreen->Draw();
		}
		break;
	case 1:
		loadingScreen->Draw();
		break;
	case 2:
		tutorialArea->Draw();
		break;
	case 3:
		baseArea->Draw();
		break;
	case 4:
		gameOverCutscene->Draw();
		break;
	case 5:
		gameClearCutscene->Draw();
		break;
	}

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	/*if (page == 1)
	{
		fadeSPRITE->Draw();
	}*/
	
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

void GameScene::CreatePipelines()
{
	switch (createPipelineLevel)
	{
	case 0:
		PipelineSetup::CreatePipeline("Sprite", SPRITE);
		break;
	case 1:
		PipelineSetup::CreatePipeline("FBX", FBX);
		PipelineSetup::CreatePipeline("3DObj", OBJ);
		break;
	case 2:
		PipelineSetup::CreatePipeline("Particle", PARTICLE, ADD);
		break;
	default:
		createPipelineFinish = true;
		break;
	}
	createPipelineLevel++;
}

void GameScene::LoadResources()
{
	switch (loadResourceLevel)
	{
	case 0:
		// Texture Initialization
		Texture::Initialize();
		// Debug Text Loading
		Texture::LoadTexture("DebugText", "debugfont.png");
		// Black Screen Texture
		Texture::LoadTexture("BlackScreen", "BlackScreen.png");
		// Base Area Sprites
		Texture::LoadTexture("HPBar", "HPBar.png");
		Texture::LoadTexture("HPBarFrame", "HPBarFrame.png");
		Texture::LoadTexture("STBar", "STBar.png");
		Texture::LoadTexture("STBarFrame", "STBarFrame.png");
		Texture::LoadTexture("Mission1", "Mission1.png");
		Texture::LoadTexture("BaseAreaMinimap", "BaseAreaMinimap.png");
		Texture::LoadTexture("PlayerMinimapSprite", "PlayerMinimapSprite.png");
		Texture::LoadTexture("EnemyMinimapSprite", "EnemyMinimapSprite.png");
		Texture::LoadTexture("EnemyHumanHPBar", "EnemyHumanHPBar.png");
		Texture::LoadTexture("EnemyHumanHPBarFrame", "EnemyHumanHPBarFrame.png");
		Texture::LoadTexture("DamageOverlay", "DamageOverlay.png");
		Texture::LoadTexture("Heal", "Heal.png");
		Texture::LoadTexture("HealK", "HealK.png");
		Texture::LoadTexture("HealC", "HealC.png");
		break;
	case 1:
		break;
	default:
		loadResourceFinish = true;
		break;
	}
	loadResourceLevel++;
}

void GameScene::thread1()
{
	tutorialArea = new TutorialArea;
	if (baseArea != nullptr)
	{
		baseArea = nullptr;
	}
	tutorialArea->Initialize(dxCommon, input, audio);
}

void GameScene::thread2()
{
	baseArea = new BaseArea;
	if (tutorialArea != nullptr)
	{
		tutorialArea = nullptr;
	}
	baseArea->Initialize(dxCommon, input, audio);
}

void GameScene::thread3()
{
	while (loadingProgress < 100.0f)
	{
		loadingScreen->addLoadingPercent(loadingProgress);
		loadingScreen->Update(loadingPercent);
	}
}

void GameScene::addLoadingPercent(float percent)
{
	if (loadingScreen != nullptr)
	{
		loadingScreen->addLoadingPercent(percent);
	}
}