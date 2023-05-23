#pragma once

#include "OBJLoader.h"
#include "Audio.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "DebugText.h"
#include "FbxLoader.h"
#include "Object3D.h"
#include "Object3DCreation.h"
#include "Textures.h"
#include "Scene.h"
#include "TitleScreen.h"
#include "TutorialArea.h"
#include "BaseArea.h"
#include "Play.h"
#include "GameClearCutscene.h"
#include "GameOverCutscene.h"
#include "PipelineStatus.h"
#include "FBXManager.h"
#include "ShadingComputation.h"
#include "SettingParameters.h"
#include "GameWindow.h"
#include "DirectXCommon.h"
#include "LightCamera.h"
#include "SceneManager.h"
#include "Sprite.h"
#include "SSAO.h" // Screen Space Ambient Occlusion
#include "SSAOCombination.h" // Screen Space Ambient Occlusion Combine Pass
#include "TextureResource.h"
#include "Vector.h"
#include "DeltaTime.h"
#include "WrapperComputation.h"

#include <memory>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

class GameWindow;
class DirectXCommon;
class LightCamera;
class SceneManager;
class Sprite;
class SSAO;
class SSAOCombination;
class TextureResource;
class Vector;
class GameFramework
{
public:
	static GameFramework* GetInstance();

	~GameFramework();

	void Initialize();

	void Run();

	void End();
private:
	enum AssetLevel
	{
		TITLE = 1,
		TUTORIAL = 2,
		MAINGAME = 3,
		GAMECLEAR = 4,
		GAMEOVER = 5,
		FINISHED = 6,
	};

	AssetLevel assetLevel = TITLE;

	GameFramework();

	static GameFramework* gameInstance;

	void AssetLoading();

	void PipelineCreation();

	void PostLoading();

	void DrawLoadTex();

	SceneManager* sceneManager = nullptr;
	std::unique_ptr<GameWindow> gameWindow;
	DirectXCommon* directX = nullptr;
	WrapperComputation* computationWrapper = nullptr;

	Vector3 cameraPos;

	std::unique_ptr <LightCamera> lightCamera = nullptr;
	std::unique_ptr <TextureResource> shadowMap = nullptr;
	std::unique_ptr <SSAO> ssao = nullptr;
	std::unique_ptr <SSAOCombination> ssaoCombination = nullptr;

	std::unique_ptr <TextureResource> normalResource = nullptr;
	std::unique_ptr <TextureResource> halfNormalResource = nullptr;
	std::unique_ptr <TextureResource> mainResource = nullptr;
	std::unique_ptr <TextureResource> depthResource = nullptr;
	std::unique_ptr <TextureResource> halfDepthResource = nullptr;
	std::unique_ptr <TextureResource> ssaoResource = nullptr;

	std::unique_ptr <Sprite> depthTex = nullptr;
	std::unique_ptr <Sprite> normalTex = nullptr;
	std::unique_ptr <Sprite> loadTex = nullptr;
	std::unique_ptr <Sprite> loadDot = nullptr;

	float dir[3] = { 1.0f, -0.60f, 0.0f };
	float distance = 150;

	// Loading bool
	bool loading;

	bool finishedLoadingAssets = false;
	bool finishedPipelineCreation = false;
};