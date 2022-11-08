#include "BaseArea.h"

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;

BaseArea::BaseArea()
{
}

BaseArea::~BaseArea()
{
	// Safe Delete
	safe_delete(camera);
	collisionManager = nullptr;
	particleMan = nullptr;
	debugText = nullptr;
	safe_delete(HPBarSPRITE);
	safe_delete(HPBarFrameSPRITE);
	safe_delete(STBarSPRITE);
	safe_delete(STBarFrameSPRITE);
	safe_delete(baseAreaMissionSPRITE);
	safe_delete(skydomeMODEL);
	safe_delete(groundMODEL);
	safe_delete(extendedGroundMODEL);
	safe_delete(positionMODEL);
	safe_delete(attackRangeMODEL);
	safe_delete(visionRangeMODEL);
	safe_delete(skydomeOBJ);
	for (int i = 0; i < 5; i++) { safe_delete(attackRangeOBJ[i]); }
	for (int i = 0; i < 4; i++) { safe_delete(enemyVisionRangeOBJ[i]); }
	safe_delete(playerPositionOBJ);
	for (int i = 0; i < 4; i++) { safe_delete(baseAreaEnemyPositionOBJ[i]); }
	safe_delete(groundOBJ);
	safe_delete(extendedGroundOBJ);
	safe_delete(playerFBX);
	for (int i = 0; i < 4; i++) { safe_delete(baseAreaEnemyFBX[i]); }
}

void BaseArea::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// Checking for nullptr
	assert(dxCommon);
	assert(input);
	assert(audio);

	// Assigning variables to this
	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// Camera initialization
	camera = new DebugCamera(WinApp::window_width, WinApp::window_height, input);

	// Collision Manager initialization
	collisionManager = CollisionManager::GetInstance();

	// Particle Manager initialization/generation
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	// Light Group Creation
	lightGroup = LightGroup::Create();

	// Setting DxCommon device
	FBXGeneration::SetDevice(dxCommon->GetDevice());
	EnemyHuman::SetDevice(dxCommon->GetDevice());
	Player::SetDevice(dxCommon->GetDevice());

	// Setting camera
	Object3d::SetCamera(camera);
	FBXGeneration::SetCamera(camera);
	EnemyHuman::SetCamera(camera);
	Player::SetCamera(camera);
	PlayerPositionObject::SetCamera(camera);

	//std::thread th1(&BaseArea::thread1, std::ref(dxCommon), std::ref(input), std::ref(audio), this);
	std::thread th2(&BaseArea::thread2, this); // 2D Initialization
	std::thread th3(&BaseArea::thread3, this); // 3D Initialization (other than touchable objects)
	std::thread th4(&BaseArea::thread4, this); // Model and Touchable Object Initialization
	//th1.join();
	th2.join();
	th3.join();
	th4.join();
}

