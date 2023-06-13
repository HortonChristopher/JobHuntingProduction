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
#include "BaseAreaConditionals.h"

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

	bool DontStackOnTop(XMFLOAT3 enemyPos, XMFLOAT3 playerPos, float collisionRadius);
	void MoveTowards(float& current, float target, float speed, float elapsedTime);

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
	const float mapBorder = 398.0f;
	const float mapBorderParticles = 100.0f;
	const float mapBorderParticlePosition = 400.0f;
	const float mapBorderVisibleDistance = 25.0f;

	// Attack range visual objects (Debug Use Only)
	std::array<Object3d*, 11> attackRangeOBJ = { {} }; // 0 = Player; 1 = Enemy1; 2 = Enemy2; 3 = Enemy3; 4 = Enemy4
	std::array<Object3d*, 10> landingPositionOBJ = { {} }; // Unlike above, no player object needed (players don't have flying attacks)
	Model* attackRangeMODEL = nullptr;
	const int attackRangeRadius = 15;
	const float attackRangeYOffset = 0.5f;

	// Vision range visual objects (Debug Use Only)
	std::array<Object3d*, 10> enemyVisionRangeOBJ = { {} }; // 0 = Enemy1; 1 = Enemy2; 2 = Enemy3; 3 = Enemy4
	Model* visionRangeMODEL = nullptr;
	const int visionRangeRadius = 40;
	const float visionRangeYOffset = 0.5f;

	// Mission sprite for base area
	Sprite* baseAreaMissionSPRITE = nullptr;
	bool startMissionSpriteMovement = false;
	const float movementStartZPosition = 600.0f;
	bool xSet = false;
	bool ySet = false;
	bool sizeSet = false;
	const XMFLOAT2 initialBaseAreaMissionSpritePosition = { 100.0f, 50.0f };
	const XMFLOAT2 initialBaseAreaMissionSpriteSize = { 1080.0f, 620.0f };
	const XMFLOAT2 targetBaseAreaMissionSpritePosition = { 1150.0f, 100.0f };
	const XMFLOAT2 targetBaseAreaMissionSpriteSize = { 100.0f, 80.0f };
	const float baseAreaMissionSpriteMovementSpeed = 360.0f;
	const XMFLOAT2 baseAreaMissionSpriteSizeSpeed = { 360.0f, 180.0f };
	const XMFLOAT2 missionTrackerPosition = { 1173.0f, 160.0f };

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
	const float staminaSpriteInteger = 0.8f;
	float blinkingStaminaAlpha = 1.0f;
	const float blinkingStaminaSpriteInteger = 2.4f;
	bool staminaBlinkingEffect = false;
	const float minAlpha = 0.0f;
	const float maxAlpha = 1.0f;
	const float HPBarSpriteMultiplier = 20.0f;
	const XMFLOAT2 HPBarSpriteSize = { 25.0f * 20.0f, 40.0f };
	const XMFLOAT2 STBarSpriteSize = { 50.0f, 20.0f };
	const XMFLOAT2 lockOnSTBarPosition = { 720.0f, 480.0f };
	const XMFLOAT2 lockOnSTWarningPosition = { 720.0f, 490.0f };
	const XMFLOAT2 standardSTBarPosition = { 720.0f, 380.0f };
	const XMFLOAT2 standardSTWarningPosition = { 720.0f, 390.0f };
	const float STBarRotation = 270.0f;

	// Player aspects
	Player* playerFBX = nullptr;
	PlayerPositionObject* playerPositionOBJ = nullptr;
	Model* positionMODEL = nullptr; // Used for both player and enemies
	float playerInteresectSize = 3.0f;

	// Base area enemy aspects
	const int numberOfEnemiesTotal = 10;
	const int closestEnemyDefaultNumber = 10; // This number should always be equal to numberOfEnemiesTotal
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
	const float enemyAggroVisionRange = 80.0f;
	const float maxChargeDistance = 80.0f;
	const float minChargeDistance = 15.0f;
	const float damageAdvantageShortLongRangeBorder = 40.0f;

	// Player Attack aspects
	const float playerAttackRange = 25.0f;
	const float playerAttackDamage = 1.0f;
	const float playerFirstAttackStartTimer = 54.2f;
	const float playerFirstAttackEndTimer = 62.2f;
	const float playerSecondAttackStartTimer = 90.35f;
	const float playerSecondAttackEndTimer = 98.3f;
	const float playerThirdAttackStartTimer = 152.6f;
	const float playerThirdAttackEndTimer = 160.64f;
	const float playerAttackKnockbackDistance = 180.0f;
	const float playerAttackKnockbackInterval = 60.0f;
	const float playerAttackKnockbackMaxtime = 30.0f;

	// Player heal particle aspects
	int healNumberOfParticles = 180;
	const float healFirstParticleRingTimer = 80.0f;
	const float healSecondParticleRingTimer = 90.0f;
	const float healThirdParticleRingTimer = 100.0f;
	const XMFLOAT2 healTrackerPosition = { 1182.0f, 614.0f };

	// Particle aspects
	const int particleLifeStandardPlusHalf = 90;
	const int particleLifeStandard = 60;
	const int particleLifeHalf = 30;
	const int particleHealLife = 10;
	const int particleSprintLife = 10;
	const float particleAttackOffset = 0.0f;
	const float particleAttackStartScale = 10.0f;
	const float landingAttackOffset = 2.0f;
	const float landingAttackStartScale = 40.0f;
	const float chargeAttackOffset = 4.0f;
	const float chargeAttackScale = 30.0f;
	const float parryParticleOffset = 6.0f;
	const float parryParticleStartScale = 20.0f;
	const float playerKnockbackAttackOffset = 5.0f;
	const float playerKnockbackAttackStartScale = 10.0f;
	const float healParticleOffset = 0.0f;
	const float healParticleStartScale = 1.0f;
	const float healParticleRadius = 10.0f;
	const float mapBorderParticleOffset = 5.0f;
	const float mapBorderParticleStartScale = 3.0f;
	const float sprintParticleOffset = 1.0f;
	const float sprintParticleStartScale = 1.0f;

	// Enemy Attack aspects
	const float baseAttackRange = 18.0f;
	const float baseAttackDamage = 1.0f;
	const float particleAttackRange = 12.0f;
	const float particleAttackDamage = 3.0f;
	const float particleAttackKnockbackDistance = 60.0f;
	const float landingAttackRange = 24.0f;
	const float landingAttackDamage = 5.0f;
	const float landingAttackKnockbackDistance = 180.0f;
	const float chargeAttackRange = 12.0f;
	const float chargeAttackDamage = 4.0f;
	const float chargeAttackKnockbackDistance = 120.0f;
	const float twoEnemySurroundRange = 24.0f;
	const float twoEnemySurroundDamage = 4.0f;
	const float twoEnemySurroundKnockbackDistance = 120.0f;
	const float knockbackInterval = 60.0f;
	const float knockbackMaxTime = 30.0f;
	const float knockbackYOffset = 3.0f;
	const float knockbackTimeReset = 0.0f;

	// Base Area minimap
	Sprite* baseAreaMinimapSPRITE = nullptr;
	Sprite* baseAreaMinimapPlayerSPRITE = nullptr;
	std::array<Sprite*, 10> baseAreaMinimapEnemySPRITE = { {} };
	const float minimapSpriteMultiplier = 0.3f;
	const XMFLOAT2 minimapSpriteOffset = { 165.0f, 545.0f };

	// Base Area enemy HP bar
	std::array<Sprite*, 10> baseAreaEnemyHPBarSPRITE = { {} };
	std::array<Sprite*, 10> baseAreaEnemyHPBarFrameSPRITE = { {} };
	std::array<bool, 10> isInFront = { {} };

	// Fleeing aspects
	const float fleeHP = 2.0f;
	const float fleeSpeed = 30.0f;
	const float fleeHelpCallRange = 35.0f;

	// Damage Overlay
	Sprite* baseAreaDamageOverlaySPRITE = nullptr;
	float damageOverlaySpriteALPHA = 1.0f;
	bool damageOverlayDisplay = false;

	// Screen shake
	bool screenShake = false;
	float shakeTimer = 0.0f;
	const float shakeTimerInterval = 60.0f;
	const float maxShakeTimer = 15.0f;
	const float shakeTimerReset = 0.0f;

	// Base area general aspects
	bool attacking = false;
	float attackTime = 0.0f;
	int enemyDefeated = 0;
	const int clearCondition = 5;
	const float yRotationOffset = 90.0f;

	// Fade in-out sprite
	Sprite* fadeSPRITE = nullptr;
	float fadeSpriteALPHA = 1.0f;
	const float fadeSpriteInteger = 0.4f;

	bool gameStart = false;

	float knockbackTime = 0.0f;
	float enemyKnockbackTime = 0.0f;
	bool knockback = false;
	bool enemyKnockback = false;

	// Used due to heavy loading
	float startTimer = 0.0f;
	const float startTimerLimit = 220.0f;
	const float startTimerIntegral = 20.0f;

	//Lock on aspects
	int closestEnemy = 20;
	bool lockOnSet = false;

	const float deltaTimeOneSecond = 60.0f;

	// Slow Motion aspects
	Sprite* slowMotionSPRITE = nullptr;
	Sprite* slowMotionBarSPRITE = nullptr;
	Sprite* slowMotionBarFrameSPRITE = nullptr;
	float slowMotionSpriteALPHA = 0.0f;
	const XMFLOAT2 slowMotionSpriteBarSize = { 50.0f, 20.0f };
	const XMFLOAT2 lockOnSlowMotionSpritePosition = { 745.0f, 480.0f };
	const XMFLOAT2 standardSlowMotionSpritePosition = { 745.0f, 380.0f };
	const float lockOnRotation = 270.0f;

	const float standardCameraDistance = 48.0f;
	const float cameraYOffset = 15.0f;
	const XMFLOAT3 skydomeScale = { 6, 6, 6 };
	const XMFLOAT3 enemyVisionRangeOBJScale = { 90, 1, 90 };
	const XMFLOAT3 attackRangeOBJScale = { 15, 1, 15 };
	const XMFLOAT3 standardCameraUp = { 0, 1, 0 };

	const XMFLOAT3 groundOBJScale = { 2, 0.5f, 2 };
	const XMFLOAT3 tutorialGroundOBJScale = { 50, 10, 50 };
	const XMFLOAT3 extendedGroundOBJScale = { 1000, 1, 1000 };

	const XMFLOAT3 groundOBJPosition = { 0, -15, 0 };
	const XMFLOAT3 tutorialGroundOBJPosition = { 0.0f, 18.0f, -1450.0f };
	const XMFLOAT3 extendedGroundOBJPosition = { 0, -10, 0 };

	const float initialPlayerYPosition = 20.0f;
	const float initialEnemyYPosition = 30.0f;

	int oddEven = 1;
	

	bool firstRun = true;
public:
	bool initialization = true;
	bool initializeFinished = false;
	bool deletion = false;
	int result = 0; // 0 = default, 1 = defeat, 2 = victory
	const int defeat = 1;
	const int victory = 2;
};