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