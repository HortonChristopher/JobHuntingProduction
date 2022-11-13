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
	if (!Sprite::LoadTexture(96, L"Resources/P1.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(97, L"Resources/P2.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(98, L"Resources/P3.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(99, L"Resources/P4.png")) { assert(0); return; }

	// 背景スプライト生成
	p1 = Sprite::Create(96, { 0.0f, 0.0f });
	p2 = Sprite::Create(97, { 0.0f, 0.0f });
	p3 = Sprite::Create(98, { 0.0f, 0.0f });
	p4 = Sprite::Create(99, { 0.0f, 0.0f });

	titleScreen = new TitleScreen;
	titleScreen->Initialize(dxCommon, input, audio);
}

void GameScene::Update()
{
	if (page < 2)
	{
		titleScreen->Update();

		if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
		{
			if (page == 1)
			{
				titleScreen->~TitleScreen();
				titleScreen = nullptr;
				baseArea = new BaseArea;
				baseArea->initialization = true;
				page++;
			}
			else
			{
				page++;
			}
		}
	}

	if (page == 2)
	{
		if (baseArea->initialization)
		{
			baseArea->Initialize(dxCommon, input, audio);
			baseArea->initialization = false;
		}

		if (baseArea->deletion)
		{
			baseArea->~BaseArea();
			if (baseArea->result == 1)
			{
				page = 3;
			}
			else if (baseArea->result == 2)
			{
				page = 4;
			}
			baseArea->deletion = false;
			baseArea = nullptr;
		}

		if (baseArea != nullptr)
		{
			baseArea->Update();
		}
	}

	if (page > 2)
	{
		if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
		{
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

	if (page < 2)
	{
		titleScreen->Draw();
	}
	if (page == 2)
	{
		baseArea->Draw();
	}

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	/*if (page == 0)
	{
		p1->Draw();
	}
	if (page == 1)
	{
		p2->Draw();
	}*/
	if (page == 3)
	{
		p3->Draw();
	}
	if (page == 4)
	{
		p4->Draw();
	}
	
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}