#pragma once

#include "FBX3DModel.h"
#include "FBXGeneration.h"
#include "Camera.h"
#include "input/Input.h"
#include "DeltaTime.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

class EnemyHuman
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
		WANDER = 2,
		AGGRO = 3,
		ATTACK = 4,
		COOLDOWN = 5,
		DAMAGED = 6,
		DEAD = 7,
		PARTICLEATTACK = 8,
		FLEE = 9,
		LANDINGATTACK = 10,
		CHARGEATTACK = 11,
		TWOENEMYSURROUND = 12,
		BACKATTACK = 13,
		JETSTREAMATTACK = 14
	};

	status enumStatus = STAND;

	enum patrol
	{
		FRONT = 1,
		BACK = 2,
		DEFAULT = 3
	};

	patrol patrolStatus = DEFAULT;

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
	void SetHomePosition(XMFLOAT2 position) { this->homePosition = position; }
	void SetAttack(bool attack) { this->attack = attack; }
	void SetAggro(bool aggro) { this->aggro = aggro; }
	void SetAggroSwitch(bool aggroswitch) { this->aggroSwitch = aggroswitch; }
	void SetPatrolPosition(XMFLOAT3 position) { this->frontPatrolPosition = position; }
	void SetEnumStatus(status enumStatus) { this->enumStatus = enumStatus; }
	void SetPatrolStatus(patrol patrolStatus) { this->patrolStatus = patrolStatus; }
	void SetTimer(float Btimer) { this->timer = Btimer; }

	void Reset();

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
	static void SetDevice(ID3D12Device* device) { EnemyHuman::device = device; }
	static void SetCamera(Camera* camera) { EnemyHuman::camera = camera; }

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
	//DeltaTime* deltaTime = nullptr;

	// Local scale
	XMFLOAT3 scale = { 0.15f, 0.15f, 0.15f };
	// Local Rotation
	XMFLOAT3 rotation = { 0,0,0 };
	// Local transformation
	XMFLOAT3 position = { 0,-10,0 };
	XMFLOAT3 newPosition = { 0,0,0 };
	// Local World matrix
	XMMATRIX matWorld;
	// Model
	FBX3DModel* model = nullptr;

	XMFLOAT2 homePosition = { 0, 0 };

	bool aggro = false;
	bool aggroSwitch = false;

	bool attack = false;
	bool attackAnimation = false;

	bool FirstRun = true;

	FBX3DModel* modelEnemy = nullptr; // Combined Animation Model

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
	                                                                
	// If animations can be combined into one FBX file, this becomes redundant and should be deleted
	int animationNo = 0; // Stand(0), Walk(1), Run(2), Attack(3), Damaged(4), Death(5), JumpBack(6), ParticleAttack(7)
	bool animationSet = false;

	float degrees = 0.0f;
	float x = 0.0f;
	float x2 = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float hypotenuse = 0.0f;
	float hypotenuse2 = 0.0f;
	float radians = 0.0f;

	int xQuadrant = 0;
	int yQuadrant = 0;

	float endAnimationDEBUG = 0.0f;

	XMFLOAT3 cooldownPosition = { 0.0f, 0.0f, 0.0f };
public:
	XMFLOAT3 particleAttackPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 landingAttackPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 frontPatrolPosition = { 0.0f, 0.0f, 0.0f };
	float HP = 5.0f; // Default 10.0f
	bool dead = false;
	bool aggroSet = false;
	bool modelChange = false;
	bool wander = false;
	bool attackDamagePossible = false;
	bool ableToDamage = true;
	bool set = false;
	bool particleAttackActive = false;
	float attackTimer = 0.0f;
	float timer = 179.0f;
	bool fleeSet = false;
	bool helpCall = false;
	int closestEnemy = 0;
	int particleAttackStage = 0;
	int landingAttackStage = 0;
	int chargeAttackStage = 0;
	int jetStreamAttackStage = 0;
	int twoEnemySurroundStage = 0;
	int agrooNumber = 0;
	bool landed = false;
	bool landingParticles = false;
	bool chargeAttackCheck = false;
	bool debugJetAttacked = false;
	bool jetStreamCounted = false;
	bool isPartnerDead = false;
};