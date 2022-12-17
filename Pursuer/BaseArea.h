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

class BaseArea
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
	BaseArea();
	~BaseArea();

	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	void Update();

	void Draw();

	int intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH);

	void ParticleCreation(float x, float y, float z, int life, float offset, float start_scale);
	XMFLOAT3 ScreenShake(XMFLOAT3 playerPosition);

	void thread1();
	void thread2();
	void thread3();

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

	// First run bool for delta time
	bool firstRunBOOL = true;

	// Skydome & Ground objects & models
	Object3d* skydomeOBJ = nullptr;
	TouchableObject* groundOBJ = nullptr;
	TouchableObject* tutorialGroundOBJ = nullptr;
	TouchableObject* extendedGroundOBJ = nullptr;
	Model* skydomeMODEL = nullptr;
	Model* groundMODEL = nullptr;
	Model* tutorialGroundMODEL = nullptr;
	Model* extendedGroundMODEL = nullptr;

	// Attack range visual objects (Debug Use Only)
	std::array<Object3d*, 5> attackRangeOBJ = { {} }; // 0 = Player; 1 = Enemy1; 2 = Enemy2; 3 = Enemy3; 4 = Enemy4
	Model* attackRangeMODEL = nullptr;

	// Vision range visual objects (Debug Use Only)
	std::array<Object3d*, 4> enemyVisionRangeOBJ = { {} }; // 0 = Enemy1; 1 = Enemy2; 2 = Enemy3; 3 = Enemy4
	Model* visionRangeMODEL = nullptr;

	// Mission sprite for base area
	Sprite* baseAreaMissionSPRITE = nullptr;

	// HP and Stamina Bar Sprites
	Sprite* HPBarSPRITE = nullptr;
	Sprite* HPBarFrameSPRITE = nullptr;
	Sprite* STBarSPRITE = nullptr;
	Sprite* STBarFrameSPRITE = nullptr;

	// Player aspects
	Player* playerFBX = nullptr;
	PlayerPositionObject* playerPositionOBJ = nullptr;
	Model* positionMODEL = nullptr; // Used for both player and enemies

	// Base area enemy aspects
	std::array<EnemyHuman*, 4> baseAreaEnemyFBX = { {} };
	std::array<PlayerPositionObject*, 4> baseAreaEnemyPositionOBJ = { {} };
		// Base area enemy alive bool
	std::array<bool, 4> baseAreaEnemyAliveBOOL = { {true, true, true, true} };
		// Base area enemy respawn timer
	std::array<float, 4> baseAreaEnemyRespawnTimerFLOAT = { {0.0f, 0.0f, 0.0f, 0.0f} };
		// Base area enemy spawn positions
	std::array<XMFLOAT3, 4> baseAreaEnemySpawnXMFLOAT3 = { {{0.0f, 30.0f, 200.0f}, // Enemy 1
															{200.0f, 30.0f, 0.0f}, // Enemy 2
															{0.0f, 30.0f, -200.0f}, // Enemy 3
															{-200.0f, 30.0f, 0.0f}} }; // Enemy 4

	// Base Area minimap
	Sprite* baseAreaMinimapSPRITE = nullptr;
	Sprite* baseAreaMinimapPlayerSPRITE = nullptr;
	std::array<Sprite*, 4> baseAreaMinimapEnemySPRITE = { {} };

	// Base Area enemy HP bar
	std::array<Sprite*, 4> baseAreaEnemyHPBarSPRITE = { {} };
	std::array<Sprite*, 4> baseAreaEnemyHPBarFrameSPRITE = { {} };

	// Damage Overlay
	Sprite* baseAreaDamageOverlaySPRITE = nullptr;
	float damageOverlaySpriteALPHA = 1.0f;
	bool damageOverlayDisplay = false;

	// Screen shake
	bool screenShake = false;
	float shakeTimer = 0.0f;

	// Base area general aspects
	bool attacking = false;
	float attackTime = 0.0f;
	int enemyDefeated = 0;

	// Fade in-out sprite
	Sprite* fadeSPRITE = nullptr;
	float fadeSpriteALPHA = 1.0f;

	bool gameStart = false;

	float knockbackTime = 0.0f;
	bool knockback = false;
public:
	bool initialization = true;
	bool deletion = false;
	int result = 0; // 0 = default, 1 = defeat, 2 = victory
};