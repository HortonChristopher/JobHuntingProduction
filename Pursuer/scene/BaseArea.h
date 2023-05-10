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
	bool FBXCollisionDetection(XMFLOAT3 FBX1, XMFLOAT3 FBX2, float FBX1R, float FBX2R);

	void ParticleCreation(float x, float y, float z, int life, float offset, float start_scale);
	void ParticleCreationHeal(float x, float y, float z, int life, float offset, float start_scale);
	void ParticleCreationEdge(float x, float y, float z, int life, float offset, float start_scale);
	void ParticleCreationExplosion(float x, float y, float z, int life, float offset, float start_scale);
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
	ParticleManager* particleManExplosion = nullptr;
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

	// Playable Area Limit
	float mapBorder = 398.0f;
	float mapBorderParticles = 100.0f;
	float mapBorderParticlePosition = 400.0f;
	float mapBorderVisibleDistance = 25.0f;

	// Attack range visual objects (Debug Use Only)
	std::array<Object3d*, 11> attackRangeOBJ = { {} }; // 0 = Player; 1 = Enemy1; 2 = Enemy2; 3 = Enemy3; 4 = Enemy4
	std::array<Object3d*, 10> landingPositionOBJ = { {} }; // Unlike above, no player object needed (players don't have flying attacks)
	Model* attackRangeMODEL = nullptr;

	// Vision range visual objects (Debug Use Only)
	std::array<Object3d*, 10> enemyVisionRangeOBJ = { {} }; // 0 = Enemy1; 1 = Enemy2; 2 = Enemy3; 3 = Enemy4
	Model* visionRangeMODEL = nullptr;

	// Mission sprite for base area
	Sprite* baseAreaMissionSPRITE = nullptr;
	bool startMissionSpriteMovement = false;
	float movementStartZPosition = 600.0f;
	bool xSet = false;
	bool ySet = false;
	bool sizeSet = false;

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
	Sprite* staminaWarningSPRITE = nullptr;
	float staminaSpriteAlpha = 1.0f;
	float staminaSpriteInteger = 0.8f;
	float blinkingStaminaAlpha = 1.0f;
	float blinkingStaminaSpriteInteger = 2.4f;
	bool staminaBlinkingEffect = false;

	// Player aspects
	Player* playerFBX = nullptr;
	PlayerPositionObject* playerPositionOBJ = nullptr;
	Model* positionMODEL = nullptr; // Used for both player and enemies
	float playerInteresectSize = 3.0f;

	// Base area enemy aspects
	int numberOfEnemiesTotal = 10;
	int closestEnemyDefaultNumber = 10; // This number should always be equal to numberOfEnemiesTotal
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
	float enemyAggroVisionRange = 80.0f;
	float maxChargeDistance = 80.0f;
	float minChargeDistance = 15.0f;
	float damageAdvantageShortLongRangeBorder = 40.0f;

	// Player Attack aspects
	float playerAttackRange = 25.0f;
	float playerAttackDamage = 1.0f;
	float playerFirstAttackStartTimer = 54.2f;
	float playerFirstAttackEndTimer = 62.2f;
	float playerSecondAttackStartTimer = 90.35f;
	float playerSecondAttackEndTimer = 98.3f;
	float playerThirdAttackStartTimer = 152.6f;
	float playerThirdAttackEndTimer = 160.64f;
	float playerAttackKnockbackDistance = 180.0f;
	float playerAttackKnockbackInterval = 60.0f;
	float playerAttackKnockbackMaxtime = 30.0f;

	// Player heal particle aspects
	int healNumberOfParticles = 180;
	float healFirstParticleRingTimer = 80.0f;
	float healSecondParticleRingTimer = 90.0f;
	float healThirdParticleRingTimer = 100.0f;

	// Enemy Attack aspects
	float baseAttackRange = 18.0f;
	float baseAttackDamage = 1.0f;
	float particleAttackRange = 12.0f;
	float particleAttackDamage = 3.0f;
	float particleAttackKnockbackDistance = 60.0f;
	float landingAttackRange = 24.0f;
	float landingAttackDamage = 5.0f;
	float landingAttackKnockbackDistance = 180.0f;
	float chargeAttackRange = 12.0f;
	float chargeAttackDamage = 4.0f;
	float chargeAttackKnockbackDistance = 120.0f;
	float twoEnemySurroundRange = 24.0f;
	float twoEnemySurroundDamage = 4.0f;
	float twoEnemySurroundKnockbackDistance = 120.0f;
	float knockbackInterval = 60.0f;
	float knockbackMaxTime = 30.0f;

	// Base Area minimap
	Sprite* baseAreaMinimapSPRITE = nullptr;
	Sprite* baseAreaMinimapPlayerSPRITE = nullptr;
	std::array<Sprite*, 10> baseAreaMinimapEnemySPRITE = { {} };

	// Base Area enemy HP bar
	std::array<Sprite*, 10> baseAreaEnemyHPBarSPRITE = { {} };
	std::array<Sprite*, 10> baseAreaEnemyHPBarFrameSPRITE = { {} };
	std::array<bool, 10> isInFront = { {} };

	// Fleeing aspects
	float fleeHP = 2.0f;
	float fleeSpeed = 30.0f;
	float fleeHelpCallRange = 35.0f;

	// Damage Overlay
	Sprite* baseAreaDamageOverlaySPRITE = nullptr;
	float damageOverlaySpriteALPHA = 1.0f;
	bool damageOverlayDisplay = false;

	// Screen shake
	bool screenShake = false;
	float shakeTimer = 0.0f;
	float shakeTimerInterval = 60.0f;
	float maxShakeTimer = 15.0f;

	// Base area general aspects
	bool attacking = false;
	float attackTime = 0.0f;
	int enemyDefeated = 0;
	int clearCondition = 5;
	float yRotationOffset = 90.0f;

	// Fade in-out sprite
	Sprite* fadeSPRITE = nullptr;
	float fadeSpriteALPHA = 1.0f;
	float fadeSpriteInteger = 0.4f;

	bool gameStart = false;

	float knockbackTime = 0.0f;
	float enemyKnockbackTime = 0.0f;
	bool knockback = false;
	bool enemyKnockback = false;

	// Used due to heavy loading
	float startTimer = 0.0f;
	float startTimerLimit = 220.0f;
	float startTimerIntegral = 20.0f;

	//Lock on aspects
	int closestEnemy = 20;
	bool lockOnSet = false;

	int oddEven = 1;

	bool firstRun = true;
public:
	bool initialization = true;
	bool initializeFinished = false;
	bool deletion = false;
	int result = 0; // 0 = default, 1 = defeat, 2 = victory
};