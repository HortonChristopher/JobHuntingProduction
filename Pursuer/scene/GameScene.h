#pragma once

#include "SafeDelete.h"
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "Input.h"
#include "Sprite.h"
#include "Audio.h"
#include "DeltaTime.h"
#include "Texture.h"
#include "BaseArea.h"
#include "TitleScreen.h"
#include "TutorialArea.h"
#include "GameOverCutscene.h"
#include "GameClearCutscene.h"
#include "Loading.h"

#include <atomic>
#include <iostream>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

public: // メンバ関数

	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// Pipeline Creation
	void CreatePipelines();

	void LoadResources(); // Loading 2D and 3D assets

	void thread1();
	void thread2();
	void thread3();

	void addLoadingPercent(float percent);

public:
	//Player* object1 = nullptr;

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;

	TitleScreen* titleScreen = nullptr;
	TutorialArea* tutorialArea = nullptr;
	BaseArea* baseArea = nullptr;
	GameOverCutscene* gameOverCutscene = nullptr;
	GameClearCutscene* gameClearCutscene = nullptr;
	Loading* loadingScreen = nullptr;
	bool showLoading = false;

	Sprite* fadeSPRITE = nullptr;
	float fadeSpriteAlpha = 1.0f;

	int createPipelineLevel = 0; // Pipeline level
	int loadResourceLevel = 0; // Resource level

	bool createPipelineFinish = false;
	bool loadResourceFinish = false;


	int tutorialOrBase = 0; // 0 = tutorial, 1 = base
public:
	int page = 0;
	bool mainLoop = true;
};

