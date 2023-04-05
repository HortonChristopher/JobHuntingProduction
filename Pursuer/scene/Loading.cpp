#include "Loading.h"

extern float loadingProgress;

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

	percentage = 0.1f;

	loadingScreenSprite = Sprite::Create(300, { 0.0f, 0.0f });
	loadingScreenSpriteBar = Sprite::Create(299, { 299.0f, 364.0f });
	loadingScreenSpriteBar->SetSize({ percentage * 6.82f, 54.0f});
}

void Loading::Update()
{
	//percentage = loadingProgress;

	if (percentage >= 100.0f)
	{
		percentage = 100.0f;
	}

	loadingScreenSpriteBar->SetSize({ percentage * 6.82f, 54.0f });

	//Debug Start
	/*char msgbuf[256];
	sprintf_s(msgbuf, 256, "Loading: %f\n", percentage);
	OutputDebugStringA(msgbuf);*/
	//Debug End
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

void Loading::addLoadingPercent(float percent)
{
	percentage += percent;
	//Update();
}

void Loading::subtractLoadingPercent(float percent)
{
	percentage -= percent;
	//Update();
}