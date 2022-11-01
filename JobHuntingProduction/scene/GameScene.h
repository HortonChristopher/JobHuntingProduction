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
#include <chrono>
#include "DeltaTime.h"
#include "TouchableObject.h"
#include "PlayerPositionObject.h"
#include "CollisionManager.h"

#include <vector>
#include <array>

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

	int intersect(XMFLOAT3 player, XMFLOAT3 wall, float circleR, float rectW, float rectH);

	void ParticleCreation(float x, float y, float z, int life, float offset, float start_scale);

public:
	//Player* object1 = nullptr;

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	DebugText* debugText;	

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	DebugCamera* camera = nullptr;
	Sprite* spriteBG = nullptr;
	ParticleManager* particleMan = nullptr;
	CollisionManager* collisionManager = nullptr;

	LightGroup* lightGroup = nullptr;

	Object3d* objSkydome = nullptr;
	TouchableObject* objGround = nullptr;
	TouchableObject* objExtendedGround = nullptr;
	Object3d* objAttackRange = nullptr;
	Object3d* objEAttackRange1 = nullptr;
	Object3d* objEAttackRange2 = nullptr;
	Object3d* objEAttackRange3 = nullptr;
	Object3d* objEAttackRange4 = nullptr;

	Sprite* p1 = nullptr;
	Sprite* p2 = nullptr;
	Sprite* p3 = nullptr;
	Sprite* p4 = nullptr;

	Sprite* mission1 = nullptr;

	Sprite* HPBar = nullptr;
	Sprite* HPBarFrame = nullptr;
	int playerHP = 10;

	Sprite* STBar = nullptr;
	Sprite* STBarFrame = nullptr;

	Model* modelAttackRange = nullptr;

	Model* modelSkydome = nullptr;
	Model* modelGround = nullptr;
	Model* modelExtendedGround = nullptr;

	FBX3DModel* model1 = nullptr;
	Player* object1 = nullptr;
	PlayerPositionObject* playerPositionObject = nullptr;

	FBX3DModel* model2 = nullptr;
	EnemyHuman* object2 = nullptr;
	PlayerPositionObject* enemyPositionObject1 = nullptr;
	EnemyHuman* object3 = nullptr;
	PlayerPositionObject* enemyPositionObject2 = nullptr;
	EnemyHuman* object4 = nullptr;
	PlayerPositionObject* enemyPositionObject3 = nullptr;
	EnemyHuman* object5 = nullptr;
	PlayerPositionObject* enemyPositionObject4 = nullptr;
	Player* object6 = nullptr;

	FBX3DModel* model3 = nullptr;
	Player* object7 = nullptr;

	Model* modelVisionRange = nullptr;
	Object3d* objVisionRange1 = nullptr;
	Object3d* objVisionRange2 = nullptr;
	Object3d* objVisionRange3 = nullptr;
	Object3d* objVisionRange4 = nullptr;

	bool attacking = false;
	int attackTime = 0;
	int enemyDefeated = 0;
	bool gateOpen = false;
	bool area1Clear = false;

	bool enemy1Alive = true;
	bool enemy2Alive = true;
	bool enemy3Alive = true;
	bool enemy4Alive = true;

	int e1Respawn = 0;
	int e2Respawn = 0;
	int e3Respawn = 0;
	int e4Respawn = 0;

	bool FirstRun = true;

	XMFLOAT3 enemy1Position = { 0, -10, 200 };
	XMFLOAT3 enemy2Position = { 200, -10, 0 };
	XMFLOAT3 enemy3Position = { 0, -10, -200 };
	XMFLOAT3 enemy4Position = { -200, -10, 0 };

	XMFLOAT3 movement1Position = { 0,0,0 };
	XMFLOAT3 movement2Position = { 0,0,0 };
	XMFLOAT3 movement3Position = { 0,0,0 };
	XMFLOAT3 movement4Position = { 0,0,0 };

	int page = 0;

	float movement = 1.0f;
	bool testReset = false;

	std::array<float, 4> distanceArray{ {0.0f, 0.0f, 0.0f, 0.0f} };
};

