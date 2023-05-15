#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "Mesh.h"
#include "OBJModel.h"
#include "Textures.h"
#include "DirectXCommon.h"

using namespace std;

// Model data
class Model
{
private: // Alias
	// Using Microsoft::WRL::
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// Using DirectX::
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // Static member function
	static Model* CreateModel(const std::string& name, bool smooth);

private: // Static member variable
	static const std::string baseDirectory;

public: // Member function
	~Model();

	void Initialize(const std::string& name, bool smooth);

	void Draw();

	void LoadMaterial(const std::string& name);

	void AddMaterial(Material* material);

	void LoadTextures();

	void CreateDescriptorHeap();

	// Get mesh container
	inline const std::vector<Mesh*>& GetMeshes() { return meshes; }

	// Retrieve material containers
	inline const std::unordered_map<std::string, Material*>& GetMaterials() { return materials; }
private: // Member variable
	std::string name;
	std::vector<Mesh*> meshes;
	std::unordered_map<std::string, Material*> materials;

	Material* defaultMaterial = nullptr;

	ComPtr<ID3D12DescriptorHeap> descHeap;
};

