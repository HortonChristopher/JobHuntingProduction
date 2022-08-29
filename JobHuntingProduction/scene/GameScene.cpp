#include "GameScene.h"
#include "FBXGeneration.h"
#include "EnemyHuman.h"
#include "Player.h"
#include "FbxLoader/FbxLoader.h"

#include <cassert>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <time.h>

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

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

	//collisionManager = CollisionManager::GetInstance();

	// Device set
	FBXGeneration::SetDevice(dxCommon->GetDevice());
	EnemyHuman::SetDevice(dxCommon->GetDevice());
	Player::SetDevice(dxCommon->GetDevice());
	// Camera set
	Object3d::SetCamera(camera);
	FBXGeneration::SetCamera(camera);
	EnemyHuman::SetCamera(camera);
	Player::SetCamera(camera);

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

	if (!Sprite::LoadTexture(3, L"Resources/P1.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(4, L"Resources/P2.png")) {
		assert(0);
		return;
	}

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	p1 = Sprite::Create(3, { 0.0f, 0.0f });
	p2 = Sprite::Create(4, { 0.0f, 0.0f });
	// パーティクルマネージャ生成
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	objSkydome = Object3d::Create();
	objGround = Object3d::Create();
	objAttackRange = Object3d::Create();
	objNorthWall1 = Object3d::Create();
	objNorthWall2 = Object3d::Create();
	objEastWall = Object3d::Create();
	objWestWall = Object3d::Create();
	objSouthWall = Object3d::Create();
	objDoor1 = Object3d::Create();

	objVisionRange1 = Object3d::Create();
	objVisionRange2 = Object3d::Create();
	objVisionRange3 = Object3d::Create();
	objVisionRange4 = Object3d::Create();

	modelSkydome = Model::CreateFromOBJ("skydome");
	modelGround = Model::CreateFromOBJ("ground");
	modelAttackRange = Model::CreateFromOBJ("yuka");
	modelWall = Model::CreateFromOBJ("kabe");
	modelDoor = Model::CreateFromOBJ("DoorBase");

	objSkydome->SetModel(modelSkydome);
	objGround->SetModel(modelGround);
	objAttackRange->SetModel(modelAttackRange);
	objNorthWall1->SetModel(modelWall);
	objNorthWall2->SetModel(modelWall);
	objEastWall->SetModel(modelWall);
	objWestWall->SetModel(modelWall);
	objSouthWall->SetModel(modelWall);
	objDoor1->SetModel(modelDoor);

	// Specify the FBX model and read the file
	model1 = FbxLoader::GetInstance()->LoadModelFromFile("ProtoRunning");
	model2 = FbxLoader::GetInstance()->LoadModelFromFile("ProtoStanding");
	model3 = FbxLoader::GetInstance()->LoadModelFromFile("ProtoAttack");

	object1 = new Player;
	object1->Initialize();
	object1->SetModel(model1);

	object2 = new EnemyHuman;
	object2->Initialize();

	object3 = new EnemyHuman;
	object3->Initialize();

	object4 = new EnemyHuman;
	object4->Initialize();

	object5 = new EnemyHuman;
	object5->Initialize();

	object6 = new Player;
	object6->Initialize();
	object6->SetModel(model2);

	object7 = new Player;
	object7->Initialize();
	object7->SetModel(model3);

	object8 = new EnemyHuman;
	object8->Initialize();

	object9 = new EnemyHuman;
	object9->Initialize();

	object10 = new EnemyHuman;
	object10->Initialize();

	object11 = new EnemyHuman;
	object11->Initialize();

	// テクスチャ2番に読み込み
	Sprite::LoadTexture(2, L"Resources/tex1.png");

	// ライト生成
	lightGroup = LightGroup::Create();

	// カメラ注視点をセット
	camera->SetTarget({0, 0.0f, -30.0f});
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(48.0f);

	objNorthWall1->SetPosition({ -90.0f, 35.0f, 160.0f });
	objNorthWall2->SetPosition({ 90.0f, 35.0f, 160.0f });
	objNorthWall1->SetScale({ 140.0f, 60.0f, 3.0f });
	objNorthWall2->SetScale({ 140.0f, 60.0f, 3.0f });
	
	objEastWall->SetPosition({ 160.0f, 35.0f, 0.0f });
	objEastWall->SetScale({ 3.0f, 60.0f, 320.0f });
	objWestWall->SetPosition({ -160.0f, 35.0f, 0.0f });
	objWestWall->SetScale({ 3.0f, 60.0f, 320.0f });

	objSouthWall->SetPosition({ 0.0f, 35.0f, -160.0f });
	objSouthWall->SetScale({ 320.0f, 60.0f, 3.0f });

	objDoor1->SetPosition({ 0.0f, -5.0f, 160.0f });
	objDoor1->SetScale({ 40.0f, 10.0f, 3.0f });

	objAttackRange->SetPosition({ objectPosition.x, objectPosition.y + 0.5f, objectPosition.z + 5.0f });
	objAttackRange->SetRotation({0, 0, 0});
	objAttackRange->SetScale({ 15, 15, 15 });

	objSkydome->SetScale({ 5,5,5 });
	objGround->SetScale({ 100,0,100 });

	objGround->SetPosition({ 0, -10, 0 });

	srand(time(NULL));
}