void BaseArea::Update()
{
	lightGroup->Update();
	particleMan->Update();
	
	// Debug Text string
	std::ostringstream missionTracker;

	camera->SetTarget(playerPositionOBJ->GetPosition());
	camera->Update();

	if (input->TriggerKey(DIK_SPACE) && attackTime == 0 || input->TriggerControllerButton(XINPUT_GAMEPAD_A) && attackTime == 0.0f)
	{
		attackTime = 30.0f;
	}

	if (attackTime > 0)
	{
		attackTime -= 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	}
	else
	{
		attackTime = 0.0f;
	}

	objectPosition = playerFBX->GetPosition();
	objectRotation = playerFBX->GetRotation();

	skydomeOBJ->SetPosition(objectPosition);

#pragma region DebugAttackRange
	attackRangeOBJ[0]->SetPosition({ (objectPosition.x + (sinf(XMConvertToRadians(objectRotation.y)) * 15)), objectPosition.y + 0.5f, (objectPosition.z + (cosf(XMConvertToRadians(objectRotation.y)) * 15)) });
	attackRangeOBJ[0]->SetRotation(objectRotation);

	for (int i = 0; i < 4; i++)
	{
		attackRangeOBJ[i + 1]->SetPosition({ (baseAreaEnemyFBX[i]->GetPosition().x + (sinf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * 5)), baseAreaEnemyFBX[i]->GetPosition().y + 0.5f, (baseAreaEnemyFBX[i]->GetPosition().z + (sinf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * 5)) });
		attackRangeOBJ[i + 1]->SetRotation(baseAreaEnemyFBX[i]->GetRotation());
	}
#pragma endregion
	
#pragma region VisionRange
	for (int i = 0; i < 4; i++)
	{
		enemyVisionRangeOBJ[i]->SetPosition({ (baseAreaEnemyFBX[i]->GetPosition().x + (sinf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * 40)), baseAreaEnemyFBX[i]->GetPosition().y + 0.5f, (baseAreaEnemyFBX[i]->GetPosition().z + (cosf(XMConvertToRadians(baseAreaEnemyFBX[i]->GetRotation().y)) * 40))});
		enemyVisionRangeOBJ[i]->SetRotation(baseAreaEnemyFBX[i]->GetRotation());
	}
#pragma endregion

#pragma region EnemyAggro
	for (int i = 0; i < 4; i++)
	{
		if (intersect(playerFBX->GetPosition(), enemyVisionRangeOBJ[i]->GetPosition(), 3.0f, 60.0f, 60.0f) && baseAreaEnemyAliveBOOL[i] == true)
		{
			baseAreaEnemyFBX[i]->SetAggro(true);
			float distance = sqrt((baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x) * (baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x) + (baseAreaEnemyFBX[i]->GetPosition().y - playerFBX->GetPosition().y) * (baseAreaEnemyFBX[i]->GetPosition().y - playerFBX->GetPosition().y));
			if (distance < 6.0f)
			{
				baseAreaEnemyFBX[i]->SetAttack(true);
			}
			else
			{
				baseAreaEnemyFBX[i]->SetAttack(false);
			}
		}
		else
		{
			//baseAreaEnemyFBX[i]->SetAggro(false);
			baseAreaEnemyFBX[i]->SetAggroSwitch(true);
			baseAreaEnemyFBX[i]->SetAttack(false);
		}
	}
#pragma endregion

#pragma region playerHPDamage
	for (int i = 0; i < 4; i++)
	{
		if (intersect(attackRangeOBJ[i + 1]->GetPosition(), playerFBX->GetPosition(), 3.0f, 10.0f, 10.0f) && baseAreaEnemyAliveBOOL[i] == true && baseAreaEnemyFBX[i]->attackTimer > 14 && baseAreaEnemyFBX[i]->attackTimer < 16)
		{
			playerFBX->hp -= 1.0f;
		}
	}

	if (playerFBX->hp < 0.1f)
	{
		result = 1;
		deletion = true;
		playerFBX->hp = 0.0f;
	}
#pragma endregion

#pragma region playerAttack
	if (attackTime > 10.0f && attackTime < 20.0f)
	{
		for (int i = 0; i < 4; i++)
		{
			if (intersect(attackRangeOBJ[0]->GetPosition(), baseAreaEnemyFBX[i]->GetPosition(), 3.0f, 25.0f, 25.0f) && baseAreaEnemyAliveBOOL[i] == true)
			{
				baseAreaEnemyAliveBOOL[i] = false;
				enemyDefeated++;
				baseAreaEnemyFBX[i]->dead = true;
				baseAreaEnemyFBX[i]->modelChange = true;
				ParticleCreation(baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyFBX[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z, 60, 5.0f, 10.0f);
				baseAreaEnemyRespawnTimerFLOAT[i] = 2400;
			}
		}
	}
#pragma endregion

	if (enemyDefeated > 9)
	{
		result = 2;
		deletion = true;
	}

#pragma region enemyRespawn
	for (int i = 0; i < 4; i++)
	{
		if (baseAreaEnemyRespawnTimerFLOAT[i] > 0.0f)
		{
			baseAreaEnemyRespawnTimerFLOAT[i] -= 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
		}
		else
		{
			baseAreaEnemyRespawnTimerFLOAT[i] = 0.0f;
		}

		if (baseAreaEnemyRespawnTimerFLOAT[i] < 0.1f && !baseAreaEnemyAliveBOOL[i])
		{
			baseAreaEnemyFBX[i]->SetPosition(baseAreaEnemySpawnXMFLOAT3[i]);
			baseAreaEnemyPositionOBJ[i]->SetPosition(baseAreaEnemySpawnXMFLOAT3[i]);
			baseAreaEnemyFBX[i]->dead = false;
			baseAreaEnemyFBX[i]->wander = false;
			baseAreaEnemyFBX[i]->set = false;
			baseAreaEnemyFBX[i]->timer = 239.0f;
			baseAreaEnemyAliveBOOL[i] = true;
		}
	}
#pragma endregion

#pragma region areaBoundaryLimits
	if (playerFBX->GetPosition().x > 398.0f)
	{
		playerFBX->SetPosition({ 398.0f, playerFBX->GetPosition().y, playerFBX->GetPosition().z });
		camera->SetTarget(playerFBX->GetPosition());
	}
	else if (playerFBX->GetPosition().x < -398.0f)
	{
		playerFBX->SetPosition({ -398.0f, playerFBX->GetPosition().y, playerFBX->GetPosition().z });
		camera->SetTarget(playerFBX->GetPosition());
	}

	if (playerFBX->GetPosition().z > 398.0f)
	{
		playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, 398.0f });
		camera->SetTarget(playerFBX->GetPosition());
	}
	else if (playerFBX->GetPosition().z < -398.0f)
	{
		playerFBX->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y, -398.0f });
		camera->SetTarget(playerFBX->GetPosition());
	}
