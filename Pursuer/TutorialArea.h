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

class TutorialArea
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
	enum tutorialStatus
	{
		INTROCUTSCENE = 1,
		MOVEMENTTUTORIAL = 2,
		STAMINATUTORIAL = 3,
		ATTACKTUTORIAL = 4,
		DODGETUTORIAL = 5,
		STRAFEB = 6,
		DODGE = 7,
		ATTACK = 8,
		DAMAGED = 9,
		DEAD = 10
	};

	tutorialStatus tutorialStatus = INTROCUTSCENE;

public:
	TutorialArea();
	~TutorialArea();

	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	void Update();

	void Draw();

	int intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH);

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
	Model* groundMODEL = nullptr;
	Object3d* skydomeOBJ = nullptr;
	Model* skydomeMODEL = nullptr;

	// Sprite Generation
	Sprite* tutorialTextFrame = nullptr;
	std::array<Sprite*, 5> tutorialTextSPRITE = { {} };

	// Player aspects
	TutorialPlayer* playerFBX = nullptr;
	PlayerPositionObject* playerPositionOBJ = nullptr;
	Model* positionMODEL = nullptr; // Used for both player and enemies

	bool tutorialActive = true;
	float progress = 0.0f;
	int tutorialPage = 0;
public:
	bool initialize = false;
};