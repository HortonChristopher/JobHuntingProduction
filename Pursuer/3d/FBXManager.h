#pragma once

#include <unordered_map>
#include <string>

#include "FBX3DModel.h"
#include "FbxLoader/FbxLoader.h"

class FBXManager
{
public:
	static void LoadModelFromFile(const std::string& keyName, const std::string& fileName, const bool& smooth);

	static void ModelDeletion();

	static void AddToAnimationList(const std::string& modelName, const std::string& animationName, const int startFrame, const int endFrame);

	static void AddToAnimationList(FBX3DModel* model, const std::string& animationName, const int startFrame, const int endFrame);

	static FBX3DModel* GetModel(const std::string& keyName) {
		if (models[keyName] == nullptr)
			assert(0);
		return models[keyName];
	};
private:
	static std::unordered_map<std::string, FBX3DModel*> models;
};