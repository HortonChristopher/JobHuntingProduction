#pragma once

#include "fbxsdk.h"
#include "FBX3DModel.h"
#include "Textures.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>
#include <cassert>

using namespace DirectX;

class FbxLoader
{
private:
	// std abbreviation
	using string = std::string;

public:
	// Model Storage Route Bus
	static const string baseDirectory;

	// Material Directory Route
	static const string materialDirectory;

	// Standard texture file name when there is no texture
	static const string defaultTextureFileName;

public:
	/// <summary>
	/// Obtaining a singleton instance
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	/// <summary>
	/// Convert FBX matrix to XMMatrix
	/// </summary>
	/// <param name="dst">Write destination</param>
	/// <param name="src">Original FBX matrix</param>
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);

	/// <summary>
	/// Initialization
	/// </summary>
	void Initialize();

	/// <summary>
	/// Clean Up
	/// </summary>
	void Finalize();

	/// <summary>
	/// Load FBX model from file
	/// </summary>
	/// <param name="modelName">Model Name</param>
	//void LoadModelFromFile(const string& modelName);
	FBX3DModel* LoadModelFromFile(const std::string& modelName);

	// Read Skinning Information
	void ParseSkin(FBX3DModel* model, FbxMesh* fbxMesh);
private:
	/// <summary>
	/// Recursively analyze node configuration
	/// </summary>
	/// <param name="model">Import destination model object</param>
	/// <param name="fbxNode">Nodes to be analyzed</param>
	void ParseNodeRecursive(FBX3DModel* model, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// Reading of mesh
	/// </summary>
	/// <param name="model">Imported Model Object</param>
	/// <param name="fbxNode">Node to be analyzed</param>
	void ParseMesh(FBX3DModel* model, FbxNode* fbxNode);

	// Vertex coordinate reading
	void ParseMeshVertices(FBX3DModel* model, FbxMesh* fbxMesh);

	// Surface information reading
	void ParseMeshFaces(FBX3DModel* model, FbxMesh* fbxMesh);

	// Material reading
	void ParseMaterial(FBX3DModel* model, FbxNode* fbxNode);

	// Texture reading
	void LoadTexture(const std::string& fullpath);

	// Extract file names from file paths containing directories
	std::string ExtractFileName(const std::string& path);

	// Private constructor (Singleton pattern)
	FbxLoader() = default;
	// Private destructor (Singleton pattern)
	~FbxLoader() = default;
	// Prohibit copy constructor (Singleton pattern)
	FbxLoader(const FbxLoader& obj) = delete;
	// Prohibit copy assignment operator (Singleton pattern)
	void operator=(const FbxLoader& obj) = delete;

	// D3D12Device
	ID3D12Device* device = nullptr;
	// FBXManager
	FbxManager* fbxManager = nullptr;
	// FBXImporter
	FbxImporter* fbxImporter = nullptr;
};