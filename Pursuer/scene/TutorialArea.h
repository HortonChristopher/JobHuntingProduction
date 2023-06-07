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
#include "TutorialPlayer.h"
#include "TutorialEnemy.h"
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

class TutorialArea
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
	enum TutorialStatus
	{
		INTROCUTSCENE = 1,
		MOVEMENTTUTORIAL = 2,
		STAMINATUTORIAL = 3,
		ATTACKTUTORIAL = 4,
		DODGETUTORIAL = 5,
		BATTLETUTORIAL = 6,
		TUTORIALEND = 7
	};

	TutorialStatus tutorialStatus = INTROCUTSCENE;

public:
	TutorialArea();
	~TutorialArea();

	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	void Update();

	void Draw();

	int intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH);
	void ParticleCreation(float x, float y, float z, int life, float offset, float start_scale);
	void ParticleCreationHeal(float x, float y, float z, int life, float offset, float start_scale);
	void ParticleCreationEdge(float x, float y, float z, int life, float offset, float start_scale);
	float distance(XMFLOAT3 player, XMFLOAT3 center);

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

	// Stage Model and Obj
	TouchableObject* groundOBJ = nullptr;
	TouchableObject* outsideGroundOBJ = nullptr;
	Model* groundMODEL = nullptr;
	Model* outsideGroundMODEL = nullptr;
	Object3d* skydomeOBJ = nullptr;
	Model* skydomeMODEL = nullptr;

	XMFLOAT3 skydomeScale = { 5, 5, 5 };

	Object3d* playerAttackRangeOBJ = nullptr;
	Object3d* enemyAttackRangeOBJ = nullptr;

	// Sprite Generation
	Sprite* tutorialTextFrameSPRITEc = nullptr;
	Sprite* tutorialTextFrameSPRITEk = nullptr;
	std::array<Sprite*, 26> tutorialTextSPRITE = { {} };
	int numberOfTutorialTextSprites = 26;
	std::array<Sprite*, 4> tutorialMissionSPRITE = { {} };
	Sprite* missionBarSPRITE = nullptr;
	Sprite* missionBarFrameSPRITE = nullptr;
	// HP and Stamina Bar Sprites
	Sprite* HPBarSPRITE = nullptr;
	Sprite* HPBarFrameSPRITE = nullptr;
	Sprite* STBarSPRITE = nullptr;
	Sprite* STBarFrameSPRITE = nullptr;
	Sprite* staminaTutorialMaskSPRITE = nullptr;
	Sprite* minimapTutorialMaskSPRITE = nullptr;
	Sprite* tutorialMinimapSPRITE = nullptr;
	Sprite* tutorialMinimapPlayerSPRITE = nullptr;
	Sprite* tutorialMinimapEnemySPRITE = nullptr;
	Sprite* healSPRITE = nullptr;
	Sprite* healControllerSPRITE = nullptr;
	Sprite* healKeyboardSPRITE = nullptr;
	Sprite* healTutorialMaskSPRITE = nullptr;
	Sprite* fadeSPRITE = nullptr;
	Sprite* staminaWarningSPRITE = nullptr;
	float fadeSpriteAlpha = 0.0f;
	float staminaSpriteAlpha = 1.0f;
	float staminaSpriteInteger = 0.8f;
	float blinkingStaminaAlpha = 1.0f;
	float blinkingStaminaSpriteInteger = 2.4f;
	bool staminaBlinkingEffect = false;

	// Sprite Positions and Sizes
	const XMFLOAT2 tutorialTextFrameSPRITEPos = { 390.0f, 300.0f };
	const XMFLOAT2 tutorialTextSpritePos = { 390.0f, 300.0f };
	const XMFLOAT2 tutorialMissionSPRITEPos = { 1150.0f, 100.0f };
	const XMFLOAT2 tutorialMissionSPRITESize = { 100.0f, 80.0f };
	const XMFLOAT2 missionBarSPRITEPos = { 1150.0f, 150.0f };
	const XMFLOAT2 missionBarFrameSPRITEPos = { 1150.0f, 150.0f };
	const XMFLOAT2 missionBarSPRITESize = { 100.0f, 30.0f };
	const XMFLOAT2 missionBarFrameSPRITESize = { 100.0f, 30.0f };
	const XMFLOAT2 HPBarSPRITEPos = { 25.0f, 25.0f };
	const XMFLOAT2 HPBarFrameSPRITEpos = { 25.0f, 25.0f };
	const XMFLOAT2 STBarSPRITEPos = { 25.0f, 55.0f };
	const XMFLOAT2 STBarFrameSPRITEPos = { 25.0f, 55.0f };
	const XMFLOAT2 staminaTutorialMaskSPRITEPos = { 0.0f, 0.0f };
	const XMFLOAT2 tutorialMinimapSPRITEPos = { 50.0f, 430.0f };
	const XMFLOAT2 minimapTutorialMaskSPRITEPos = { 0.0f, 0.0f };
	XMFLOAT2 tutorialMinimapEnemySPRITEPos = { 0.0f, 0.0f };
	XMFLOAT2 tutorialMinimapPlayerSPRITEPos = { 0.0f, 0.0f };
	const XMFLOAT2 tutorialAreaDamageOverlaySPRITEPos = { 0.0f, 0.0f };
	const XMFLOAT2 healSPRITEPos = { 1102.0f, 542.0f };
	const XMFLOAT2 healKeyboardSPRITEPos = { 1102.0f, 542.0f };
	const XMFLOAT2 healControllerSPRITEPos = { 1102.0f, 542.0f };
	const XMFLOAT2 healTutorialMaskSPRITEPos = { 0.0f, 0.0f };
	const XMFLOAT2 fadeSPRITEPos = { 0.0f, 0.0f };
	const XMFLOAT2 staminaWarningSPRITEPos = { 720.0f, 190.0f };
	const XMFLOAT2 staminaWarningSPRITESize = { 75.0f, 42.0f };

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

	// Damage Overlay
	Sprite* tutorialAreaDamageOverlaySPRITE = nullptr;
	float damageOverlaySpriteALPHA = 1.0f;
	bool damageOverlayDisplay = false;

	// Screen shake
	bool screenShake = false;
	float shakeTimer = 0.0f;

	bool doorClose = false;
	bool doorOpenCutscene = false;
	const int numberOfDoorsTotal = 2;
	const XMFLOAT3 doorScale = { 50, 40, 10 };
	const XMFLOAT3 doorPos = { 0.0f, 80.0f, -520.0f };

	// Player aspects
	TutorialPlayer* playerFBX = nullptr;
	PlayerPositionObject* playerPositionOBJ = nullptr;
	Model* positionMODEL = nullptr; // Used for both player and enemies
	const XMFLOAT3 playerInitialPos = { 0.0f, 0.0f, -700.0f };

	// Enemy aspects
	TutorialEnemy* enemyFBX = nullptr;
	PlayerPositionObject* enemyPositionOBJ = nullptr;
	const XMFLOAT3 enemyInitialPos = { 0.0f, 0.0f, 0.0f };

	std::array<TouchableObject*, 2> doorOBJ = { {} };
	Model* doorMODEL = nullptr;
	const XMFLOAT3 groundPos = { 0, 0, 0 };
	const XMFLOAT3 groundScale = { 50, 10, 50 };
	const XMFLOAT3 outsideGroundPos = { 0.0f, -33.0f, 1150.0f };
	const XMFLOAT3 outsideGroundScale = { 400, 200, 400 };
	const XMFLOAT3 playerPositionOBJScale = { 10, 10, 10 };
	const XMFLOAT3 enemyPositionOBJScale = { 10, 10, 10 };

	const XMFLOAT3 cameraUp = { 0, 1, 0 };
	float cameraDistance = 48.0f;

	bool arenaClamp = false;
	XMFLOAT3 fromOriginToObject = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 targetDifference = { 0.0f, 0.0f, 0.0f };
	int targetMove = 0;
	bool tutorialActive = true;
	float progress = 0.0f;
	int tutorialPage = 0;
	float enemyKnockbackTime = 0.0f;
	bool enemyKnockback = false;
	float knockbackTime = 0.0f;
	bool knockback = false;

	// Used due to heavy loading
	float startTimer = 0.0f;
public:
	bool initialize = false;
	bool deletion = false;
};