void GameScene::Update()
{
	lightGroup->Update();
	particleMan->Update();

	if (page < 2)
	{
		if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
		{
			page++;
		}
	}

	if (page > 1)
	{
		camera->Update();

		// A,Dで旋回
		/*if (input->PushKey(DIK_A)) {
			objectRotation.y -= 2.0f;
		}
		else if (input->PushKey(DIK_D)) {
			objectRotation.y += 2.0f;
		}*/

		if (input->TriggerKey(DIK_SPACE) && attackTime == 0 || input->TriggerControllerButton(XINPUT_GAMEPAD_A) && attackTime == 0)
		{
			attackTime = 30;
		}

		if (attackTime > 0)
		{
			attackTime--;
		}
		else
		{
			attackTime = 0;
		}

		objSkydome->SetPosition(objectPosition);

		objAttackRange->SetPosition({ (objectPosition.x + (sinf(XMConvertToRadians(objectRotation.y)) * 5)), objectPosition.y + 0.5f, (objectPosition.z + (cosf(XMConvertToRadians(objectRotation.y)) * 5)) });
		objAttackRange->SetRotation(objectRotation);

		if (attackTime > 10 && attackTime < 20)
		{
			if (intersect(objAttackRange->GetPosition(), object2->GetPosition(), 3.0f, 7.5f, 7.5f) && enemy1Alive == true)
			{
				enemy1Alive = false;
				if (!area1Clear)
				{
					enemyDefeated++;
				}
				e1Respawn = 30;
			}

			if (intersect(objAttackRange->GetPosition(), object3->GetPosition(), 3.0f, 7.5f, 7.5f) && enemy2Alive == true)
			{
				enemy2Alive = false;
				if (!area1Clear)
				{
					enemyDefeated++;
				}
				e2Respawn = 30;
			}

			if (intersect(objAttackRange->GetPosition(), object4->GetPosition(), 3.0f, 7.5f, 7.5f) && enemy3Alive == true)
			{
				enemy3Alive = false;
				if (!area1Clear)
				{
					enemyDefeated++;
				}
				e3Respawn = 30;
			}

			if (intersect(objAttackRange->GetPosition(), object5->GetPosition(), 3.0f, 7.5f, 7.5f) && enemy4Alive == true)
			{
				enemy4Alive = false;
				if (!area1Clear)
				{
					enemyDefeated++;
				}
				e4Respawn = 30;
			}
		}

		if (e1Respawn > 0)
		{
			e1Respawn--;
		}
		else
		{
			e1Respawn = 0;
		}

		if (e2Respawn > 0)
		{
			e2Respawn--;
		}
		else
		{
			e2Respawn = 0;
		}

		if (e3Respawn > 0)
		{
			e3Respawn--;
		}
		else
		{
			e3Respawn = 0;
		}

		if (e4Respawn > 0)
		{
			e4Respawn--;
		}
		else
		{
			e4Respawn = 0;
		}

		if (e1Respawn <= 0 && !enemy1Alive)
		{
			object2->SetPosition({ rand() % 301 - 150.0f, -10, rand() % 301 - 150.0f });
			enemy1Alive = true;
		}

		if (e2Respawn <= 0 && !enemy2Alive)
		{
			object3->SetPosition({ rand() % 301 - 150.0f, -10, rand() % 301 - 150.0f });
			enemy2Alive = true;
		}

		if (e3Respawn <= 0 && !enemy3Alive)
		{
			object4->SetPosition({ rand() % 301 - 150.0f, -10, rand() % 301 - 150.0f });
			enemy3Alive = true;
		}

		if (e4Respawn <= 0 && !enemy4Alive)
		{
			object5->SetPosition({ rand() % 301 - 150.0f, -10, rand() % 301 - 150.0f });
			enemy4Alive = true;
		}

		if (enemyDefeated > 5 && !area1Clear)
		{
			gateOpen = true;
			enemyDefeated = 0;
			area1Clear = true;
		}

		if (gateOpen)
		{
			objDoor1->SetPosition({ objDoor1->GetPosition().x, objDoor1->GetPosition().y + 0.1f, objDoor1->GetPosition().z });
			objDoor1->Update();

			if (objDoor1->GetPosition().y > 20.0f)
			{
				gateOpen = false;
			}
		}
		object1->Update();
		if (enemy1Alive)
		{
			object2->Update();
		}
		if (enemy2Alive)
		{
			object3->Update();
		}
		if (enemy3Alive)
		{
			object4->Update();
		}
		if (enemy4Alive)
		{
			object5->Update();
		}
		object6->Update();
		object7->Update();

		objAttackRange->Update();
		objNorthWall1->Update();
		objNorthWall2->Update();
		objEastWall->Update();
		objWestWall->Update();
		objSouthWall->Update();
		objSkydome->Update();
		objGround->Update();
		objDoor1->Update();
	}

	//Debug Start
	//char msgbuf[256];
	//char msgbuf2[256];
	//char msgbuf3[256];

	//sprintf_s(msgbuf, 256, "X: %f\n", objectPosition.x);
	//sprintf_s(msgbuf2, 256, "Y: %f\n", objectPosition.y);
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
	Object3d::PreDraw(cmdList);

	// 3D Object Drawing
	if (attackTime > 0)
	{
		object7->Draw(cmdList);
		if (attackTime > 10 && attackTime < 20)
		{
			objAttackRange->Draw();
		}
	}
	else if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushLStickUp() || input->PushLStickDown())
	{
		object1->Draw(cmdList);
	}
	else
	{
		object6->Draw(cmdList);
	}
	if (enemy1Alive)
	{
		object2->Draw(cmdList);
	}
	if (enemy2Alive)
	{
		object3->Draw(cmdList);
	}
	if (enemy3Alive)
	{
		object4->Draw(cmdList);
	}
	if (enemy4Alive)
	{
		object5->Draw(cmdList);
	}

	objSkydome->Draw();
	objGround->Draw();
	objNorthWall1->Draw();
	objNorthWall2->Draw();
	objEastWall->Draw();
	objWestWall->Draw();
	objSouthWall->Draw();
	objDoor1->Draw();

	// パーティクルの描画
	particleMan->Draw(cmdList);

	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	if (page == 0)
	{
		p1->Draw();
	}
	if (page == 1)
	{
		p2->Draw();
	}

	// デバッグテキストの描画
	debugText->DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

int GameScene::intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH)
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
