#pragma once

#include "FBX3DModel.h"
#include "Camera.h"
#include "input/Input.h"
#include "Vector.h"
#include "Easing.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include <array>
#include <vector>

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
	static const int MAX_BONES = 256;

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
	enum status
	{
		STAND = 1,
		WALK = 2,
		RUN = 3,
		STRAFEL = 4,
		STRAFER = 5,
		STRAFEB = 6,
		DODGE = 7,
		ATTACK = 8,
		DAMAGED = 9,
		DEAD = 10,
		HEAL = 11
	};

	status enumStatus = STAND;

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
	void SetEnumStatus(status enumStatus) { this->enumStatus = enumStatus; }

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
	XMFLOAT3 scale = { 0.075f, 0.075f, 0.075f };
	// Local Rotation
	XMFLOAT3 rotation = { 0,0,0 };
	// Local transformation
	XMFLOAT3 position = { 0.0f, 20.0f, -950.0f };
	// Local World matrix
	XMMATRIX matWorld;
	// Model
	FBX3DModel* model = nullptr;

	Vector3 direction = { 0, 0, 1 };
	Vector3 moveDirection = {};

	FBX3DModel* modelPlayer = nullptr; // Combined Animation Model

	// If animations can be combined into one FBX file, this becomes redundant and should be deleted
	// Stand(0), Walk(1), Run(2), Strafe Left(3), Strafe Right(4), Strafe Back(5),
	// DodgeRoll(6), Attack(7), Damaged(8), Death(9)
	int animationNo = 0;
	bool animationSet = false;
	bool modelChange = false;

	// 1 frame time
	FbxTime frameTime;

	// Animation start time
	FbxTime startTime;
	int startFrame = 0;

	// Animation end time
	FbxTime endTime;
	int endFrame = 0;

	// Animation current time
	FbxTime currentTime;

	// Animation is playing
	bool isPlay = false;
	bool repeatAnimation = false;

	float rotateSpeed = 1350.0f; // Currently using delta time; 22.5f using frame speed
	float speed = 40.0f; // Currently using delta time; 1.0f using frame speed
	float sprintSpeed = 80.0f; // 1.5f using frame speed
	float rollSpeed = 45.0f; // 0.75 using frame speed
	float debugTimer = 0.0f;
	float dodgeCameraTime = 0.0f;
	bool movementAllowed = false;
	XMFLOAT3 dodgeStartPosition = { 0.0f, 10.0f, 0.0f };
public:
	XMFLOAT3 dodgePosition = { 0.0f, 10.0f, 0.0f };
	bool dodge = false;
	bool isPlayerDead = false;
	bool baseAreaOpeningCutscene = true;
	float stamina = 100.0f;
	float hp = 25.0f; // Base of 25.0f
	int attackCombo = 0; // Which attack the animation stops at
	int healRemaining = 4;
	bool healed = false;
	std::array<XMFLOAT3, 3> healParticlePosition = { {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }};
	bool ableToDamage = false;
	float timer = 0.0f;
	bool damageAdvantage = false;
};