#pragma endregion

#pragma region FBXObjectPositionLineup
	// Makes sure the position object and fbx object XYZ values match each other
	playerFBX->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });
	playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });

	for (int i = 0; i < 4; i++)
	{
		baseAreaEnemyFBX[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyPositionOBJ[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
		baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, baseAreaEnemyPositionOBJ[i]->GetPosition().y, baseAreaEnemyFBX[i]->GetPosition().z });
	}
#pragma endregion

#pragma region HPSTUpdates
	HPBarSPRITE->SetSize({ playerFBX->hp * 20.0f, 20.0f });
	STBarSPRITE->SetSize({ playerFBX->stamina * 2.0f, 20.0f });

	if (input->PushKey(DIK_LSHIFT) && playerFBX->stamina > 0.0f || input->PushControllerButton(XINPUT_GAMEPAD_LEFT_SHOULDER) && playerFBX->stamina > 0.0f)
	{
		ParticleCreation(playerFBX->GetPosition().x, playerFBX->GetPosition().y, playerFBX->GetPosition().z, 10, -1.0f, 1.0f);
	}
#pragma endregion

#pragma region missionTracker
	missionTracker << enemyDefeated << " / 10"
		<< std::fixed << std::setprecision(0)
		<< std::setw(2) << std::setfill('0');
	debugText->Print(missionTracker.str(), 1173.0f, 160.0f, 1.0f);
#pragma endregion

#pragma region updates
	playerFBX->Update();
	playerPositionOBJ->Update();
	attackRangeOBJ[0]->Update();
	for (int i = 0; i < 4; i++)
	{
		baseAreaEnemyFBX[i]->Update();
		baseAreaEnemyPositionOBJ[i]->Update();
		if (baseAreaEnemyAliveBOOL[i])
		{
			enemyVisionRangeOBJ[i]->Update();
			attackRangeOBJ[i + 1]->Update();
		}
	}
	skydomeOBJ->Update();
	extendedGroundOBJ->SetPosition({ playerFBX->GetPosition().x, -11, playerFBX->GetPosition().z });
	extendedGroundOBJ->Update();
	groundOBJ->Update();
	collisionManager->CheckAllCollisions();
#pragma endregion

#pragma region debugTestStrings
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
#pragma endregion
}

