#include "GameScene.h"
#include "FBXGeneration.h"
#include "EnemyHuman.h"
#include "Player.h"
#include "FbxLoader/FbxLoader.h"
#include "DeltaTime.h"

#include <cassert>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <time.h>


using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;

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

	if (!Sprite::LoadTexture(5, L"Resources/P3.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(6, L"Resources/P4.png")) {
		assert(0);
		return;
	}

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	p1 = Sprite::Create(3, { 0.0f, 0.0f });
	p2 = Sprite::Create(4, { 0.0f, 0.0f });
	p3 = Sprite::Create(5, { 0.0f, 0.0f });
	p4 = Sprite::Create(6, { 0.0f, 0.0f });
	// パーティクルマネージャ生成
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	objSkydome = Object3d::Create();
	objGround = Object3d::Create();
	objAttackRange = Object3d::Create();
	objEAttackRange1 = Object3d::Create();
	objEAttackRange2 = Object3d::Create();
	objEAttackRange3 = Object3d::Create();
	objEAttackRange4 = Object3d::Create();
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
	modelVisionRange = Model::CreateFromOBJ("yuka");
	modelWall = Model::CreateFromOBJ("kabe");
	modelDoor = Model::CreateFromOBJ("DoorBase");

	objSkydome->SetModel(modelSkydome);
	objGround->SetModel(modelGround);
	objAttackRange->SetModel(modelAttackRange);
	objEAttackRange1->SetModel(modelAttackRange);
	objEAttackRange2->SetModel(modelAttackRange);
	objEAttackRange3->SetModel(modelAttackRange);
	objEAttackRange4->SetModel(modelAttackRange);
	objNorthWall1->SetModel(modelWall);
	objNorthWall2->SetModel(modelWall);
	objEastWall->SetModel(modelWall);
	objWestWall->SetModel(modelWall);
	objSouthWall->SetModel(modelWall);
	objDoor1->SetModel(modelDoor);

	objVisionRange1->SetModel(modelVisionRange);
	objVisionRange2->SetModel(modelVisionRange);
	objVisionRange3->SetModel(modelVisionRange);
	objVisionRange4->SetModel(modelVisionRange);

	// Specify the FBX model and read the file
	model1 = FbxLoader::GetInstance()->LoadModelFromFile("ProtoRunning");
	model2 = FbxLoader::GetInstance()->LoadModelFromFile("ProtoStanding");
	model3 = FbxLoader::GetInstance()->LoadModelFromFile("ProtoAttack");

	//deltaTime = new DeltaTime;

	object1 = new Player;
	object1->Initialize();
	//object1->SetModel(model1);

	object2 = new EnemyHuman;
	object2->Initialize();
	object2->SetPosition({ -300.0f, -10.0f, 0.0f });
	object2->SetHomePosition({ -300.0f, 0.0f });

	object3 = new EnemyHuman;
	object3->Initialize();
	object3->SetPosition({ 300.0f, -10.0f, 0.0f });
	object3->SetHomePosition({ 300.0f, 0.0f });

	object4 = new EnemyHuman;
	object4->Initialize();
	object4->SetPosition({ 0.0f, -10.0f, -300.0f });
	object4->SetHomePosition({ 0.0f, -300.0f });

	object5 = new EnemyHuman;
	object5->Initialize();
	object5->SetPosition({ 0.0f, -10.0f, 300.0f });
	object5->SetHomePosition({ 0.0f, 300.0f });

	object6 = new Player;
	object6->Initialize();
	object6->SetModel(model2);

	object7 = new Player;
	object7->Initialize();
	object7->SetModel(model3);

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
	objAttackRange->SetScale({ 15, 1, 15 });

	objEAttackRange1->SetPosition({ object2->GetPosition().x, object2->GetPosition().y + 0.5f, object2->GetPosition().z + 5.0f });
	objEAttackRange1->SetScale({ 15, 1, 15 });
	objEAttackRange2->SetPosition({ object3->GetPosition().x, object3->GetPosition().y + 0.5f, object3->GetPosition().z + 5.0f });
	objEAttackRange2->SetScale({ 15, 1, 15 }); 
	objEAttackRange3->SetPosition({ object4->GetPosition().x, object4->GetPosition().y + 0.5f, object4->GetPosition().z + 5.0f });
	objEAttackRange3->SetScale({ 15, 1, 15 }); 
	objEAttackRange4->SetPosition({ object5->GetPosition().x, object5->GetPosition().y + 0.5f, object5->GetPosition().z + 5.0f });
	objEAttackRange4->SetScale({ 15, 1, 15 });

	objVisionRange1->SetScale({ 90,1,90 });
	objVisionRange2->SetScale({ 90,1,90 });
	objVisionRange3->SetScale({ 90,1,90 });
	objVisionRange4->SetScale({ 90,1,90 });

	objSkydome->SetScale({ 5,5,5 });
	objGround->SetScale({ 100,0,100 });

	objGround->SetPosition({ 0, -10, 0 });

	srand(time(NULL));
}

