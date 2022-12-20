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
	float distance(XMFLOAT3 player, XMFLOAT3 center);

	void ParticleCreation(float x, float y, float z, int life, float offset, float start_scale);

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
	
	Object3d* playerAttackRangeOBJ = nullptr;
	Object3d* enemyAttackRangeOBJ = nullptr;

	// Sprite Generation
	Sprite* tutorialTextFrameSPRITE = nullptr;
	std::array<Sprite*, 26> tutorialTextSPRITE = { {} };
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

	// Player aspects
	TutorialPlayer* playerFBX = nullptr;
	PlayerPositionObject* playerPositionOBJ = nullptr;
	Model* positionMODEL = nullptr; // Used for both player and enemies

	// Enemy aspects
	TutorialEnemy* enemyFBX = nullptr;
	PlayerPositionObject* enemyPositionOBJ = nullptr;

	bool arenaClamp = false;
	XMFLOAT3 fromOriginToObject = { 0.0f, 0.0f, 0.0f };
	bool tutorialActive = true;
	float progress = 0.0f;
	int tutorialPage = 0;
public:
	bool initialize = false;
	bool deletion = false;
};