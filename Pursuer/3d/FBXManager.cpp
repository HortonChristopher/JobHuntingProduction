#include "FBXManager.h"

std::unordered_map<std::string, FBX3DModel*> FBXManager::models = {};

void FBXManager::LoadModelFromFile(const std::string& keyName, const std::string& fileName, const bool& smooth)
{
	// If a model is already registered, exit without adding it.
	if (models[keyName] != nullptr)
	{
		return;
	}

	FBX3DModel* model = FbxLoader::GetInstance()->LoadModelFromFile(fileName);

	models[keyName] = model;
}

void FBXManager::ModelDeletion()
{
	for (auto& itr : models)
	{
		delete itr.second;
		itr.second = nullptr;
	}
}

void FBXManager::AddToAnimationList(const std::string& modelName, const std::string& animationName, const int startFrame, const int endFrame)
{
	models[modelName]->AddAnimation(animationName, startFrame, endFrame);
}

void FBXManager::AddToAnimationList(FBX3DModel* model, const std::string& animationName, const int startFrame, const int endFrame)
{
	model->AddAnimation(animationName, startFrame, endFrame);
}