#pragma once

#include "DeltaTime.h"
#include "Model.h"
#include "DirectXCommon.h"
#include "Matrix.h"
#include "FbxLoader/FbxLoader.h"
#include "Object3d.h"
#include "Textures.h"
#include "Quaternion.h"

#include <string>
#include <vector>
#include <array>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <fbxsdk.h>
#include <unordered_map>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

struct Node
{
	// Name
	std::string name;
	// Local Scale
	DirectX::XMVECTOR scaling = { 1, 1, 1, 0 };
	// Local Rotation
	DirectX::XMVECTOR rotation = { 0, 0, 0, 0 };
	// Local Move/Translation
	DirectX::XMVECTOR translation = { 0, 0, 0, 1 };
	// Local transformation matrix
	DirectX::XMMATRIX transform;
	// Global transformation matrix
	DirectX::XMMATRIX globalTransform;
	// Parent Node
	Node* parent = nullptr;
};

class FBX3DModel : public Model
{
private: // Alias
	// Using Microsoft::WRL
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// Using DirectX::
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;

	// Using std::
	using string = std::string;
	template <class T> using vector = std::vector<T>;
public: // Constant
	// Friend class
	friend class FbxLoader;

	// Bone structure
	struct Bone
	{
		// Name
		std::string name;

		// Inverse matrix of initial posture
		DirectX::XMMATRIX invInitialPose;

		// Cluster (FBX example channel information)
		FbxCluster* fbxCluster;

		// Constructor
		Bone(const std::string& name)
		{
			this->name = name;
		}
	};

	enum FrameSpeed
	{
		NORMAL = 1,
		HALF = 2,
		DOUBLE = 3,
		ONEPOINTFIVE = 4,
		POINTSEVENFIVE = 5
	};

	FrameSpeed frameSpeed = NORMAL;

	// Maximum number of bone instances
	/*static const int MAX_BONE_INDICES = 4;

	static const int MAX_BONES = 256;

	struct ConstBufferDataSkin
	{
		std::array<XMMATRIX, MAX_BONES> bones;
	};*/
private:
	void CreateBuffers();

	struct AnimationTime
	{
		FbxTime startTime;
		FbxTime endTime;
	};

	std::unordered_map<std::string, AnimationTime > animations;
	// Model name
	string name;
	// Node array
	vector<Node> nodes;
	// Ambient coefficient
	XMFLOAT3 ambient = { 1,1,1 };
	// Diffuse coefficient
	XMFLOAT3 diffuse = { 1,1,1 };
	// Scratch image
	ScratchImage scratchImg = {};
	// Vertex buffer
	ComPtr<ID3D12Resource>vertBuff;
	// Index buffer
	ComPtr<ID3D12Resource> indexBuff;
	// Vertex buffer view
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// Index buffer view
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	// Texture name
	string texName;
	// Bone Array
	std::vector<Bone> bones;
	// FBX scene
	FbxScene* fbxScene = nullptr;
public: // Member function
	~FBX3DModel();
	// Drawing
	void Draw() override;
	// Getter
	std::vector <Bone>& GetBones() { return bones; }
	// Maximum number of bone indices
	static const int MAX_BONE_INDICES = 4;
	// Obtaining the deformation matrix of the model
	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }
	// Getter
	FbxScene* GetFbxScene() { return fbxScene; }
	// Maximum number of bones
	static const int MAX_BONES = 32;
	struct ConstBufferDataSkin
	{
		std::array<XMMATRIX, MAX_BONES> bones;
	};
	void AddAnimation(const std::string& animationName, const int startFrame, const int endFrame);
	const AnimationTime& GetAnimation(const std::string& animationName);

	/// <summary>
	/// Start of animation
	/// </summary>
	void AnimationInit();

	void SetAnimationFrame(const int startFrame, const int endFrame, const int frameTime = 1);
	void SetAnimation(const std::string& animationName, const int frameTime = 1);

	bool PlayAnimation(bool endless = false);

	inline const FbxTime& GetCurrentAnimationTime() { return currentTime; }
private:
	ComPtr<ID3D12Resource> constBuffSkin; // Constant buffer
	// Time per frame
	FbxTime frameTime;
	// Animation start time
	FbxTime startTime;
	// Animation end time
	FbxTime endTime;
	// Current time (animation)
	FbxTime currentTime;
	// Current animation name
	std::string nowAnimationName;
	std::string lerpAnimationName;
	// Animation playing
	bool isPlay = false;
	float motionBlendTime = 7.0f;
	float lerpTime = 0.0f;

	// EvaluateGlobalTransform function is slow, so two bone arrays for motion blending.
	std::array<FbxAMatrix, MAX_BONES> blendStartBorn;
	std::array<FbxAMatrix, MAX_BONES> blendEndBorn;
public: // Subclass
	// Vertex data structure
	struct VertexPosNormalUvSkin
	{
		XMFLOAT3 pos; // Position
		XMFLOAT3 normal; // Normal vector
		XMFLOAT2 uv; // uv coordinates
		std::array<UINT, MAX_BONE_INDICES> boneIndex; // Number
		std::array<float, MAX_BONE_INDICES> boneWeight; // Weight
	};

	// Node with mesh
	Node* meshNode = nullptr;
	// Vertex data array
	vector<VertexPosNormalUvSkin> vertices;
	// Vertex index array
	vector<unsigned short>indices;
};