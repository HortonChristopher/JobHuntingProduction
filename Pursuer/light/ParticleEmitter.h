#pragma once

#include <random>

#include "ParticleManager.h"
#include "Camera.h"
#include "Matrix.h"

class ParticleEmitter
{
private:
	static ParticleManager* particleManager;
public:
	static void Initialize(Camera* camera);

	static void CreateExplosion(const Vector3& pos);

	static void CreateShock(const Vector3& pos, const Vector3& arg_rotation = { XM_PI * 0.5f,0,0 });

	static void CreateParryEffect(const Vector3& pos);

	static void CreateWalkDust(const Vector3& pos, const Vector3& direction);

	static void CreateRunDust(const Vector3& pos, const Vector3& direction);

	static void Draw();

	static void Update();

	static void ShutDown();
};