void GameScene::Update()
{
	/*if (!FirstRun)
	{
		deltaTime->deltaTimeCalc();
	}
	else
	{
		FirstRun = false;
	}
	deltaTime->deltaTimePrev();*/
	lightGroup->Update();
	particleMan->Update();

	if (page < 2)
	{
		if (input->TriggerKey(DIK_SPACE) || input->TriggerControllerButton(XINPUT_GAMEPAD_A))
		{
			page++;
		}
	}

	if (page > 1 && page < 3)
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

		objectPosition = object1->GetPosition();
		objectRotation = object1->GetRotation();

		objSkydome->SetPosition(objectPosition);

		objAttackRange->SetPosition({ (objectPosition.x + (sinf(XMConvertToRadians(objectRotation.y)) * 15)), objectPosition.y + 0.5f, (objectPosition.z + (cosf(XMConvertToRadians(objectRotation.y)) * 15)) });
		objAttackRange->SetRotation(objectRotation);

		objEAttackRange1->SetPosition({ (object2->GetPosition().x + (sinf(XMConvertToRadians(object2->GetRotation().y)) * 5)), object2->GetPosition().y + 0.5f, (object2->GetPosition().z + (cosf(XMConvertToRadians(object2->GetRotation().y)) * 5)) });
		objEAttackRange1->SetRotation(object2->GetRotation());

		objEAttackRange2->SetPosition({ (object3->GetPosition().x + (sinf(XMConvertToRadians(object3->GetRotation().y)) * 5)), object3->GetPosition().y + 0.5f, (object3->GetPosition().z + (cosf(XMConvertToRadians(object3->GetRotation().y)) * 5)) });
		objEAttackRange2->SetRotation(object3->GetRotation());

		objEAttackRange3->SetPosition({ (object4->GetPosition().x + (sinf(XMConvertToRadians(object4->GetRotation().y)) * 5)), object4->GetPosition().y + 0.5f, (object4->GetPosition().z + (cosf(XMConvertToRadians(object4->GetRotation().y)) * 5)) });
		objEAttackRange3->SetRotation(object4->GetRotation());

		objEAttackRange4->SetPosition({ (object5->GetPosition().x + (sinf(XMConvertToRadians(object5->GetRotation().y)) * 5)), object5->GetPosition().y + 0.5f, (object5->GetPosition().z + (cosf(XMConvertToRadians(object5->GetRotation().y)) * 5)) });
		objEAttackRange4->SetRotation(object5->GetRotation());

		objVisionRange1->SetPosition({(object2->GetPosition().x + (sinf(XMConvertToRadians(object2->GetRotation().y)) * 40)), object2->GetPosition().y + 0.5f, (object2->GetPosition().z + (cosf(XMConvertToRadians(object2->GetRotation().y)) * 40))});
		objVisionRange1->SetRotation(object2->GetRotation());

		objVisionRange2->SetPosition({ (object3->GetPosition().x + (sinf(XMConvertToRadians(object3->GetRotation().y)) * 40)), object3->GetPosition().y + 0.5f, (object3->GetPosition().z + (cosf(XMConvertToRadians(object3->GetRotation().y)) * 40)) });
		objVisionRange2->SetRotation(object3->GetRotation());

		objVisionRange3->SetPosition({ (object4->GetPosition().x + (sinf(XMConvertToRadians(object4->GetRotation().y)) * 40)), object4->GetPosition().y + 0.5f, (object4->GetPosition().z + (cosf(XMConvertToRadians(object4->GetRotation().y)) * 40)) });
		objVisionRange3->SetRotation(object4->GetRotation());

		objVisionRange4->SetPosition({ (object5->GetPosition().x + (sinf(XMConvertToRadians(object5->GetRotation().y)) * 40)), object5->GetPosition().y + 0.5f, (object5->GetPosition().z + (cosf(XMConvertToRadians(object5->GetRotation().y)) * 40)) });
		objVisionRange4->SetRotation(object5->GetRotation());

		if (intersect(object1->GetPosition(), objVisionRange1->GetPosition(), 3.0f, 60.0f, 60.0f) && enemy1Alive == true)
		{
			object2->SetAggro(true);
			float distance1 = sqrt((object2->GetPosition().x - object1->GetPosition().x) * (object2->GetPosition().x - object1->GetPosition().x) + (object2->GetPosition().y - object1->GetPosition().y) * (object2->GetPosition().y - object1->GetPosition().y));
			if (distance1 < 3.0f)
			{
				object2->SetAttack(true);
			}
			else
			{
				object2->SetAttack(false);
			}
		}
		else
		{
			//object2->SetAggro(false);
			object2->SetAggroSwitch(true);
			object2->SetAttack(false);
		}
		if (intersect(object1->GetPosition(), objVisionRange2->GetPosition(), 3.0f, 60.0f, 60.0f) && enemy1Alive == true)
		{
			object3->SetAggro(true);
			float distance2 = sqrt((object3->GetPosition().x - object1->GetPosition().x) * (object3->GetPosition().x - object1->GetPosition().x) + (object3->GetPosition().y - object1->GetPosition().y) * (object3->GetPosition().y - object1->GetPosition().y));
			if (distance2 < 3.0f)
			{
				object3->SetAttack(true);
			}
			else
			{
				object3->SetAttack(false);
			}
		}
		else
		{
			//object3->SetAggro(false);
			object3->SetAggroSwitch(true);
			object3->SetAttack(false);
		}
		if (intersect(object1->GetPosition(), objVisionRange3->GetPosition(), 3.0f, 60.0f, 60.0f) && enemy1Alive == true)
		{
			object4->SetAggro(true);
			float distance3 = sqrt((object4->GetPosition().x - object1->GetPosition().x) * (object4->GetPosition().x - object1->GetPosition().x) + (object4->GetPosition().y - object1->GetPosition().y) * (object4->GetPosition().y - object1->GetPosition().y));
			if (distance3 < 3.0f)
			{
				object4->SetAttack(true);
			}
			else
			{
				object4->SetAttack(false);
			}
		}
		else
		{
			//object4->SetAggro(false);
			object4->SetAggroSwitch(true);
			object4->SetAttack(false);
		}
		if (intersect(object1->GetPosition(), objVisionRange4->GetPosition(), 3.0f, 60.0f, 60.0f) && enemy1Alive == true)
		{
			object5->SetAggro(true);
			float distance4 = sqrt((object5->GetPosition().x - object1->GetPosition().x) * (object5->GetPosition().x - object1->GetPosition().x) + (object5->GetPosition().y - object1->GetPosition().y) * (object5->GetPosition().y - object1->GetPosition().y));
			if (distance4 < 3.0f)
			{
				object5->SetAttack(true);
			}
			else
			{
				object5->SetAttack(false);
			}
		}
		else
		{
			//object5->SetAggro(false);
			object5->SetAggroSwitch(true);
			object5->SetAttack(false);
		}	

		if (intersect(objEAttackRange1->GetPosition(), object1->GetPosition(), 3.0f, 10.0f, 10.0f) && enemy1Alive == true)
		{
			page++;
		}
		if (intersect(objEAttackRange2->GetPosition(), object1->GetPosition(), 3.0f, 10.0f, 10.0f) && enemy2Alive == true)
		{
			page++;
		}
		if (intersect(objEAttackRange3->GetPosition(), object1->GetPosition(), 3.0f, 10.0f, 10.0f) && enemy3Alive == true)
		{
			page++;
		}
		if (intersect(objEAttackRange4->GetPosition(), object1->GetPosition(), 3.0f, 10.0f, 10.0f) && enemy4Alive == true)
		{
			page++;
		}

		if (attackTime > 10 && attackTime < 20)
		{
			if (intersect(objAttackRange->GetPosition(), object2->GetPosition(), 3.0f, 25.0f, 25.0f) && enemy1Alive == true)
			{
				enemy1Alive = false;
				enemyDefeated++;
				e1Respawn = 30;
			}

			if (intersect(objAttackRange->GetPosition(), object3->GetPosition(), 3.0f, 25.0f, 25.0f) && enemy2Alive == true)
			{
				enemy2Alive = false;
				enemyDefeated++;
				e2Respawn = 30;
			}

			if (intersect(objAttackRange->GetPosition(), object4->GetPosition(), 3.0f, 25.0f, 25.0f) && enemy3Alive == true)
			{
				enemy3Alive = false;
				enemyDefeated++;
				e3Respawn = 30;
			}

			if (intersect(objAttackRange->GetPosition(), object5->GetPosition(), 3.0f, 25.0f, 25.0f) && enemy4Alive == true)
			{
				enemy4Alive = false;
				enemyDefeated++;
				e4Respawn = 30;
			}
		}

		if (enemyDefeated > 9)
		{
			page += 2;
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
			object2->SetPosition({ -300.0f, -10.0f, 0.0f });
			enemy1Alive = true;
		}

		if (e2Respawn <= 0 && !enemy2Alive)
		{
			object3->SetPosition({ 300.0f, -10.0f, 0.0f });
			enemy2Alive = true;
		}

		if (e3Respawn <= 0 && !enemy3Alive)
		{
			object4->SetPosition({ 0.0f, -10.0f, -300.0f });
			enemy3Alive = true;
		}

		if (e4Respawn <= 0 && !enemy4Alive)
		{
			object5->SetPosition({ 0.0f, -10.0f, 300.0f });
			enemy4Alive = true;
		}

		/*if (enemyDefeated > 5 && !area1Clear)
		{
			gateOpen = true;
			enemyDefeated = 0;
			area1Clear = true;
		}*/

		/*if (gateOpen)
		{
			objDoor1->SetPosition({ objDoor1->GetPosition().x, objDoor1->GetPosition().y + 0.1f, objDoor1->GetPosition().z });
			objDoor1->Update();

			if (objDoor1->GetPosition().y > 20.0f)
			{
				gateOpen = false;
			}
		}*/

		object1->Update();
		if (enemy1Alive)
		{
			object2->Update();
			objVisionRange1->Update();
			objEAttackRange1->Update();
		}
		if (enemy2Alive)
		{
			object3->Update();
			objVisionRange2->Update();
			objEAttackRange2->Update();
		}
		if (enemy3Alive)
		{
			object4->Update();
			objVisionRange3->Update();
			objEAttackRange3->Update();
		}
		if (enemy4Alive)
		{
			object5->Update();
			objVisionRange4->Update();
			objEAttackRange4->Update();
		}
		object6->Update();
		object7->Update();

		objAttackRange->Update();
		/*objNorthWall1->Update();
		objNorthWall2->Update();
		objEastWall->Update();
		objWestWall->Update();
		objSouthWall->Update();*/
		objSkydome->Update();
		objGround->Update();
		//objDoor1->Update();
	}

	if (page > 2)
	{

	}

	//float distance = sqrt((object2->GetPosition().x - object1->GetPosition().x) * (object2->GetPosition().x - object1->GetPosition().x) + (object2->GetPosition().y - object1->GetPosition().y) * (object2->GetPosition().y - object1->GetPosition().y));

	//deltaTime->deltaTimeNow();

	//Debug Start
	char msgbuf[256];
	//char msgbuf2[256];
	//char msgbuf3[256];

	sprintf_s(msgbuf, 256, "X: %f\n", deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	//sprintf_s(msgbuf2, 256, "Y: %f\n", );
	//sprintf_s(msgbuf3, 256, "Z: %f\n", objectPosition.z);
	OutputDebugStringA(msgbuf);
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
	/*if (attackTime > 0)
	{
		object7->Draw(cmdList);	
	}
	else if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushLStickUp() || input->PushLStickDown())
	{
		object1->Draw(cmdList);
	}
	else
	{
		object6->Draw(cmdList);
	}*/
	object1->Draw(cmdList);
	if (attackTime > 10 && attackTime < 20)
	{
		objAttackRange->Draw();
	}

	if (enemy1Alive && page == 2)
	{
		object2->Draw(cmdList);
		//objVisionRange1->Draw();
		//objEAttackRange1->Draw();
	}
	if (enemy2Alive && page == 2)
	{
		object3->Draw(cmdList);
		//objVisionRange2->Draw();
		//objEAttackRange2->Draw();
	}
	if (enemy3Alive && page == 2)
	{
		object4->Draw(cmdList);
		//objVisionRange3->Draw();
		//objEAttackRange3->Draw();
	}
	if (enemy4Alive && page == 2)
	{
		object5->Draw(cmdList);
		//objVisionRange4->Draw();
		//objEAttackRange4->Draw();
	}

	objSkydome->Draw();
	objGround->Draw();
	/*objNorthWall1->Draw();
	objNorthWall2->Draw();
	objEastWall->Draw();
	objWestWall->Draw();
	objSouthWall->Draw();
	objDoor1->Draw();*/

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
	if (page == 3)
	{
		p3->Draw();
	}
	if (page == 4)
	{
		p4->Draw();
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
