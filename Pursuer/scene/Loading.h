#pragma once

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
#include <atomic>
#include <iostream>

class Loading
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
	Loading();
	~Loading();

	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);

	void Update(float percent);

	void Draw();

	void addLoadingPercent(float percent);
private:
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	DebugText* debugText;

	Sprite* loadingScreenSprite = nullptr;
	Sprite* loadingScreenSpriteBar = nullptr;

	float percentage = 0.0f;
};