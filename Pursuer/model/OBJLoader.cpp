#include "OBJLoader.h"

std::unordered_map<std::string, std::unique_ptr<Model>> OBJLoader::models = {};

void OBJLoader::LoadModelFromFile(const std::string& keyName, const std::string& fileName, const bool& smooth)
{
	Model* model = Model::CreateModel(fileName, smooth);

	models[keyName].reset(model);
}