#include "GameFramework.h"

TextureResource* TextureResource::mainResource = nullptr;

GameFramework::GameFramework()
{
	gameWindow = std::make_unique<GameWindow>(1920, 1080);
	directX = DirectXCommon::GetInstance();
}

GameFramework* GameFramework::GetInstance()
{
	if (gameInstance == nullptr)
	{
		gameInstance = new GameFramework();
	}

	return gameInstance;
}

GameFramework::~GameFramework()
{
}

void GameFramework::AssetLoading()
{
	switch (assetLevel)
	{
	case TITLE: // Title Screen Assets
		// Texture initialization
		Textures::Initialize();

		// 2D Sprite loading
		Textures::LoadTexture("DebugText", "debugfont.png"); // Standard debug texture
		Textures::LoadTexture("Title", "Title.png");
		Textures::LoadTexture("TitleStartStop", "TitleStartStop.png");
		Textures::LoadTexture("BlackScreen", "BlackScreen.png"); // Also used in other scenes

		Textures::LoadTexture("TitleTutorialSelection_1", "TitleTutorialSelection_1.png");
		Textures::LoadTexture("TitleTutorialSelection_2", "TitleTutorialSelection_2.png");
		Textures::LoadTexture("TitleTutorialSelection_k", "TitleTutorialSelection_k.png");
		Textures::LoadTexture("TitleTutorialSelection_c", "TitleTutorialSelection_c.png");

		// Particles (used throughout the game)
		Textures::LoadTexture("BasicParticles", "effect1.png");
		Textures::LoadTexture("ExplosionParticles", "effect2.png");

		// 3D object loading
		OBJLoader::LoadModelFromFile("Skydome", "skydome/skydome.obj", false);
		OBJLoader::LoadModelFromFile("Landscape", "Landscape2/Landscape2.obj", false);
		OBJLoader::LoadModelFromFile("Ground", "ground/ground.obj", false);

		break;
	case TUTORIAL:
		// 2D Sprite loading
		Textures::LoadTexture("TutorialTextFrameC", "TutorialTextFrameC.png");
		Textures::LoadTexture("TutorialTextFrameK", "TutorialTextFrameK.png");
		// Tutorial 1
		Textures::LoadTexture("Tutorial1_1", "Tutorial1_1.png");
		Textures::LoadTexture("Tutorial1_2", "Tutorial1_2.png");
		Textures::LoadTexture("Tutorial1_3", "Tutorial1_3.png");
		Textures::LoadTexture("Tutorial1_3_k", "Tutorial1_3_k.png");
		Textures::LoadTexture("Tutorial1_3_c", "Tutorial1_3_c.png");
		Textures::LoadTexture("Tutorial1_4", "Tutorial1_4.png");

		// Tutorial 2
		Textures::LoadTexture("Tutorial2_1", "Tutorial2_1.png");
		Textures::LoadTexture("Tutorial2_2", "Tutorial2_2.png");
		Textures::LoadTexture("Tutorial2_3", "Tutorial2_3.png");
		Textures::LoadTexture("Tutorial2_3_k", "Tutorial2_3_k.png");
		Textures::LoadTexture("Tutorial2_3_k", "Tutorial2_3_c.png");
		Textures::LoadTexture("Tutorial2_4", "Tutorial2_4.png");

		// Tutorial 3
		Textures::LoadTexture("Tutorial3_1", "Tutorial3_1.png");
		Textures::LoadTexture("Tutorial3_2", "Tutorial3_2.png");
		Textures::LoadTexture("Tutorial3_2_k", "Tutorial3_2_k.png");
		Textures::LoadTexture("Tutorial3_2_c", "Tutorial3_2_c.png");
		Textures::LoadTexture("Tutorial3_3", "Tutorial3_3.png");
		Textures::LoadTexture("Tutorial3_4", "Tutorial3_4.png");

		// Tutorial 4
		Textures::LoadTexture("Tutorial4_1", "Tutorial4_1.png");
		Textures::LoadTexture("Tutorial4_2", "Tutorial4_2.png");
		Textures::LoadTexture("Tutorial4_3", "Tutorial4_3.png");
		Textures::LoadTexture("Tutorial4_3_k", "Tutorial4_3_k.png");
		Textures::LoadTexture("Tutorial4_3_c", "Tutorial4_3_c.png");
		Textures::LoadTexture("Tutorial4_4", "Tutorial4_4.png");
		Textures::LoadTexture("Tutorial4_5", "Tutorial4_5.png");
		Textures::LoadTexture("Tutorial4_6", "Tutorial4_6.png");
		
		// Healing
		Textures::LoadTexture("Heal", "Heal.png"); // Also used in other scenes
		Textures::LoadTexture("HealK", "HealK.png"); // Also used in other scenes
		Textures::LoadTexture("HealC", "HealC.png"); // Also used in other scenes
		Textures::LoadTexture("HealTutorialMask", "HealTutorialMask.png");

		// Minimap
		Textures::LoadTexture("PlayerMinimapSprite", "PlayerMinimapSprite.png"); // Also used in other scenes
		Textures::LoadTexture("EnemyMinimapSprite", "EnemyMinimapSprite.png"); // Also used in other scenes
		Textures::LoadTexture("MinimapTutorialMask", "MinimapTutorialMask.png");
		Textures::LoadTexture("TutorialMinimap", "TutorialMinimap.png");

		// Tutorial Missions
		Textures::LoadTexture("TutorialMission1", "TutorialMission1.png");
		Textures::LoadTexture("TutorialMission2", "TutorialMission2.png");
		Textures::LoadTexture("TutorialMission3", "TutorialMission3.png");
		Textures::LoadTexture("TutorialMission4", "TutorialMission4.png");

		// Loading Bar (used in tutorial as mission progress bar)
		Textures::LoadTexture("LoadingBar", "LoadingBar.png");
		Textures::LoadTexture("LoadingBarFrame", "LoadingBarFrame.png");

		// HP bar
		Textures::LoadTexture("HPBar", "HPBar.png"); // Also used in other scenes
		Textures::LoadTexture("HPBarFrame", "HPBarFrame.png"); // Also used in other scenes
		Textures::LoadTexture("DamageOverlay", "DamageOverlay.png"); // Also used in other scenes

		// Stamina bar
		Textures::LoadTexture("STBar", "STBar.png"); // Also used in other scenes
		Textures::LoadTexture("STBarFrame", "STBarFrame.png"); // Also used in other scenes
		Textures::LoadTexture("LowStaminaWarning", "LowStaminaWarning.png"); // Also used in other scenes
		Textures::LoadTexture("StaminaTutorialMask", "StaminaTutorialMask.png");

		Textures::LoadTexture("BlackScreen", "BlackScreen.png"); // Also used in other scenes

		// 3D objects
		// skydome carries over from last loading case
		// landscape2 carries over from last loading case
		OBJLoader::LoadModelFromFile("TutorialStage", "TutorialStage/TutorialStage.obj", false);
		OBJLoader::LoadModelFromFile("Yuka", "yuka/yuka.obj", false); // Also used in other scenes
		OBJLoader::LoadModelFromFile("RedDoor", "DoorRed/DoorRed.obj", false);

		// FBX Objects
		FBXManager::LoadModelFromFile("Player", "Player/Player.fbx", false);
		FBXManager::LoadModelFromFile("Enemy", "EnemyHuman/EnemyHuman.fbx", false);

		// Animation Frame Start-End
		// For Player (Also used in main game)
		FBXManager::AddToAnimationList("Player", "Standing", 0, 359);
		FBXManager::AddToAnimationList("Player", "Walking", 361, 416);
		FBXManager::AddToAnimationList("Player", "Running", 418, 447);
		FBXManager::AddToAnimationList("Player", "StrafeLeft", 449, 487);
		FBXManager::AddToAnimationList("Player", "StrafeRight", 489, 527);
		FBXManager::AddToAnimationList("Player", "StrafeBack", 529, 566);
		FBXManager::AddToAnimationList("Player", "ComboAttack", 568, 818);
		FBXManager::AddToAnimationList("Player", "Injured", 820, 924);
		FBXManager::AddToAnimationList("Player", "DodgeRoll", 926, 998);
		FBXManager::AddToAnimationList("Player", "Dead", 1000, 1139);
		FBXManager::AddToAnimationList("Player", "Heal", 1141, 1299);

		// For Enemy (Also used in main game)
		FBXManager::AddToAnimationList("Enemy", "Standing", 0, 359);
		FBXManager::AddToAnimationList("Enemy", "Walking", 361, 443);
		FBXManager::AddToAnimationList("Enemy", "Running", 445, 474);
		FBXManager::AddToAnimationList("Enemy", "Attack", 476, 623);
		FBXManager::AddToAnimationList("Enemy", "Damaged", 625, 729);
		FBXManager::AddToAnimationList("Enemy", "Dead", 731, 946);
		FBXManager::AddToAnimationList("Enemy", "Jump", 948, 1027);
		FBXManager::AddToAnimationList("Enemy", "ParticleAttack", 1029, 1189);
		FBXManager::AddToAnimationList("Enemy", "InjureRun", 1191, 1226);
		FBXManager::AddToAnimationList("Enemy", "HardLanding", 1228, 1346);

		break;
	case MAINGAME:
		// 2D Sprite Loading
		// All HP Bar assets are carried over from Tutorial

		// All ST Bar assets are carried over from Tutorial

		// Mission
		Textures::LoadTexture("Mission1", "Mission1.png");

		// Minimap
		Textures::LoadTexture("BaseAreaMinimap", "BaseAreaMinimap.png");
		// Player minimap sprite carried over
		// Enemy minimap sprite carried over

		// Black Screen carried over

		// Enemy HP Bar
		Textures::LoadTexture("EnemyHumanHPBar", "EnemyHumanHPBar.png");
		Textures::LoadTexture("EnemyHumanHPBarFrame", "EnemyHumanHPBarFrame.png");

		// All heal assets carried over from tutorial

		// 3D Objects
		// skydome carries over
		// landscape carries over
		// tutorial stage carries over
		// ground carries over from title
		// yuka carries over

		// FBX Objects
		// Player FBX carries over
		// Enemy FBX carries over
		
		// All animations carry over

		break;
	case GAMECLEAR:
		// 2D Sprite loading
		Textures::LoadTexture("GameClearScreen", "p4.png");
		
		// All other assets carried over

		break;
	case GAMEOVER:
		// 2D Sprite loading
		Textures::LoadTexture("GameOverScreen", "p3.png");

		// All other assets carried over

		break;
	case FINISHED:
		finishedLoadingAssets = true;

		break;
	default:
		finishedLoadingAssets = true;

		break;
	}
}