void BaseArea::Draw()
{
	// Setting command list
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region backgroundSpriteDrawing
	// Background sprite drawing preprocessing
	//Sprite::PreDraw(cmdList);
	// Background sprite drawing
	//spriteBG->Draw();

	/// <summary>
	/// Background sprite drawing processes
	/// </summary>

	// Sprite drawing post-processing
	//Sprite::PostDraw();
	// Clearing depth buffer
	//dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3DDrawing
	Object3d::PreDraw(cmdList);

	// 3D Object Drawing
	playerFBX->Draw(cmdList);

	for (int i = 0; i < 4; i++)
	{
		baseAreaEnemyFBX[i]->Draw(cmdList);
	}

	// Debug only
	if (attackTime > 10.0f && attackTime < 20.0f)
	{
		//attackRangeOBJ[0]->Draw();
	}

	for (int i = 0; i < 4; i++)
	{
		//attackRangeOBJ[i + 1]->Draw();
		//enemyVisionRangeOBJ[i]->Draw();
	}
	// End Debug

	skydomeOBJ->Draw();
	extendedGroundOBJ->Draw();
	groundOBJ->Draw();

	// Particle drawing
	particleMan->Draw(cmdList);

	Object3d::PostDraw();
#pragma endregion

#pragma region foregroundSpriteDrawing
	Sprite::PreDraw(cmdList);

	// Foreground sprite drawing
	HPBarSPRITE->Draw();
	HPBarFrameSPRITE->Draw();
	STBarSPRITE->Draw();
	STBarFrameSPRITE->Draw();
	baseAreaMissionSPRITE->Draw();

	// Debug text drawing
	debugText->DrawAll(cmdList);

	// Sprite post draw
	Sprite::PostDraw();
#pragma endregion
}

int BaseArea::intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH)
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

void BaseArea::ParticleCreation(float x, float y, float z, int life, float offset, float start_scale)
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

void BaseArea::thread1(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// Checking for nullptr
	assert(dxCommon);
	assert(input);
	assert(audio);

	// Assigning variables to this
	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// Camera initialization
	camera = new DebugCamera(WinApp::window_width, WinApp::window_height, input);

	// Collision Manager initialization
	collisionManager = CollisionManager::GetInstance();

	// Particle Manager initialization/generation
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	// Light Group Creation
	lightGroup = LightGroup::Create();

	// Setting DxCommon device
	FBXGeneration::SetDevice(dxCommon->GetDevice());
	EnemyHuman::SetDevice(dxCommon->GetDevice());
	Player::SetDevice(dxCommon->GetDevice());

	// Setting camera
	Object3d::SetCamera(camera);
	FBXGeneration::SetCamera(camera);
	EnemyHuman::SetCamera(camera);
	Player::SetCamera(camera);
	PlayerPositionObject::SetCamera(camera);
}

void BaseArea::thread2()
{
	// Loading debug text
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) { assert(0); return; }

	// Debug text initialization
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	// Sprite texture loading
	if (!Sprite::LoadTexture(1, L"Resources/HPBar.png")) { assert(0); return; } // HP bar texture
	if (!Sprite::LoadTexture(2, L"Resources/HPBarFrame.png")) { assert(0); return; } // HP bar frame texture
	if (!Sprite::LoadTexture(3, L"Resources/STBar.png")) { assert(0); return; } // ST bar texture
	if (!Sprite::LoadTexture(4, L"Resources/STBarFrame.png")) { assert(0); return; } // ST bar frame texture
	if (!Sprite::LoadTexture(5, L"Resources/Mission1.png")) { assert(0); return; } // Base mission texture

	// Sprite generation
	HPBarSPRITE = Sprite::Create(1, { 25.0f, 25.0f });
	HPBarFrameSPRITE = Sprite::Create(2, { 25.0f, 25.0f });
	STBarSPRITE = Sprite::Create(3, { 25.0f, 55.0f });
	STBarFrameSPRITE = Sprite::Create(4, { 25.0f, 55.0f });
	baseAreaMissionSPRITE = Sprite::Create(5, { 1150.0f, 100.0f });
	// Resizing mission sprite
	baseAreaMissionSPRITE->SetSize({ 100.0f, 80.0f });
}

