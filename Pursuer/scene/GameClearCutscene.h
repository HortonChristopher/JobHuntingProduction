#pragma once

#include "SafeDelete.h"
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "Input.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "DebugText.h"
#include "Audio.h"
#include "DebugCamera.h"
#include "LightGroup.h"
#include "FBXGeneration.h"
#include "Player.h"
#include "EnemyHuman.h"
#include "Object3D.h"
#include "FbxLoader/FbxLoader.h"
#include "DeltaTime.h"
#include "TouchableObject.h"
#include "PlayerPositionObject.h"
#include "CollisionManager.h"

#include <chrono>
#include <vector>
#include <array>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <time.h>
#include <thread>

class GameClearCutscene
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

public:
	enum CutsceneStatus
	{
		PLAYERATTACK = 0,
		LOOKRIGHT = 1,
		LOOKLEFT = 2,
		RETURNCENTER = 3,
		PAUSE = 4,
		LOOKUP = 5,
		GREYSCREEN = 6,
		GAMECLEARSCREEN = 7
	};

	CutsceneStatus cutsceneStatus = PLAYERATTACK;

public:
	GameClearCutscene();
	~GameClearCutscene();

	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	void Update();

	void Draw();

private:
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	DebugText* debugText;
	DebugCamera* camera = nullptr;
	Sprite* spriteBG = nullptr;
	ParticleManager* particleMan = nullptr;
	CollisionManager* collisionManager = nullptr;
	LightGroup* lightGroup = nullptr;

	// Skydome & Ground objects & models
	Object3d* skydomeOBJ = nullptr;
	TouchableObject* groundOBJ = nullptr;
	Model* skydomeMODEL = nullptr;
	Model* groundMODEL = nullptr;

	// Player aspects
	Player* playerFBX = nullptr;
	PlayerPositionObject* playerPositionOBJ = nullptr;
	Model* positionMODEL = nullptr; // Used for both player and enemies

	// Base area enemy aspects
	std::array<EnemyHuman*, 8> cutsceneEnemyFBX = { {} };
	std::array<PlayerPositionObject*, 8> cutsceneEnemyPositionOBJ = { {} };

	// Fade in-out sprite
	Sprite* fadeSPRITE = nullptr;
	float fadeSpriteALPHA = 0.0f;

	float degrees = 0.0f;

	Sprite* gameClearSPRITE = nullptr;

public:
	bool initialization = true;
	bool initializeFinished = false;
	bool deletion = false;
};