void GameFramework::PipelineCreation()
{
	PipelineStatus::CreatePipeline("FBX", FBX);
	PipelineStatus::CreatePipeline("Particle", PARTICLE);
	PipelineStatus::CreatePipeline("Sprite", SPRITE);
	PipelineStatus::CreatePipeline("BasePostEffect", BasePostEffect);
	PipelineStatus::CreatePipeline("BasicObj", BasicObj);
	PipelineStatus::CreatePipeline("NormalMap", NormalMap);
	PipelineStatus::CreatePipeline("NormalShadowMap", NormalShadowMap);
	PipelineStatus::CreatePipeline("FBXShadowMap", FBXShadowMap);
	PipelineStatus::CreatePipeline("DrawShadowOBJ", DrawShadowOBJ);
	PipelineStatus::CreatePipeline("GPUParticle", GPUPARTICLE);
	PipelineStatus::CreatePipeline("SSAO", Ssao);
	PipelineStatus::CreatePipeline("SSAOCombine", SsaoCombine);
	PipelineStatus::CreatePipeline("SkyDome", SkyDome);

	finishedPipelineCreation = true;
}

void GameFramework::PostLoading()
{
	Input::Initialize(gameWindow->GetHwnd());

	CollisionManager::GetInstance()->Initialize(Vector3{ -70.0f, -10.0f, -70.0f } + 100.0f, Vector3{ 70.0f, 50.0f, 70.0f } + 100.0f);

	// Light Camera
	lightCamera = std::make_unique<LightCamera>();
	lightCamera->SetDistance(100);
	lightCamera->SetLightDir({ dir[0], dir[1], dir[2] });
	Object3d::SetLightCamera(lightCamera.get());

	// Scene Manager
	sceneManager = SceneManager::GetInstance();
	sceneManager->Initialize();
	sceneManager->AddScene(Scene::SCENE::Title, new TitleScreen());
	sceneManager->AddScene(Scene::SCENE::Tutorial, new TutorialArea());
	sceneManager->AddScene(Scene::SCENE::MainGame, new BaseArea());
	sceneManager->AddScene(Scene::SCENE::GameClear, new GameClearCutscene());
	sceneManager->AddScene(Scene::SCENE::GameOver, new GameOverCutscene());
	sceneManager->ChangeScene(Scene::SCENE::Title);

	mainResource = std::make_unique<TextureResource>("mainTex");
	TextureResource::SetMainResource(mainResource.get());

	shadowMap.reset(new TextureResource("shadowMap", { 1920, 1080 }, DXGI_FORMAT_R32_FLOAT, { 0, 0, 0, 0 }));
	depthResource.reset(new TextureResource("depthTex", { 1920,1080 }, DXGI_FORMAT_R32_FLOAT, { 1,1,1,1 }, false));
	halfDepthResource.reset(new TextureResource("halfDepthTex", { 1920 / 2,1080 / 2 }, DXGI_FORMAT_R32_FLOAT));
	normalResource = std::make_unique<TextureResource>("normalTex", false);
	halfNormalResource.reset(new TextureResource("halfNormalTex", { 1920 / 2,1080 / 2 }));
	ssaoResource.reset(new TextureResource("SSAO", { 1920 / 2,1080 / 2 }, DXGI_FORMAT_R32_FLOAT, { 1,0,0,0 }));
	ssao = std::make_unique<SSAO>();
	ssaoCombination = std::make_unique<SSAOCombination>();

	depthTex = std::make_unique<Sprite>();
	normalTex = std::make_unique<Sprite>();

	loading = false;
}

