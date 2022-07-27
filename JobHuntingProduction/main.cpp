﻿#include "WinApp.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "GameScene.h"
#include "LightGroup.h"
#include "ParticleManager.h"
#include "Object3d.h"
#include "FbxLoader/FbxLoader.h"
#include "2d/PostEffect.h"
#include "input/Input.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
{
#ifdef _Debug
	ID3D12Debug1* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	// 汎用機能
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;	
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;
	PostEffect* postEffect = nullptr;

	// ゲームウィンドウの作成
	win = new WinApp();
	win->CreateGameWindow();
		
	//DirectX初期化処理
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(win);

#ifdef _Debug
	ID3D12InfoQueue* infoQueue;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->Release();
	}
#endif

#pragma region 汎用機能初期化
	// 入力の初期化
	input = Input::GetInstance();
	if (!input->Initialize(win->GetInstance(), win->GetHwnd())) {
		assert(0);
		return 1;
	}
	// オーディオの初期化
	audio = new Audio();
	if (!audio->Initialize()) {
		assert(0);
		return 1;
	}
	// スプライト静的初期化
	if (!Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height)) {
		assert(0);
		return 1;
	}

	// Post effect texture reading
	//Sprite::LoadTexture(100, L"Resources/white1x1.png");

	// Post effect initialization
	postEffect = new PostEffect();
	postEffect->Initialize();

	// ライト静的初期化
	LightGroup::StaticInitialize(dxCommon->GetDevice());
	// パーティクルマネージャ初期化
	ParticleManager::GetInstance()->Initialize(dxCommon->GetDevice());

	Object3d::StaticInitialize(dxCommon->GetDevice());

	// FBX
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());
#pragma endregion

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize(dxCommon, input, audio);
	
	// メインループ
	while (true)
	{
		// メッセージ処理
		if (win->ProcessMessage()) {	break; }

		// 入力関連の毎フレーム処理
		input->Update();
		// ゲームシーンの毎フレーム処理
		gameScene->Update();

		
		// Render Texture Drawing
		postEffect->PreDrawScene(dxCommon->GetCommandList());
		// ゲームシーンの描画
		gameScene->Draw();
		postEffect->PostDrawScene(dxCommon->GetCommandList());
		// 描画開始
		dxCommon->PreDraw();
		// Post effect Drawing
		postEffect->Draw(dxCommon->GetCommandList());
		// 描画終了
		dxCommon->PostDraw();
	}
	// 各種解放
	safe_delete(gameScene);
	safe_delete(audio);
	safe_delete(dxCommon);

	delete postEffect;

	FbxLoader::GetInstance()->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
	safe_delete(win);

	return 0;
}