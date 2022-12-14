#pragma once

#include "Sprite.h"
#include "Input.h"
#include "Audio.h"
#include "SafeDelete.h"
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "ParticleManager.h"
#include "Object3d.h"
#include "FbxLoader/FbxLoader.h"
#include "FBXGeneration.h"
#include "TouchableObject.h"
#include "LightGroup.h"
#include "DebugText.h"
#include "DebugCamera.h"
#include "DeltaTime.h"

class TitleScreen
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
	TitleScreen();
	~TitleScreen();

	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	void Update();

	void Draw();

	void ParticleCreation(float x, float y, float z, int life, float offset, float start_scale);

private:
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	DebugText* debugText;
	DebugCamera* camera = nullptr;
	ParticleManager* particleMan = nullptr;
	LightGroup* lightGroup = nullptr;

	// Skydome and Ground Objects
	Object3d* skydomeOBJ = nullptr;
	TouchableObject* groundOBJ = nullptr;
	TouchableObject* extendedGroundOBJ = nullptr;
	Model* skydomeMODEL = nullptr;
	Model* groundMODEL = nullptr;
	Model* extendedGroundMODEL = nullptr;

	Sprite* titleSPRITE = nullptr;
	Sprite* titleStartStopSPRITE = nullptr;
	float titleSpriteALPHA = 0.0f;
	float degrees = 0.0f;

	std::array<Sprite*, 2> tutorialSelectionSPRITE = { {} };
	bool tutorialSelectionBOOL = false;

	Sprite* fadeSPRITE = nullptr;
	float fadeSpriteALPHA = 0.0f;
	bool selectionBOOL = false;

	bool particleMovement = false;
	float particleYPosition = 23.0f;
public:
	bool initialization = true;
	bool firstRun = true;
	bool deletion = false;
	bool tutorialStart = false;
	int selection = 0;
	bool gameStart = false;
};

