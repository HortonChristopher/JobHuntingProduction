#include "GameScene.h"
#include "FBXGeneration.h"
#include "FbxLoader/FbxLoader.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

extern XMFLOAT3 objectPosition = { 0, -10, 0 };
extern XMFLOAT3 objectRotation = { 0, 0, 0 };

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	safe_delete(spriteBG);
	safe_delete(lightGroup);
	safe_delete(object1);
	safe_delete(model1);
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

	// カメラ生成
	camera = new DebugCamera(WinApp::window_width, WinApp::window_height, input);

	// Device set
	FBXGeneration::SetDevice(dxCommon->GetDevice());
	// Camera set
	FBXGeneration::SetCamera(camera);

	// デバッグテキスト用テクスチャ読み込み
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
		return ;
	}
	// デバッグテキスト初期化
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	if (!Sprite::LoadTexture(1, L"Resources/background.png")) {
		assert(0);
		return;
	}
	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	// パーティクルマネージャ生成
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	// Specify the FBX model and read the file
	model1 = FbxLoader::GetInstance()->LoadModelFromFile("PlayerRunning");
	model2 = FbxLoader::GetInstance()->LoadModelFromFile("PlayerStanding");

	object1 = new FBXGeneration;
	object1->Initialize();
	object1->SetModel(model1);

	object2 = new FBXGeneration;
	object2->Initialize();
	object2->SetModel(model2);

	object3 = new FBXGeneration;
	object3->Initialize();
	object3->SetModel(model2);

	object4 = new FBXGeneration;
	object4->Initialize();
	object4->SetModel(model2);

	object5 = new FBXGeneration;
	object5->Initialize();
	object5->SetModel(model2);

	object6 = new FBXGeneration;
	object6->Initialize();
	object6->SetModel(model2);

	// テクスチャ2番に読み込み
	Sprite::LoadTexture(2, L"Resources/tex1.png");

	// ライト生成
	lightGroup = LightGroup::Create();

	// カメラ注視点をセット
	//camera->SetTarget({0, 20, 0});
	//camera->SetDistance(100.0f);
	camera->SetTarget({0, 0.0f, 0});
	camera->SetUp({ 0,1,0 });
	camera->SetDistance(32.0f);

	object1->SetPosition(objectPosition);
	object1->SetRotation(objectRotation);
	object6->SetPosition(objectPosition);
	object6->SetRotation(objectRotation);

	object2->SetPosition({ 0, -10, 100 });
	object2->SetRotation({ 0, 180, 0 });

	object3->SetPosition({ 100, -10, 0 });
	object3->SetRotation({ 0, 90, 0 });

	object4->SetPosition({ 0, -10, -100 });
	object4->SetRotation({ 0, 0, 0 });
	
	object5->SetPosition({ -100, -10, 0 });
	object5->SetRotation({ 0, 270, 0 });
}

void GameScene::Update()
{
	lightGroup->Update();
	camera->Update();
	particleMan->Update();

	// A,Dで旋回
	/*if (input->PushKey(DIK_A)) {
		objectRotation.y -= 2.0f;
	}
	else if (input->PushKey(DIK_D)) {
		objectRotation.y += 2.0f;
	}*/

	// 移動ベクトルをY軸周りの角度で回転
	XMVECTOR move = { 0,0,1.0f,0 };
	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(objectRotation.y));
	move = XMVector3TransformNormal(move, matRot);

	// 向いている方向に移動
	if (input->PushKey(DIK_S)) {
		objectPosition.x -= move.m128_f32[0];
		objectPosition.y -= move.m128_f32[1];
		objectPosition.z -= move.m128_f32[2];
	}
	else if (input->PushKey(DIK_W)) {
		objectPosition.x += move.m128_f32[0];
		objectPosition.y += move.m128_f32[1];
		objectPosition.z += move.m128_f32[2];
	}

	object1->SetPosition(objectPosition);
	object1->SetRotation(objectRotation);
	object6->SetPosition(objectPosition);
	object6->SetRotation(objectRotation);

	object1->Update();
	object2->Update();
	object3->Update();
	object4->Update();
	object5->Update();
	object6->Update();

	//Debug Start
	//char msgbuf[256];
	//char msgbuf2[256];
	//char msgbuf3[256];

	//sprintf_s(msgbuf, 256, "B: %d\n", objectRotation.y);
	//sprintf_s(msgbuf2, 256, "LB: %d\n", why3);
	//sprintf_s(msgbuf3, 256, "RB: %d\n", why4);
	//OutputDebugStringA(msgbuf);
	//OutputDebugStringA(msgbuf2);
	//OutputDebugStringA(msgbuf3);
	//Debug End
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	//Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	//spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	//Sprite::PostDraw();
	// 深度バッファクリア
	//dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3D描画

	// 3D Object Drawing
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S))
	{
		object1->Draw(cmdList);
	}
	else
	{
		object6->Draw(cmdList);
	}
	object2->Draw(cmdList);
	object3->Draw(cmdList);
	object4->Draw(cmdList);
	object5->Draw(cmdList);

	// パーティクルの描画
	particleMan->Draw(cmdList);
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>


	// デバッグテキストの描画
	debugText->DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}
