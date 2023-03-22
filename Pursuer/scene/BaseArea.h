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
#include "Loading.h"

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
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private: // �ÓI�����o�ϐ�
	static const int debugTextTexNumber = 0;

public:
	BaseArea();
	~BaseArea();

	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	void Update();

	void Draw();

	int intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH);

	void ParticleCreation(float x, float y, float z, int life, float offset, float start_scale);
	void ParticleCreationHeal(float x, float y, float z, int life, float offset, float start_scale);
	void ParticleCreationEdge(float x, float y, float z, int life, float offset, float start_scale);
	float Distance(XMFLOAT3 player, XMFLOAT3 center);

	XMFLOAT3 ScreenShake(XMFLOAT3 playerPosition);

	void thread1();
	void thread2();
	void thread3();
	void thread4();

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
	std::array<Object3d*, 11> attackRangeOBJ = { {} }; // 0 = Player; 1 = Enemy1; 2 = Enemy2; 3 = Enemy3; 4 = Enemy4
	std::array<Object3d*, 10> landingPositionOBJ = { {} }; // Unlike above, no player object needed (players don't have flying attacks)
	Model* attackRangeMODEL = nullptr;

	// Vision range visual objects (Debug Use Only)
	std::array<Object3d*, 10> enemyVisionRangeOBJ = { {} }; // 0 = Enemy1; 1 = Enemy2; 2 = Enemy3; 3 = Enemy4
	Model* visionRangeMODEL = nullptr;

	// Mission sprite for base area
	Sprite* baseAreaMissionSPRITE = nullptr;

	// HP and Stamina Bar Sprites
	Sprite* HPBarSPRITE = nullptr;
	Sprite* HPBarFrameSPRITE = nullptr;
	Sprite* STBarSPRITE = nullptr;
	Sprite* STBarFrameSPRITE = nullptr;
	Sprite* healSPRITE = nullptr;
	Sprite* healControllerSPRITE = nullptr;
	Sprite* healKeyboardSPRITE = nullptr;
	Sprite* loadingBarSPRITE = nullptr;
	Sprite* loadingBarFrameSPRITE = nullptr;

	// Player aspects
	Player* playerFBX = nullptr;
	PlayerPositionObject* playerPositionOBJ = nullptr;
	Model* positionMODEL = nullptr; // Used for both player and enemies

	// Base area enemy aspects
	std::array<EnemyHuman*, 10> baseAreaEnemyFBX = { {} };
	std::array<PlayerPositionObject*, 10> baseAreaEnemyPositionOBJ = { {} };
		// Base area enemy alive bool
	std::array<bool, 10> baseAreaEnemyAliveBOOL = { {true, true, true, true, true, true, true, true, true, true} };
		// Base area enemy respawn timer
	std::array<float, 10> baseAreaEnemyRespawnTimerFLOAT = { {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f} };
		// Base area enemy spawn positions
	std::array<XMFLOAT3, 10> baseAreaEnemySpawnXMFLOAT3 = { {{0.0f, 20.0f, 200.0f}, // Pair 1
															{0.0f, 20.0f, 210.0f}, // Pair 1
															{200.0f, 20.0f, 0.0f}, // Pair 2
															{210.0f, 20.0f, 0.0f}, // Pair 2
															{0.0f, 20.0f, -200.0f}, // Pair 3
															{0.0f, 20.0f, -210.0f}, // Pair 3
															{-200.0f, 20.0f, 0.0f}, // Pair 4
															{-210.0f, 20.0f, 0.0f}, // Pair 4
															{0.0f, 20.0f, 0.0f}, // Pair 5
															{10.0f, 20.0f, 0.0f}} }; // Pair 5

	// Base Area minimap
	Sprite* baseAreaMinimapSPRITE = nullptr;
	Sprite* baseAreaMinimapPlayerSPRITE = nullptr;
	std::array<Sprite*, 10> baseAreaMinimapEnemySPRITE = { {} };

	// Base Area enemy HP bar
	std::array<Sprite*, 10> baseAreaEnemyHPBarSPRITE = { {} };
	std::array<Sprite*, 10> baseAreaEnemyHPBarFrameSPRITE = { {} };

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
	float enemyKnockbackTime = 0.0f;
	bool knockback = false;
	bool enemyKnockback = false;

	// Used due to heavy loading
	float startTimer = 0.0f;

	bool firstRun = true;
public:
	bool initialization = true;
	bool initializeFinished = false;
	bool deletion = false;
	int result = 0; // 0 = default, 1 = defeat, 2 = victory
};