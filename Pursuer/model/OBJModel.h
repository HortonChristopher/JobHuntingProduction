#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Mesh.h"
#include "Model.h"
#include "Textures.h"
#include "DirectXCommon.h"

using namespace std;

class OBJModel : public Model
{
public:
	OBJModel() = default;

	virtual ~OBJModel() {};

	virtual void Draw() = 0;
};