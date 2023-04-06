#include "Loading.h"

extern std::atomic<int> loadingPercent;
extern std::atomic<int> percent(0);
extern std::atomic<bool> loadingFinished;
extern float loadingProgress = 0.1f;
extern DeltaTime* deltaTime;

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

	loadingPercent.store(0);

	loadingScreenSprite = Sprite::Create(300, { 0.0f, 0.0f });
	loadingScreenSpriteBar = Sprite::Create(299, { 299.0f, 364.0f });
	loadingScreenSpriteBar->SetSize({ (float)loadingPercent.load() * 6.82f, 54.0f});
}

void Loading::Update()
{
	//percentage = loadingProgress;
	//percentage += 40.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	if (loadingPercent.load() >= 10)
	{
		loadingPercent.store(10);
		loadingScreenSpriteBar->SetSize({ (float)loadingPercent.load() * 68.2f, 54.0f });
		loadingFinished.store(true);
	}

	loadingScreenSpriteBar->SetSize({ (float)loadingPercent.load() * 68.2f, 54.0f });

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