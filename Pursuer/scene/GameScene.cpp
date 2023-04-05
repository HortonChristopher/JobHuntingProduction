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

	menu = new Menu;
	//menu->Initialize(dxCommon, input);

	if (!Sprite::LoadTexture(115, "BlackScreen.png")) { assert(0); return; } // Black Screen
	fadeSPRITE = Sprite::Create(115, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, fadeSpriteAlpha });
}

void GameScene::Update()
{
	switch (page)
	{
	case 0:
		gameOverCutscene = nullptr;
		menu = nullptr;
		pause = false;
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
			while (loadingScreen->percentage < 100.0f)
			{
				loadingScreen->addLoadingPercent(40.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f));
				loadingScreen->Update();
			}
			th1.join();
			showLoading = false;
		}
		else if (tutorialOrBase == 1)
		{
			std::thread th2(&GameScene::thread2, this);
			while (loadingScreen->percentage < 100.0f)
			{
				loadingScreen->addLoadingPercent(40.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f));
				loadingScreen->Update();
			}
			th2.join();
			showLoading = false;
		}

		menu = new Menu;
		menu->Initialize(dxCommon, input);

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
			if (!pause)
			{
				tutorialArea->Update();
			}
		}

		if (tutorialArea->deletion)
		{
			tutorialArea->~TutorialArea();
			page = 1;
			tutorialOrBase = 1;
			tutorialArea->deletion = false;
			break;
		}

		if (pause)
		{
			menu->Update();

			if (menu->returnTitle)
			{
				tutorialArea->~TutorialArea();
				titleScreen = new TitleScreen;
				titleScreen->Initialize(dxCommon, input, audio);
				tutorialOrBase = 1;
				tutorialArea->deletion = false;
				menu->returnTitle = false;
				page = 0;
			}
		}
		break;
	case 3:
		if (baseArea != nullptr)
		{
			if (!pause)
			{
				baseArea->Update();
			}
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

		if (pause)
		{
			menu->Update();

			if (menu->returnTitle)
			{
				baseArea->~BaseArea();
				titleScreen = new TitleScreen;
				titleScreen->Initialize(dxCommon, input, audio);
				baseArea->deletion = false;
				baseArea = nullptr;
				menu->returnTitle = false;
				page = 0;
			}
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

	if (input->TriggerKey(DIK_ESCAPE) || input->TriggerControllerButton(XINPUT_GAMEPAD_START))
	{
		if (pause)
		{
			menu->Initialize(dxCommon, input);
			pause = false;
		}
		else
		{
			pause = true;
		}
		return;
	}
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	
	switch (page)
	{
	case 0:
		titleScreen->Draw();
		break;
	case 1:
		loadingScreen->Draw();
		break;
	case 2:
		tutorialArea->Draw();
		if (pause)
		{
			menu->Draw();
		}
		break;
	case 3:
		baseArea->Draw();
		if (pause)
		{
			menu->Draw();
		}
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
	//Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	/*switch (page)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		if (pause)
		{
			menu->Draw();
		}
		break;
	case 3:
		if (pause)
		{
			menu->Draw();
		}
		break;
	case 4:
		break;
	case 5:
		break;
	}*/
	
	// スプライト描画後処理
	//Sprite::PostDraw();
#pragma endregion
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
	
}

void GameScene::addLoadingPercent(float percent)
{
	if (loadingScreen != nullptr)
	{
		loadingScreen->addLoadingPercent(percent);
	}
}