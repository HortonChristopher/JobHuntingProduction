#include "TutorialArea.h"

using namespace DirectX;

extern XMFLOAT3 objectPosition;
extern XMFLOAT3 objectRotation;

extern DeltaTime* deltaTime;

TutorialArea::TutorialArea()
{
}

TutorialArea::~TutorialArea()
{
	safe_delete(camera);
	collisionManager = nullptr;
	particleMan = nullptr;
	debugText = nullptr;
}

void TutorialArea::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
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

	// Model Creation
	groundMODEL = Model::CreateFromOBJ("TutorialStage");
	
	// Touchable Objection Creation
	groundOBJ = TouchableObject::Create(groundMODEL);

	positionMODEL = Model::CreateFromOBJ("yuka");

	// Player initialization
	playerFBX = new Player;
	playerFBX->Initialize();
	playerPositionOBJ = PlayerPositionObject::Create();
	playerPositionOBJ->SetModel(positionMODEL);
	playerFBX->SetPosition({ 0.0f, 0.0f, -700.0f });

	// Ground Aspects
	groundOBJ->SetPosition({ 0, 0, 0 });
	groundOBJ->SetScale({ 50, 10, 50 });
	playerPositionOBJ->SetScale({ 10, 10, 10 });

	// Camera initial values
	camera->SetTarget(playerFBX->GetPosition());
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(48.0f);

	srand((unsigned int)time(NULL));
}

void TutorialArea::Update()
{
	lightGroup->Update();
	particleMan->Update();
	camera->SetTarget(playerFBX->GetPosition());
	camera->Update();

	playerFBX->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });
	playerPositionOBJ->SetPosition({ playerFBX->GetPosition().x, playerPositionOBJ->GetPosition().y, playerFBX->GetPosition().z });

#pragma region updates
	groundOBJ->Update();
	playerFBX->Update();
	playerPositionOBJ->Update();
	collisionManager->CheckAllCollisions();
#pragma endregion
}

void TutorialArea::Draw()
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
	//playerPositionOBJ->Draw();
	groundOBJ->Draw();

	// Particle drawing
	particleMan->Draw(cmdList);

	Object3d::PostDraw();
#pragma endregion

#pragma region foregroundSpriteDrawing
	Sprite::PreDraw(cmdList);

	// Foreground sprite drawing

	// Sprite post draw
	Sprite::PostDraw();
#pragma endregion
}

int TutorialArea::intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH)
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

void TutorialArea::ParticleCreation(float x, float y, float z, int life, float offset, float start_scale)
{
	for (int i = 0; i < 10; i++) {
		// X,Y,Z�S��[-5.0f,+5.0f]�Ń����_���ɕ��z
		const float rnd_pos = 5.0f;
		XMFLOAT3 pos{};
		pos.x = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + x;
		pos.y = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + y + offset;
		pos.z = ((float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f) + z;

		const float rnd_vel = 0.1f; // 0.1f
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		XMFLOAT3 acc{};
		const float rnd_acc = 0.001f; // 0.001f
		acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		// �ǉ�
		particleMan->Add(life, pos, vel, acc, start_scale, 0.0f);
	}
}