void BaseArea::thread3()
{
	// 3D Object creation
	skydomeOBJ = Object3d::Create();
	for (int i = 0; i < 5; i++) { attackRangeOBJ[i] = Object3d::Create(); }
	for (int i = 0; i < 4; i++) { enemyVisionRangeOBJ[i] = Object3d::Create(); }
	playerPositionOBJ = PlayerPositionObject::Create();
	for (int i = 0; i < 4; i++) { baseAreaEnemyPositionOBJ[i] = PlayerPositionObject::Create(); }

	// Player initialization
	playerFBX = new Player;
	playerFBX->Initialize();

	// Enemy initialization
	for (int i = 0; i < 4; i++)
	{
		baseAreaEnemyFBX[i] = new EnemyHuman;
		baseAreaEnemyFBX[i]->Initialize();
		baseAreaEnemyFBX[i]->SetPosition(baseAreaEnemySpawnXMFLOAT3[i]);
		baseAreaEnemyFBX[i]->SetHomePosition({ baseAreaEnemySpawnXMFLOAT3[i].x, baseAreaEnemySpawnXMFLOAT3[i].z });
	}

	// Camera initial values
	camera->SetTarget(playerFBX->GetPosition());
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(48.0f);

	// Attack range initial values
	for (int i = 0; i < 5; i++)
	{
		if (i == 0)
		{
			attackRangeOBJ[i]->SetPosition({ playerFBX->GetPosition().x, playerFBX->GetPosition().y + 0.5f, playerFBX->GetPosition().z + 5.0f });
		}
		else
		{
			attackRangeOBJ[i]->SetPosition({ baseAreaEnemyFBX[i - 1]->GetPosition().x, baseAreaEnemyFBX[i - 1]->GetPosition().y + 0.5f, baseAreaEnemyFBX[i - 1]->GetPosition().z + 5.0f });
		}
		attackRangeOBJ[i]->SetScale({ 15, 1, 15 });
	}

	// Vision range initial values
	for (int i = 0; i < 4; i++)
	{
		enemyVisionRangeOBJ[i]->SetScale({ 90, 1, 90 });
	}

	// Skydome scale
	skydomeOBJ->SetScale({ 5,5,5 });

	// Position Object initial positions
	playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, 30.0f, playerFBX->GetPosition().z });
	for (int i = 0; i < 4; i++)
	{
		baseAreaEnemyPositionOBJ[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x, 30.0f, baseAreaEnemyFBX[i]->GetPosition().z });
	}
}

void BaseArea::thread4()
{
	// Model creation
	skydomeMODEL = Model::CreateFromOBJ("skydome");
	groundMODEL = Model::CreateFromOBJ("Landscape2");
	extendedGroundMODEL = Model::CreateFromOBJ("ground");
	positionMODEL = Model::CreateFromOBJ("yuka");
	attackRangeMODEL = Model::CreateFromOBJ("yuka");
	visionRangeMODEL = Model::CreateFromOBJ("yuka");

	// Setting 3D model
	skydomeOBJ->SetModel(skydomeMODEL);
	for (int i = 0; i < 5; i++) { attackRangeOBJ[i]->SetModel(attackRangeMODEL); }
	for (int i = 0; i < 4; i++) { enemyVisionRangeOBJ[i]->SetModel(visionRangeMODEL); }
	playerPositionOBJ->SetModel(positionMODEL);
	for (int i = 0; i < 4; i++) { baseAreaEnemyPositionOBJ[i]->SetModel(positionMODEL); }

	// Touchable object creation
	groundOBJ = TouchableObject::Create(groundMODEL);
	extendedGroundOBJ = TouchableObject::Create(extendedGroundMODEL);

	// Ground scale
	groundOBJ->SetScale({ 400,200,400 });
	extendedGroundOBJ->SetScale({ 1000, 1, 1000 });

	// Ground positions
	groundOBJ->SetPosition({ 0, -15, 0 });
	extendedGroundOBJ->SetPosition({ 0, -10, 0 });

	srand(time(NULL));
}