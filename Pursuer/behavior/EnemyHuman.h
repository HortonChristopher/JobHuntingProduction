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

	enum FrameSpeed
	{
		NORMAL = 1,
		HALF = 2,
		DOUBLE = 3,
		ONEPOINTFIVE = 4,
		POINTSEVENFIVE = 5,
		POINTTWOFIVE = 6,
	};

	FrameSpeed frameSpeed = NORMAL;

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

	bool CheckCollisionWithEnemy(XMFLOAT3 enemyPos, float collisionRadius);

	void MoveTowards(float& current, float target, float speed, float elapsedTime);

	bool IsEnemyWandering()
	{
		return enumStatus == EnemyHuman::WANDER;
	}

	bool IsEnemyStanding()
	{
		return enumStatus == EnemyHuman::STAND;
	}

	bool IsEnemyAggro()
	{
		return enumStatus == EnemyHuman::AGGRO;
	}

	bool IsEnemyAttacking()
	{
		return enumStatus == EnemyHuman::CHARGEATTACK || enumStatus == EnemyHuman::JETSTREAMATTACK || enumStatus == EnemyHuman::LANDINGATTACK || enumStatus == EnemyHuman::PARTICLEATTACK || enumStatus == EnemyHuman::ATTACK;
	}

	bool IsEnemyCoolingDown()
	{
		return enumStatus == EnemyHuman::COOLDOWN;
	}

	bool ShouldEnemyFlee(const float fleeHP, bool enemyKnockback)
	{
		if (!enemyKnockback && enumStatus != EnemyHuman::DAMAGED && !helpCall && enumStatus != EnemyHuman::DEAD)
		{
			return (HP <= fleeHP) || isPartnerDead;
		}

		return false;
	}

	bool CanEnemyBeJetStreamAttacked()
	{
		return !IsEnemyStanding(enemy->enumStatus) &&
			IsEnemyAlive(enemy->enumStatus) &&
			!IsEnemyFleeing(enemy->enumStatus) &&
			!IsEnemyWandering(enemy->enumStatus) &&
			!IsEnemyCoolingDown(enemy->enumStatus) &&
			CanEnemyJetStreamAttack(enemy->enumStatus, enemy->debugJetAttacked);
	}

	bool CanSeePlayer(int intersect, bool enemyKnockback, bool enemyAlive)
	{
		return (intersect && !enemyKnockback && enemyAlive) || (enumStatus == EnemyHuman::AGGRO && !enemyKnockback && enemyAlive);
	}

	bool CurrentlyParticleAttackingNoStage()
	{
		return particleAttackActive;
	}

	bool HasHitGroundDuringLandingAttack()
	{
		return landingParticles;
	}

	bool IsChargeAttacking()
	{
		return enumStatus == EnemyHuman::CHARGEATTACK && chargeAttackStage == 1;
	}

	bool IsJetStreamAttacking()
	{
		return enumStatus == EnemyHuman::JETSTREAMATTACK && jetStreamAttackStage == 2;
	}

	bool IsSurroundAttacking()
	{
		return enumStatus == EnemyHuman::TWOENEMYSURROUND && twoEnemySurroundStage == 1;
	}

	bool IsFrontPatrolPosition()
	{
		return patrolStatus == EnemyHuman::FRONT;
	}

	bool IsBackPatrolPosition()
	{
		return patrolStatus == EnemyHuman::BACK;
	}

	bool DidEnemyHitPlayerWithNormalAttack(bool intersect, bool attackDamagePossible)
	{
		return intersect && attackDamagePossible;
	}

	bool DidEnemyHitPlayerWithParticleAttack(bool intersect, bool particleAttackActive)
	{
		return intersect && particleAttackActive;
	}

	bool DidEnemyHitPlayerDuringSurroundAttack(bool intersect, EnemyHuman::status enumStatus, int twoEnemySurroundStage)
	{
		return intersect && enumStatus == EnemyHuman::TWOENEMYSURROUND && twoEnemySurroundStage == 1;
	}

	bool DidEnemyHitPlayerDuringLandingAttack(bool intersect, bool landingParticles)
	{
		return intersect && landingParticles;
	}

	bool DidEnemyHitPlayerWhileCharging(bool intersect, EnemyHuman::status enumStatus, int chargeStage, int jetStreamStage)
	{
		if (intersect)
		{
			return (enumStatus == EnemyHuman::CHARGEATTACK && chargeStage == 1) || (enumStatus == EnemyHuman::JETSTREAMATTACK && jetStreamStage == 2);
		}

		return false;
	}

	bool IsEnemyAlive()
	{
		return enumStatus != EnemyHuman::DEAD;
	}

	bool IsPlayerDead()
	{
		return isPlayerDead;
	}

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
public:
	XMFLOAT3 cooldownPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 meetingPoint = { 0.0f, 0.0f, 0.0f };
	XMFLOAT2 nextPosition = { 0.0f, 0.0f };
	XMFLOAT2 midpoint = { 0.0f, 0.0f };
	XMFLOAT2 circleRadius = { 0.0f, 0.0f };
	float circleRadiusDecider = 0.0f;
	float nextDegree = 0.0f;
	float initialDegree = 0.0f;
	float surroundSpeed = 60.0f;
	float nextDegreeAngle = 0.0f;
	float distanceFloat = 0.0f;
	float origDistanceFloat = 0.0f;
	XMFLOAT3 particleAttackPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 landingAttackPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 frontPatrolPosition = { 0.0f, 0.0f, 0.0f };
	float HP = 5.0f; // Default 10.0f
	const float minAliveHP = 1.0f;
	bool dead = false;
	bool aggroSet = false;
	bool modelChange = false;
	bool wander = false;
	bool attackDamagePossible = false;
	bool ableToDamage = true;
	bool set = false;
	bool particleAttackActive = false;
	float attackTimer = 0.0f;
	const float parryPossibleMin = 43.0f;
	const float parryPossibleMax = 73.0f;
	float timer = 179.0f;
	bool fleeSet = false;
	bool helpCall = false;
	bool isBeingCalledToHelp = false;
	int closestEnemy = 0;
	int particleAttackStage = 0;
	int landingAttackStage = 0;
	int chargeAttackStage = 0;
	const int chargeAttackStageReset = 0;
	int jetStreamAttackStage = 0;
	int twoEnemySurroundStage = 0;
	int agrooNumber = 0;
	const int jetStreamAttackRequiredEnemyNumber = 2;
	const int jetStreamAttackStageReset = 0;
	const float timerReset = 0.0f;
	const int twoEnemySurroundStageReset = 0;
	bool landed = false;
	bool landingParticles = false;
	bool chargeAttackCheck = false;
	bool debugJetAttacked = false;
	bool jetStreamCounted = false;
	bool isPartnerDead = false;
	bool slowMotion = false;
	bool attackAnimation = false;
};