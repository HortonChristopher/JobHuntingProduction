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

class GameOverCutscene
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
		PAUSE = 0,
		ENEMYATTACK = 1,
		PLAYERDAMAGE = 2,
		CAMERAMOVE = 3,
		GRAYSCREEN = 4,
		GAMEOVERSCREEN = 5
	};

	CutsceneStatus cutsceneStatus = PAUSE;

public:
	GameOverCutscene();
	~GameOverCutscene();

	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	void Update();

	void Draw();

	XMFLOAT3 ScreenShake(XMFLOAT3 playerPosition);

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

	// Base area enemy aspects
	EnemyHuman* cutsceneEnemyFBX = nullptr;
	PlayerPositionObject* cutsceneEnemyPositionOBJ = nullptr;
	Model* positionMODEL = nullptr;

	XMFLOAT3 cameraTargetPosition = { 0.0f, 0.0f, 0.0f };

	// Damage Overlay
	Sprite* cutsceneDamageOverlaySPRITE = nullptr;
	float damageOverlaySpriteALPHA = 1.0f;
	bool damageOverlayDisplay = false;

	// Screen shake
	bool screenShake = false;
	float shakeTimer = 0.0f;

	// Fade in-out sprite
	Sprite* fadeSPRITE = nullptr;
	float fadeSpriteALPHA = 0.0f;

	Sprite* gameOverSPRITE = nullptr;

	float degrees = 0.0f;
public:
	bool initialization = true;
	bool initializeFinished = false;
	bool deletion = false;
};