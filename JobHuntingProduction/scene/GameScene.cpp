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
float height = 0.0f;

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

	collisionManager = CollisionManager::GetInstance();

	// Device set
	FBXGeneration::SetDevice(dxCommon->GetDevice());
	EnemyHuman::SetDevice(dxCommon->GetDevice());
	Player::SetDevice(dxCommon->GetDevice());
	// Camera set
	Object3d::SetCamera(camera);
	FBXGeneration::SetCamera(camera);
	EnemyHuman::SetCamera(camera);
	Player::SetCamera(camera);
	PlayerPositionObject::SetCamera(camera);

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

	if (!Sprite::LoadTexture(7, L"Resources/HPBar.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(8, L"Resources/HPBarFrame.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(9, L"Resources/STBar.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(10, L"Resources/STBarFrame.png")) {
		assert(0);
		return;
	}

	if (!Sprite::LoadTexture(11, L"Resources/Mission1.png")) {
		assert(0);
		return;
	}

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	p1 = Sprite::Create(3, { 0.0f, 0.0f });
	p2 = Sprite::Create(4, { 0.0f, 0.0f });
	p3 = Sprite::Create(5, { 0.0f, 0.0f });
	p4 = Sprite::Create(6, { 0.0f, 0.0f });
	HPBar = Sprite::Create(7, { 25.0f, 25.0f });
	HPBarFrame = Sprite::Create(8, { 25.0f, 25.0f });
	STBar = Sprite::Create(9, { 25.0f, 55.0f });
	STBarFrame = Sprite::Create(10, { 25.0f, 55.0f });
	mission1 = Sprite::Create(11, { 1150.0f, 100.0f });
	mission1->SetSize({ 100.0f, 80.0f });
	// パーティクルマネージャ生成
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	objSkydome = Object3d::Create();
	objAttackRange = Object3d::Create();
	objEAttackRange1 = Object3d::Create();
	objEAttackRange2 = Object3d::Create();
	objEAttackRange3 = Object3d::Create();
	objEAttackRange4 = Object3d::Create();
	playerPositionObject = PlayerPositionObject::Create();
	enemyPositionObject1 = PlayerPositionObject::Create();
	enemyPositionObject2 = PlayerPositionObject::Create();
	enemyPositionObject3 = PlayerPositionObject::Create();
	enemyPositionObject4 = PlayerPositionObject::Create();

	objVisionRange1 = Object3d::Create();
	objVisionRange2 = Object3d::Create();
	objVisionRange3 = Object3d::Create();
	objVisionRange4 = Object3d::Create();

	modelSkydome = Model::CreateFromOBJ("skydome");
	//modelGround = Model::CreateFromOBJ("ProtoLandscape");
	modelGround = Model::CreateFromOBJ("Landscape2");
	modelExtendedGround = Model::CreateFromOBJ("ground");
	modelAttackRange = Model::CreateFromOBJ("yuka");
	modelVisionRange = Model::CreateFromOBJ("yuka");

	objSkydome->SetModel(modelSkydome);
	objGround = TouchableObject::Create(modelGround);
	objExtendedGround = TouchableObject::Create(modelExtendedGround);
	objAttackRange->SetModel(modelAttackRange);
	objEAttackRange1->SetModel(modelAttackRange);
	objEAttackRange2->SetModel(modelAttackRange);
	objEAttackRange3->SetModel(modelAttackRange);
	objEAttackRange4->SetModel(modelAttackRange);
	playerPositionObject->SetModel(modelAttackRange);
	enemyPositionObject1->SetModel(modelAttackRange);
	enemyPositionObject2->SetModel(modelAttackRange);
	enemyPositionObject3->SetModel(modelAttackRange);
	enemyPositionObject4->SetModel(modelAttackRange);

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

	object2 = new EnemyHuman;
	object2->Initialize();
	object2->SetPosition({ -300.0f, 50.0f, 0.0f });
	object2->SetHomePosition({ -300.0f, 0.0f });

	object3 = new EnemyHuman;
	object3->Initialize();
	object3->SetPosition({ 300.0f, 50.0f, 0.0f });
	object3->SetHomePosition({ 300.0f, 0.0f });

	object4 = new EnemyHuman;
	object4->Initialize();
	object4->SetPosition({ 0.0f, 50.0f, -300.0f });
	object4->SetHomePosition({ 0.0f, -300.0f });

	object5 = new EnemyHuman;
	object5->Initialize();
	object5->SetPosition({ 0.0f, 50.0f, 300.0f });
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
	camera->SetTarget({122.0f, 0.0f, -358.0f});
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(48.0f);

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
	objGround->SetScale({ 400,200,400 });

	objGround->SetPosition({ 0, -15, 0 });
	objExtendedGround->SetPosition({ 0, -10, 0 });
	playerPositionObject->SetPosition({ object1->GetPosition().x, 50.0f, object1->GetPosition().z});
	playerPositionObject->SetScale({ 10.0f, 10.0f, 10.0f });
	enemyPositionObject1->SetPosition({ object2->GetPosition().x, 50.0f, object2->GetPosition().z });
	enemyPositionObject2->SetPosition({ object3->GetPosition().x, 50.0f, object3->GetPosition().z });
	enemyPositionObject3->SetPosition({ object4->GetPosition().x, 50.0f, object4->GetPosition().z });
	enemyPositionObject4->SetPosition({ object5->GetPosition().x, 50.0f, object5->GetPosition().z });

	objExtendedGround->SetScale({ 1000, 1, 1000 });

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

	if (page > 1 && page < 3)
	{
		std::ostringstream missionTracker;

		height = playerPositionObject->GetPosition().y;
		camera->SetTarget({ camera->GetTarget().x, height, camera->GetTarget().z });
		camera->Update();

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
			if (distance1 < 6.0f)
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
			if (distance2 < 6.0f)
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
			if (distance3 < 6.0f)
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
			if (distance4 < 6.0f)
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

		if (intersect(objEAttackRange1->GetPosition(), object1->GetPosition(), 3.0f, 10.0f, 10.0f) && enemy1Alive == true && object2->attackTimer > 14 && object2->attackTimer < 16)
		{
			playerHP--;
		}
		if (intersect(objEAttackRange2->GetPosition(), object1->GetPosition(), 3.0f, 10.0f, 10.0f) && enemy2Alive == true && object3->attackTimer > 14 && object3->attackTimer < 16)
		{
			playerHP--;
		}
		if (intersect(objEAttackRange3->GetPosition(), object1->GetPosition(), 3.0f, 10.0f, 10.0f) && enemy3Alive == true && object4->attackTimer > 14 && object4->attackTimer < 16)
		{
			playerHP--;
		}
		if (intersect(objEAttackRange4->GetPosition(), object1->GetPosition(), 3.0f, 10.0f, 10.0f) && enemy4Alive == true && object5->attackTimer > 14 && object5->attackTimer < 16)
		{
			playerHP--;
		}

		if (playerHP < 1)
		{
			page++;
		}

		if (attackTime > 10 && attackTime < 20)
		{
			if (intersect(objAttackRange->GetPosition(), object2->GetPosition(), 3.0f, 25.0f, 25.0f) && enemy1Alive == true)
			{
				enemy1Alive = false;
				enemyDefeated++;
				object2->dead = true;
				object2->modelChange = true;
				ParticleCreation(object2->GetPosition().x, object2->GetPosition().y, object2->GetPosition().z, 60, 5.0f, 10.0f);
				e1Respawn = 2500;
			}

			if (intersect(objAttackRange->GetPosition(), object3->GetPosition(), 3.0f, 25.0f, 25.0f) && enemy2Alive == true)
			{
				enemy2Alive = false;
				enemyDefeated++;
				object3->dead = true;
				object3->modelChange = true;
				ParticleCreation(object3->GetPosition().x, object3->GetPosition().y, object3->GetPosition().z, 60, 5.0f, 10.0f);
				e2Respawn = 2500;
			}

			if (intersect(objAttackRange->GetPosition(), object4->GetPosition(), 3.0f, 25.0f, 25.0f) && enemy3Alive == true)
			{
				enemy3Alive = false;
				enemyDefeated++;
				object4->dead = true;
				object4->modelChange = true;
				ParticleCreation(object4->GetPosition().x, object4->GetPosition().y, object4->GetPosition().z, 60, 5.0f, 10.0f);
				e3Respawn = 2500;
			}

			if (intersect(objAttackRange->GetPosition(), object5->GetPosition(), 3.0f, 25.0f, 25.0f) && enemy4Alive == true)
			{
				enemy4Alive = false;
				enemyDefeated++;
				object5->dead = true;
				object5->modelChange = true;
				ParticleCreation(object5->GetPosition().x, object5->GetPosition().y, object5->GetPosition().z, 60, 5.0f, 10.0f);
				e4Respawn = 2500;
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
			object2->SetPosition({ -300.0f, 50.0f, 0.0f });
			enemyPositionObject1->SetPosition({ -300.0f, 50.0f, 0.0f });
			object2->dead = false;
			object2->wander = false;
			object2->set = false;
			object2->timer = 239.0f;
			enemy1Alive = true;
		}

		if (e2Respawn <= 0 && !enemy2Alive)
		{
			object3->SetPosition({ 300.0f, 50.0f, 0.0f });
			enemyPositionObject2->SetPosition({ 300.0f, 50.0f, 0.0f });
			object3->dead = false;
			object3->wander = false;
			object3->set = false;
			object3->timer = 239.0f;
			enemy2Alive = true;
		}

		if (e3Respawn <= 0 && !enemy3Alive)
		{
			object4->SetPosition({ 0.0f, 50.0f, -300.0f });
			enemyPositionObject3->SetPosition({ 0.0f, 50.0f, -300.0f });
			object4->dead = false;
			object4->wander = false;
			object4->set = false;
			object4->timer = 239.0f;
			enemy3Alive = true;
		}

		if (e4Respawn <= 0 && !enemy4Alive)
		{
			object5->SetPosition({ 0.0f, 50.0f, 300.0f });
			enemyPositionObject4->SetPosition({ 0.0f, 50.0f, 300.0f });
			object5->dead = false;
			object5->wander = false;
			object5->set = false;
			object5->timer = 239.0f;
			enemy4Alive = true;
		}

		if (object1->GetPosition().x > 398.0f)
		{
			object1->SetPosition({ 398.0f, object1->GetPosition().y, object1->GetPosition().z });
			camera->SetTarget({ 398.0f, object1->GetPosition().y, object1->GetPosition().z });
		}
		else if (object1->GetPosition().x < -398.0f)
		{
			object1->SetPosition({ -398.0f, object1->GetPosition().y, object1->GetPosition().z });
			camera->SetTarget({ -398.0f, object1->GetPosition().y, object1->GetPosition().z });
		}

		if (object1->GetPosition().z > 398.0f)
		{
			object1->SetPosition({ object1->GetPosition().x, object1->GetPosition().y, 398.0f });
			camera->SetTarget({ object1->GetPosition().x, object1->GetPosition().y, 398.0f });
		}
		else if (object1->GetPosition().z < -398.0f)
		{
			object1->SetPosition({ object1->GetPosition().x, object1->GetPosition().y, -398.0f });
			camera->SetTarget({ object1->GetPosition().x, object1->GetPosition().y, -398.0f });
		}

		object1->SetPosition({ object1->GetPosition().x, playerPositionObject->GetPosition().y, object1->GetPosition().z });
		playerPositionObject->SetPosition({ object1->GetPosition().x, playerPositionObject->GetPosition().y, object1->GetPosition().z });
		object2->SetPosition({ object2->GetPosition().x, enemyPositionObject1->GetPosition().y, object2->GetPosition().z });
		enemyPositionObject1->SetPosition({ object2->GetPosition().x, enemyPositionObject1->GetPosition().y, object2->GetPosition().z });
		object3->SetPosition({ object3->GetPosition().x, enemyPositionObject2->GetPosition().y, object3->GetPosition().z });
		enemyPositionObject2->SetPosition({ object3->GetPosition().x, enemyPositionObject2->GetPosition().y, object3->GetPosition().z }); 
		object4->SetPosition({ object4->GetPosition().x, enemyPositionObject3->GetPosition().y, object4->GetPosition().z });
		enemyPositionObject3->SetPosition({ object4->GetPosition().x, enemyPositionObject3->GetPosition().y, object4->GetPosition().z }); 
		object5->SetPosition({ object5->GetPosition().x, enemyPositionObject4->GetPosition().y, object5->GetPosition().z });
		enemyPositionObject4->SetPosition({ object5->GetPosition().x, enemyPositionObject4->GetPosition().y, object5->GetPosition().z });

		HPBar->SetSize({ playerHP * 20.0f, 20.0f });
		STBar->SetSize({ object1->stamina * 2.0f, 20.0f });

		if (input->PushKey(DIK_LSHIFT) && object1->stamina > 0.0f || input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && object1->stamina > 0.0f)
		{
			ParticleCreation(object1->GetPosition().x, object1->GetPosition().y, object1->GetPosition().z, 10, -1.0f, 1.0f);
		}

		missionTracker << enemyDefeated << " / 10"
			<< std::fixed << std::setprecision(0)
			<< std::setw(2) << std::setfill('0');
		debugText->Print(missionTracker.str(), 1173.0f, 160.0f, 1.0f);

		object1->Update();
		playerPositionObject->Update();
		enemyPositionObject1->Update();
		enemyPositionObject2->Update();
		enemyPositionObject3->Update();
		enemyPositionObject4->Update();
		object2->Update();
		object3->Update();
		object4->Update();
		object5->Update();

		if (enemy1Alive)
		{
			objVisionRange1->Update();
			objEAttackRange1->Update();
		}
		if (enemy2Alive)
		{
			objVisionRange2->Update();
			objEAttackRange2->Update();
		}
		if (enemy3Alive)
		{
			objVisionRange3->Update();
			objEAttackRange3->Update();
		}
		if (enemy4Alive)
		{
			objVisionRange4->Update();
			objEAttackRange4->Update();
		}
		object6->Update();
		object7->Update();

		objAttackRange->Update();
		objSkydome->Update();
		objExtendedGround->SetPosition({ object1->GetPosition().x, -11, object1->GetPosition().z });
		objExtendedGround->Update();
		objGround->Update();
		collisionManager->CheckAllCollisions();
	}

	if (page > 2)
	{

	}

	//Debug Start
	char msgbuf[256];
	//char msgbuf2[256];
	//char msgbuf3[256];

	sprintf_s(msgbuf, 256, "X: %f\n", object2->timer);
	//sprintf_s(msgbuf2, 256, "Z: %f\n", object1->GetPosition().z);
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
	object1->Draw(cmdList);
	//playerPositionObject->Draw();
	if (attackTime > 10 && attackTime < 20)
	{
		objAttackRange->Draw();
	}

	if (/*enemy1Alive && */page == 2)
	{
		object2->Draw(cmdList);
	}
	if (/*enemy2Alive && */page == 2)
	{
		object3->Draw(cmdList);
	}
	if (/*enemy3Alive && */page == 2)
	{
		object4->Draw(cmdList);
	}
	if (/*enemy4Alive && */page == 2)
	{
		object5->Draw(cmdList);
	}

	objSkydome->Draw();
	objExtendedGround->Draw();
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
	if (page == 3)
	{
		p3->Draw();
	}
	if (page == 4)
	{
		p4->Draw();
	}

	if (page == 2)
	{
		HPBar->Draw();
		HPBarFrame->Draw();
		STBar->Draw();
		STBarFrame->Draw();
		mission1->Draw();
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

void GameScene::ParticleCreation(float x, float y, float z, int life, float offset, float start_scale)
{
	for (int i = 0; i < 10; i++) {
		// X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		const float rnd_pos = 5.0f;
		XMFLOAT3 pos{};
		pos.x = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + x;
		pos.y = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + y + offset;
		pos.z = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + z;

		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		XMFLOAT3 acc{};
		const float rnd_acc = 0.001f;
		acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		// 追加
		particleMan->Add(life, pos, vel, acc, start_scale, 0.0f);
	}
}