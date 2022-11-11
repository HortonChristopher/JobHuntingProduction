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

private:
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	DebugText* debugText;
	DebugCamera* camera = nullptr;
	ParticleManager* particleMan = nullptr;
	LightGroup* lightGroup = nullptr;

	
};

