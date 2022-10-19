#pragma once

#include "FBX3DModel.h"
#include "Camera.h"
#include "input/Input.h"
#include "Vector.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

class Player
{
protected: // Alias
	// using Microsoft::WRL
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// using DirectX::
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // Constant
	// Maximum Bone Number
	static const int MAX_BONES = 32;

public:
	// Data structure for constant buffer (for coordinate transformation matrix)
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj; // View projection line departure
		XMMATRIX world; // World matrix
		XMFLOAT3 cameraPos; // Camera coordinates (world coordinates)
	};

	// Data structure for constant buffer (skinning)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

public:
	/// <summary>
	/// Initialization
	/// </summary>
	void Initialize();

	/// <summary>
	/// Every Frame Processing
	/// </summary>
	void Update();

	/// <summary>
	/// Generate graphics pipeline
	/// </summary>
	void CreateGraphicsPipeline();

	/// <summary>
	/// Drawing
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// Setting model
	/// </summary>
	void SetModel(FBX3DModel* model) { this->model = model; }

	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }

	const XMFLOAT3& GetPosition() { return position; }
	const XMFLOAT3& GetRotation() { return rotation; }

	/// <summary>
	/// Animation Initialization
	/// </summary>
	void PlayAnimation();

protected:
	// Constant Buffer
	ComPtr<ID3D12Resource> constBuffTransform;

public:
	// setter
	static void SetDevice(ID3D12Device* device) { Player::device = device; }
	static void SetCamera(Camera* camera) { Player::camera = camera; }

	// Root signature
	static ComPtr<ID3D12RootSignature> rootsignature;
	// Pipeline state
	static ComPtr<ID3D12PipelineState> pipelinestate;

	// Constant Buffer (skinning)
	ComPtr<ID3D12Resource> constBuffSkin;

private:
	// Device
	static ID3D12Device* device;

	// Camera
	static Camera* camera;

protected:
	Input* input = nullptr;

	// Local scale
	XMFLOAT3 scale = { 1,1,1 };
	// Local Rotation
	XMFLOAT3 rotation = { 0,0,0 };
	// Local transformation
	XMFLOAT3 position = { 0,-10,-30 };
	// Local World matrix
	XMMATRIX matWorld;
	// Model
	FBX3DModel* model = nullptr;

	Vector3 direction = { 0, 0, 1 };

	FBX3DModel* modelStanding = nullptr; // 180 frames
	FBX3DModel* modelWalking = nullptr; // 30 frames
	FBX3DModel* modelRunning = nullptr; // 20(?) frames
	FBX3DModel* modelDodgeF = nullptr; // 30 frames
	FBX3DModel* modelDodgeB = nullptr; // 49 frames
	FBX3DModel* modelDodgeL = nullptr; // 29 frames
	FBX3DModel* modelDodgeR = nullptr; // 29 frames
	FBX3DModel* modelAttacking = nullptr; // 30(?) frames
	FBX3DModel* modelDamaged = nullptr; // 54 frames

	// If animations can be combined into one FBX file, this becomes redundant and should be deleted
	int animationNo = 0; // Stand(0), Walk(1), Run(2), DodgeF(3), DodgeB(4), DodgeL(5), DodgeR(6), Attack(7), Damaged(8)
	bool animationSet = false;
	bool modelChange = false;

	// 1 frame time
	FbxTime frameTime;

	// Animation start time
	FbxTime startTime;

	// Animation end time
	FbxTime endTime;

	// Animation current time
	FbxTime currentTime;

	// Animation is playing
	bool isPlay = false;

	float rotateSpeed = 7.5f;
	float speed = 60.0f; //1.0f using frame speed instead of delta speed
	int attackTime = 0;
};