void GameFramework::DrawLoadTex()
{
	directX->ClearDepth();

	/*loadTex->DrawSprite("LoadPicture", { 0,0 }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	loadDot->SpriteSetTextureRect("LoadDot", 0, 0, 42.0f * (createPipelineLevel % 8), 25);
	loadDot->DrawSprite("LoadDot", { 1560,1010 }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });*/
}

void GameFramework::Initialize()
{
	gameWindow->CreateWidow(GameWindow::WINDOW);

	directX->Initialize(gameWindow.get());

	FbxLoader::GetInstance()->Initialize();
	Sprite::StaticInitialize(gameWindow.get());
	Audio::Initialize();
	ShowCursor(false);
	loading = true;

	ShadingComputation::StaticInitialize();

	DebugText::Initialize();

	loadTex = std::make_unique<Sprite>();
	loadDot = std::make_unique<Sprite>();
}

void GameFramework::Run()
{
	MSG msg{}; // メッセージ

	while (true)
	{
		// メッセージがある?
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg); // キー入力メッセージの処理
			DispatchMessage(&msg); // プロシージャにメッセージを送る
		}

		// 終了メッセージが来たらループを抜ける
		if (msg.message == WM_QUIT) {
			break;
		}

		if (loading)
		{
			//アセットロード
			AssetLoading();
			//パイプラインの生成
			PipelineCreation();

			if (finishedLoadingAssets && finishedPipelineCreation)
			{
				PostLoading();
			}

			directX->BeginDraw();
			directX->ImguiDraw();

			DrawLoadTex();
		}
		else if (!loading)
		{
			Object::SetBbIndex();
			TextureResource::SetBbIndex();
			Sprite::SetBbIndex();
			Input::Update();
			lightCamera->Update();
			Object3d::ClucLightViewProjection();
			sceneManager->Update();
			ParticleEmitter::Update();
			directX->BeginWrapperComputation();
			//2.画面クリアコマンドここまで
			Object3d::SetDrawShadow(true);
			shadowMap->PreDraw();
			directX->ImguiDraw();
			sceneManager->PreDraw();
			//directX->DepthClear();
			shadowMap->PostDraw(false);
			Object3d::SetDrawShadow(false);

			//3.描画コマンドここから
			if (SettingParameters::GetOnSSAO())
			{
				mainResource->PreDraw();
				normalResource->PreDraw(2);
				depthResource->PreDraw(3);
			}
			else
			{
				directX->BeginDraw();
			}

			sceneManager->PreDraw();
			CollisionManager::GetInstance()->DrawCollider();
			if (!SettingParameters::GetOnSSAO())
			{
				directX->ClearDepth();
			}
			//背面描画ここまで

			DebugText::Draw();

			sceneManager->PostDraw();
			ParticleEmitter::Draw();

			if (SettingParameters::GetOnSSAO())
			{
				depthResource->PostDraw(false);
				normalResource->PostDraw(false);
				mainResource->PostDraw();
				halfDepthResource->PreDraw(1, 0, 0, 1920 / 2, 1080 / 2);
				depthTex->DrawSprite("depthTex" + std::to_string(directX->GetBbIndex()), { 0,0 }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 }, "DepthSprite");
				halfDepthResource->PostDraw();
				halfNormalResource->PreDraw(1, 0, 0, 1920 / 2, 1080 / 2);
				normalTex->DrawSprite("normalTex" + std::to_string(directX->GetBbIndex()), { 0,0 }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 }, "NoAlphaToCoverageSprite");
				halfNormalResource->PostDraw();
				ssaoResource->PreDraw(1, 0, 0, 1920 / 2, 1080 / 2);
				ssao->Draw();
				ssaoResource->PostDraw();
				directX->BeginDraw();
				ssaoCombination->Draw();
			}
		}
		directX->EndDraw();
	}
}

void GameFramework::End()
{
	while (1)
	{
		if (ShowCursor(true) >= 0)
			break;
	}

	directX->ShutDown();

	computationWrapper->End();
	ParticleEmitter::ShutDown();
	DebugText::ShutDown();

	FbxLoader::GetInstance()->Finalize();
	FBXManager::ModelDeletion();

	//デリートはここまでに終わらせる
	gameWindow->End();
}
