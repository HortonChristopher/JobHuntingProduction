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

extern XMFLOAT3 objectPosition = { 0.0f, -10.0f, 0.0f };
extern XMFLOAT3 objectRotation = { 0.0f, 0.0f, 0.0f };

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

	modelSkydome = Model::CreateFromOBJ("skydome");
	modelGround = Model::CreateFromOBJ("ground");
	modelAttackRange = Model::CreateFromOBJ("yuka");

	objSkydome->SetModel(modelSkydome);
	objGround->SetModel(modelGround);
	objAttackRange->SetModel(modelAttackRange);

	// Specify the FBX model and read the file
	//model1 = FbxLoader::GetInstance()->LoadModelFromFile("PlayerRunning");
	model1 = FbxLoader::GetInstance()->LoadModelFromFile("ProtoRunning");
	//model2 = FbxLoader::GetInstance()->LoadModelFromFile("PlayerStanding");
	model2 = FbxLoader::GetInstance()->LoadModelFromFile("ProtoStanding");
	model3 = FbxLoader::GetInstance()->LoadModelFromFile("ProtoAttack");

	object1 = new Player;
	object1->Initialize();
	object1->SetModel(model1);

	object2 = new EnemyHuman;
	object2->Initialize();
	object2->SetModel(model2);

	object3 = new EnemyHuman;
	object3->Initialize();
	object3->SetModel(model2);

	object4 = new EnemyHuman;
	object4->Initialize();
	object4->SetModel(model2);

	object5 = new EnemyHuman;
	object5->Initialize();
	object5->SetModel(model2);

	object6 = new Player;
	object6->Initialize();
	object6->SetModel(model2);

	object7 = new Player;
	object7->Initialize();
	object7->SetModel(model3);

	object8 = new EnemyHuman;
	object8->Initialize();
	object8->SetModel(model1);

	object9 = new EnemyHuman;
	object9->Initialize();
	object9->SetModel(model1);

	object10 = new EnemyHuman;
	object10->Initialize();
	object10->SetModel(model1);

	object11 = new EnemyHuman;
	object11->Initialize();
	object11->SetModel(model1);

	// テクスチャ2番に読み込み
	Sprite::LoadTexture(2, L"Resources/tex1.png");

	// ライト生成
	lightGroup = LightGroup::Create();

	// カメラ注視点をセット
	camera->SetTarget({0, 0.0f, 0});
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(64.0f);

	object1->SetPosition(objectPosition);
	object1->SetRotation(objectRotation);
	object1->SetScale({ 3, 3, 3 });
	object6->SetPosition(objectPosition);
	object6->SetRotation(objectRotation);
	object6->SetScale({ 3, 3, 3 });
	object7->SetPosition(objectPosition);
	object7->SetRotation(objectRotation);
	object7->SetScale({ 3, 3, 3 });

	objAttackRange->SetPosition({ objectPosition.x, objectPosition.y + 0.5f, objectPosition.z + 5.0f });
	objAttackRange->SetRotation({0, 0, 0});
	objAttackRange->SetScale({ 15, 15, 15 });

	object2->SetPosition(enemy1Position);
	object2->SetRotation({ 0, 180, 0 });
	object2->SetScale({ 3, 3, 3 });
	object8->SetPosition(enemy1Position);
	object8->SetRotation({ 0, 180, 0 });
	object8->SetScale({ 3, 3, 3 });

	object3->SetPosition(enemy2Position);
	object3->SetRotation({ 0, 90, 0 });
	object3->SetScale({ 3, 3, 3 });
	object9->SetPosition(enemy2Position);
	object9->SetRotation({ 0, 90, 0 });
	object9->SetScale({ 3, 3, 3 });

	object4->SetPosition(enemy3Position);
	object4->SetRotation({ 0, 0, 0 });
	object4->SetScale({ 3, 3, 3 });
	object10->SetPosition(enemy3Position);
	object10->SetRotation({ 0, 0, 0 });
	object10->SetScale({ 3, 3, 3 });
	
	object5->SetPosition(enemy4Position);
	object5->SetRotation({ 0, 270, 0 });
	object5->SetScale({ 3, 3, 3 });
	object11->SetPosition(enemy4Position);
	object11->SetRotation({ 0, 270, 0 });
	object11->SetScale({ 3, 3, 3 });

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

		// 移動ベクトルをY軸周りの角度で回転
		XMVECTOR move = { 0,0,1.0f,0 };
		XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(objectRotation.y));
		move = XMVector3TransformNormal(move, matRot);

		// 向いている方向に移動
		if (input->PushKey(DIK_S) || input->PushLStickDown()) {
			objectPosition.x -= move.m128_f32[0];
			objectPosition.y -= move.m128_f32[1];
			objectPosition.z -= move.m128_f32[2];
		}
		else if (input->PushKey(DIK_W) || input->PushLStickUp()) {
			objectPosition.x += move.m128_f32[0];
			objectPosition.y += move.m128_f32[1];
			objectPosition.z += move.m128_f32[2];
		}

		if (!wander1 && timer1 < 300)
		{
			timer1++;
		}
		else if (!wander1 && timer1 > 299)
		{
			movement1Position.x = rand() % 201 - 100;
			movement1Position.z = rand() % 201 + 100;
			timer1 = 0;
			wander1 = true;
		}

		if (!wander2 && timer2 < 300)
		{
			timer2++;
		}
		else if (!wander2 && timer2 > 299)
		{
			movement2Position.x = rand() % 201 + 100;
			movement2Position.z = rand() % 201 - 100;
			timer2 = 0;
			wander2 = true;
		}

		if (!wander3 && timer3 < 300)
		{
			timer3++;
		}
		else if (!wander3 && timer3 > 299)
		{
			movement3Position.x = rand() % 201 - 100;
			movement3Position.z = rand() % 201 - 200;
			timer3 = 0;
			wander3 = true;
		}

		if (!wander4 && timer4 < 300)
		{
			timer4++;
		}
		else if (!wander4 && timer4 > 299)
		{
			movement4Position.x = rand() % 201 - 100;
			movement4Position.z = rand() % 201 - 200;
			timer4 = 0;
			wander4 = true;
		}

		if (wander1 && !set1)
		{
			x1 = (movement1Position.x - enemy1Position.x) / 600.0f;
			y1 = (movement1Position.z - enemy1Position.z) / 600.0f;
			float x = movement1Position.x - enemy1Position.x;
			float y = movement1Position.z - enemy1Position.z;
			float theta_radians = atan2(y, x);
			float theta_degrees = XMConvertToDegrees(theta_radians);
			theta_degrees1 = theta_degrees;
			set1 = true;
		}
		else if (wander1 && set1)
		{
			if (timer1 > 600)
			{
				timer1 = 0;
				wander1 = false;
				set1 = false;
			}
			else
			{
				timer1++;
			}

			enemy1Position.x += x1;
			enemy1Position.z += y1;
			object2->SetRotation({ object2->GetRotation().x, -theta_degrees1 + 90.0f, object2->GetRotation().z });
			object8->SetRotation({ object8->GetRotation().x, -theta_degrees1 + 90.0f, object8->GetRotation().z });
			object2->SetPosition(enemy1Position);
			object8->SetPosition(enemy1Position);
		}

		if (wander2 && !set2)
		{
			x2 = (movement2Position.x - enemy2Position.x) / 600.0f;
			y2 = (movement2Position.z - enemy2Position.z) / 600.0f;
			float x2 = movement2Position.x - enemy2Position.x;
			float y2 = movement2Position.z - enemy2Position.z;
			float theta_radians2 = atan2(y2, x2);
			float theta_degrees2 = XMConvertToDegrees(theta_radians2);
			theta_degrees12 = theta_degrees2;
			set2 = true;
		}
		else if (wander2 && set2)
		{
			if (timer2 > 600)
			{
				timer2 = 0;
				wander2 = false;
				set2 = false;
			}
			else
			{
				timer2++;
			}

			enemy2Position.x += x2;
			enemy2Position.z += y2;
			object3->SetRotation({ object3->GetRotation().x, -theta_degrees12 + 90.0f, object3->GetRotation().z });
			object9->SetRotation({ object9->GetRotation().x, -theta_degrees12 + 90.0f, object9->GetRotation().z });
			object3->SetPosition(enemy2Position);
			object9->SetPosition(enemy2Position);
		}

		if (wander3 && !set3)
		{
			x3 = (movement3Position.x - enemy3Position.x) / 600.0f;
			y3 = (movement3Position.z - enemy3Position.z) / 600.0f;
			float x3 = movement3Position.x - enemy3Position.x;
			float y3 = movement3Position.z - enemy3Position.z;
			float theta_radians3 = atan2(y3, x3);
			float theta_degrees3 = XMConvertToDegrees(theta_radians3);
			theta_degrees123 = theta_degrees3;
			set3 = true;
		}
		else if (wander3 && set3)
		{
			if (timer3 > 600)
			{
				timer3 = 0;
				wander3 = false;
				set3 = false;
			}
			else
			{
				timer3++;
			}

			enemy3Position.x += x3;
			enemy3Position.z += y3;
			object4->SetRotation({ object4->GetRotation().x, -theta_degrees123 + 90.0f, object4->GetRotation().z });
			object10->SetRotation({ object10->GetRotation().x, -theta_degrees123 + 90.0f, object10->GetRotation().z });
			object4->SetPosition(enemy3Position);
			object10->SetPosition(enemy3Position);
		}

		if (wander4 && !set4)
		{
			x4 = (movement4Position.x - enemy4Position.x) / 600.0f;
			y4 = (movement4Position.z - enemy4Position.z) / 600.0f;
			float x4 = movement4Position.x - enemy4Position.x;
			float y4 = movement4Position.z - enemy4Position.z;
			float theta_radians4 = atan2(y4, x4);
			float theta_degrees4 = XMConvertToDegrees(theta_radians4);
			theta_degrees1234 = theta_degrees4;
			set4 = true;
		}
		else if (wander4 && set4)
		{
			if (timer4 > 600)
			{
				timer4 = 0;
				wander4 = false;
				set4 = false;
			}
			else
			{
				timer4++;
			}

			enemy4Position.x += x4;
			enemy4Position.z += y4;
			object5->SetRotation({ object5->GetRotation().x, -theta_degrees1234 + 90.0f, object5->GetRotation().z });
			object11->SetRotation({ object11->GetRotation().x, -theta_degrees1234 + 90.0f, object11->GetRotation().z });
			object5->SetPosition(enemy4Position);
			object11->SetPosition(enemy4Position);
		}

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

		object1->SetPosition(objectPosition);
		object1->SetRotation(objectRotation);
		object6->SetPosition(objectPosition);
		object6->SetRotation(objectRotation);
		object7->SetPosition(objectPosition);
		object7->SetRotation(objectRotation);
		objSkydome->SetPosition(objectPosition);
		objAttackRange->SetPosition({ (objectPosition.x + (sinf(XMConvertToRadians(objectRotation.y)) * 5)), objectPosition.y + 0.5f, (objectPosition.z + (cosf(XMConvertToRadians(objectRotation.y)) * 5)) });
		objAttackRange->SetRotation(objectRotation);

		if (attackTime > 10 && attackTime < 20)
		{
			if (intersect(objAttackRange->GetPosition(), object2->GetPosition(), 3.0f, 7.5f, 7.5f) && enemy1Alive == true)
			{
				enemy1Alive = false;
				e1Respawn = 30;
			}

			if (intersect(objAttackRange->GetPosition(), object3->GetPosition(), 3.0f, 7.5f, 7.5f) && enemy2Alive == true)
			{
				enemy2Alive = false;
				e2Respawn = 30;
			}

			if (intersect(objAttackRange->GetPosition(), object4->GetPosition(), 3.0f, 7.5f, 7.5f) && enemy3Alive == true)
			{
				enemy3Alive = false;
				e3Respawn = 30;
			}

			if (intersect(objAttackRange->GetPosition(), object5->GetPosition(), 3.0f, 7.5f, 7.5f) && enemy4Alive == true)
			{
				enemy4Alive = false;
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

		/*if (e1Respawn <= 0 && !enemy1Alive)
		{
			object2->SetPosition({ rand() % 201 - 100.0f, -10, rand() % 201 - 100.0f });
			enemy1Alive = true;
		}

		if (e2Respawn <= 0 && !enemy2Alive)
		{
			object3->SetPosition({ rand() % 201 - 100.0f, -10, rand() % 201 - 100.0f });
			enemy2Alive = true;
		}

		if (e3Respawn <= 0 && !enemy3Alive)
		{
			object4->SetPosition({ rand() % 201 - 100.0f, -10, rand() % 201 - 100.0f });
			enemy3Alive = true;
		}

		if (e4Respawn <= 0 && !enemy4Alive)
		{
			object5->SetPosition({ rand() % 201 - 100.0f, -10, rand() % 201 - 100.0f });
			enemy4Alive = true;
		}*/

		object1->Update();
		if (enemy1Alive)
		{
			object2->Update();
			object8->Update();
		}
		if (enemy2Alive)
		{
			object3->Update();
			object9->Update();
		}
		if (enemy3Alive)
		{
			object4->Update();
			object10->Update();
		}
		if (enemy4Alive)
		{
			object5->Update();
			object11->Update();
		}
		object6->Update();
		if (attackTime > 0)
		{
			object7->Update();
		}

		objSkydome->Update();
		objGround->Update();
		objAttackRange->Update();
	}

	//Debug Start
	//char msgbuf[256];
	//char msgbuf2[256];
	//char msgbuf3[256];

	//sprintf_s(msgbuf, 256, "X: %f\n", theta_degrees_debug);
	//sprintf_s(msgbuf2, 256, "Y: %f\n", enemy1Position.z);
	//sprintf_s(msgbuf3, 256, "Z: %f\n", timer1);
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
		if (wander1 && set1)
		{
			object8->Draw(cmdList);
		}
		else
		{
			object2->Draw(cmdList);
		}
	}
	if (enemy2Alive)
	{
		if (wander2 && set2)
		{
			object9->Draw(cmdList);
		}
		else
		{
			object3->Draw(cmdList);
		}
	}
	if (enemy3Alive)
	{
		if (wander3 && set3)
		{
			object10->Draw(cmdList);
		}
		else
		{
			object4->Draw(cmdList);
		}
	}
	if (enemy4Alive)
	{
		if (wander4 && set4)
		{
			object11->Draw(cmdList);
		}
		else
		{
			object5->Draw(cmdList);
		}
	}

	objSkydome->Draw();
	objGround->Draw();

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
