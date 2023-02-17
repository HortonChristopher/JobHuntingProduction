#include "Loading.h"

extern float loadingProgress = 0.1f;
extern std::atomic<float> loadingPercent = 0.1f;

Loading::Loading()
{
}

Loading::~Loading()
{
	debugText = nullptr;
	loadingScreenSprite = nullptr;
	loadingScreenSpriteBar = nullptr;
}

void Loading::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// Loading debug text
	if (!Sprite::LoadTexture(debugTextTexNumber, "debugfont.png")) { assert(0); return; }

	// Debug text initialization
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	if (!Sprite::LoadTexture(300, "LoadingScreen.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(299, "LoadingScreenBar.png")) { assert(0); return; }

	loadingScreenSprite = Sprite::Create(300, { 0.0f, 0.0f });
	loadingScreenSpriteBar = Sprite::Create(299, { 299.0f, 364.0f });
	loadingScreenSpriteBar->SetSize({ 10.0f * 68.2f, 54.0f});
}

void Loading::Update(float percent)
{
	if (loadingProgress < 0.1f)
	{
		loadingProgress = 0.1f;
	}
	if (loadingProgress > 100.0f)
	{
		loadingProgress = 100.0f;
	}

	loadingScreenSpriteBar->SetSize({ 10.0f * 68.2f, 54.0f });
	//loadingScreenSpriteBar->SetSize({ loadingPercent * 68.2f, 54.0f });
}

void Loading::Draw()
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
	//Object3d::PreDraw(cmdList);

	//// 3D Object Drawing

	//// Particle drawing
	//particleMan->Draw(cmdList);

	//Object3d::PostDraw();
#pragma endregion

#pragma region foregroundSpriteDrawing
	Sprite::PreDraw(cmdList);

	// Foreground sprite drawing
	loadingScreenSprite->Draw();
	loadingScreenSpriteBar->Draw();

	// Debug text drawing
	debugText->DrawAll(cmdList);

	// Sprite post draw
	Sprite::PostDraw();
#pragma endregion
}
