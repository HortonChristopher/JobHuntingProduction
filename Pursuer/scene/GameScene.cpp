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

extern int keyOrMouse = 0; // 0 = keyboard, 1 = mouse

GameScene::GameScene()
{
}

GameScene::~GameScene()
{

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

	// テクスチャ読み込み
	if (!Sprite::LoadTexture(200, "p3.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(201, "p4.png")) { assert(0); return; }

	// 背景スプライト生成
	gameOverSPRITE = Sprite::Create(200, { 0,0 });
	gameClearSPRITE = Sprite::Create(201, { 0,0 });

	titleScreen = new TitleScreen;
	titleScreen->Initialize(dxCommon, input, audio);
}

void GameScene::Update()
{
	if (page < 1)
	{
		titleScreen->Update();

		if (titleScreen->gameStart)
		{
			if (titleScreen->tutorialSelectionYesBOOL == true)
			{
				tutorialArea = new TutorialArea;
				tutorialArea->initialize = true;
				titleScreen->~TitleScreen();
				titleScreen = nullptr;
				page = 2;
			}
			else if (titleScreen->tutorialSelectionYesBOOL == false)
			{
				baseArea = new BaseArea;
				baseArea->initialization = true;
				titleScreen->~TitleScreen();
				titleScreen = nullptr;
				page = 3;
			}
			else if (titleScreen->selection == 2)
			{
				mainLoop = false;
			}
		}
	}

	/*if (page == 1)
	{
		if (input->TriggerKey(DIK_T) || input->TriggerControllerButton(XINPUT_GAMEPAD_START))
		{
			tutorialArea = new TutorialArea;
			tutorialArea->initialize = true;
			page = 2;
		}
	}*/

	if (page == 2)
	{
		if (tutorialArea->initialize)
		{
			tutorialArea->Initialize(dxCommon, input, audio);
			tutorialArea->initialize = false;
		}

		if (tutorialArea != nullptr)
		{
			tutorialArea->Update();
		}

		if (tutorialArea->deletion)
		{
			tutorialArea->~TutorialArea();
			baseArea = new BaseArea;
			baseArea->initialization = true;
			page = 3;
			tutorialArea->deletion = false;
		}
	}

	if (page == 3)
	{
		if (baseArea->initialization)
		{
			if (tutorialArea != nullptr)
			{
				tutorialArea = nullptr;
			}
			baseArea->Initialize(dxCommon, input, audio);
			baseArea->initialization = false;
		}

		if (baseArea->deletion)
		{
			baseArea->~BaseArea();
			if (baseArea->result == 1)
			{
				page = 4;
			}
			else if (baseArea->result == 2)
			{
				page = 5;
			}
			baseArea->deletion = false;
			baseArea = nullptr;
		}

		if (baseArea != nullptr)
		{
			baseArea->Update();
		}
	}

	if (page > 3)
	{
		if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
		{
			titleScreen = new TitleScreen;
			titleScreen->Initialize(dxCommon, input, audio);
			page = 0;
		}
	}

	//Debug Start
	//char msgbuf[256];
	//char msgbuf2[256];
	//char msgbuf3[256];

	//sprintf_s(msgbuf, 256, "X: %f\n", object2->timer);
	//sprintf_s(msgbuf2, 256, "Z: %f\n", object1->GetPosition().z);
	//sprintf_s(msgbuf3, 256, "Z: %f\n", objectPosition.z);
	//OutputDebugStringA(msgbuf);
	//OutputDebugStringA(msgbuf2);
	//OutputDebugStringA(msgbuf3);
	//Debug End
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

	if (page < 1)
	{
		titleScreen->Draw();
	}
	if (page == 2)
	{
		tutorialArea->Draw();
	}
	if (page == 3)
	{
		baseArea->Draw();
	}

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	if (page == 4)
	{
		gameOverSPRITE->Draw();
	}
	if (page == 5)
	{
		gameClearSPRITE->Draw();
	}
	
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}