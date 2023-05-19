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
	case TITLE:
		break;
	}
}