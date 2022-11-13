#include "TitleScreen.h"

using namespace DirectX;

extern DeltaTime* deltaTime;

TitleScreen::TitleScreen()
{
}

TitleScreen::~TitleScreen()
{
	safe_delete(skydomeMODEL);
	safe_delete(groundMODEL);
	safe_delete(extendedGroundMODEL);
	safe_delete(skydomeOBJ);
	safe_delete(groundOBJ);
	safe_delete(extendedGroundOBJ);
}

void TitleScreen::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->audio = audio;
	this->input = input;

	camera = new DebugCamera(WinApp::window_width, WinApp::window_height, input);

	// Particle Manager initialization/generation
	particleMan = ParticleManager::GetInstance();
	particleMan->SetCamera(camera);

	// Light Group Creation
	lightGroup = LightGroup::Create();

	// Setting camera
	Object3d::SetCamera(camera);

	// Loading debug text
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) { assert(0); return; }

	// Debug text initialization
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	// Model Initialization
	skydomeMODEL = Model::CreateFromOBJ("skydome");
	groundMODEL = Model::CreateFromOBJ("Landscape2");
	extendedGroundMODEL = Model::CreateFromOBJ("ground");

	// 3D object creation
	skydomeOBJ = Object3d::Create();
	skydomeOBJ->SetModel(skydomeMODEL);
	groundOBJ = TouchableObject::Create(groundMODEL);
	extendedGroundOBJ = TouchableObject::Create(extendedGroundMODEL);

	// Ground scale
	groundOBJ->SetScale({ 400, 200, 400 });
	extendedGroundOBJ->SetScale({ 1000, 1, 1000 });

	// Ground position
	groundOBJ->SetPosition({ 0, -15, 0 });
	extendedGroundOBJ->SetPosition({ 0, -10, 0 });

	// Camera initial values
	camera->SetTarget({ 0.0f, 0.0f, 0.0f });
	camera->SetUp({ 0, 1, 0 });
	camera->SetDistance(48.0f);
}

void TitleScreen::Update()
{
	lightGroup->Update();
	particleMan->Update();
	camera->Update();

	cameraTest += 10.0f;
	camera->SetEye({ camera->GetEye().x, cameraTest, camera->GetEye().z });

	skydomeOBJ->Update();
	extendedGroundOBJ->Update();
	groundOBJ->Update();
}

void TitleScreen::Draw()
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

	// Debug text drawing
	debugText->DrawAll(cmdList);

	// Sprite post draw
	Sprite::PostDraw();
#pragma endregion
}