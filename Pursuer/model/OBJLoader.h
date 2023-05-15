#pragma once

#include <unordered_map>
#include <string>

#include "Model.h"

class OBJLoader
{
public:
	static void LoadModelFromFile(const std::string& keyName, const std::string& fileName, const bool& smooth);

	static Model* GetModel(const std::string& keyName) {
		if (models[keyName] == nullptr)
		{
			assert(0);
		}

		return models[keyName].get();
	};
private:
	static std::unordered_map<std::string, std::unique_ptr<Model>> models;
};