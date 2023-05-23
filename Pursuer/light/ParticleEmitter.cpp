#include "ParticleEmitter.h"

ParticleManager* ParticleEmitter::particleManager = nullptr;

void ParticleEmitter::Initialize(Camera* camera)
{
	particleManager = ParticleManager::GetInstance();

	particleManager->SetCamera(camera);

	particleManager->Initialize();
}

void ParticleEmitter::CreateExplosion(const Vector3& pos)
{
	for (int i = 0; i < 30; i++)
	{
		Particle* particle = new Particle();

		Vector3 position = pos;

		float alpha = 1.0f;

		Vector3 rotation = { 0,0,0 };

		particle->parameters.frame = 0;
		particle->parameter.num_frame = 60;

		Vector3 velocity = { 0,0,0 };
		particle->parameter.accel = { (std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f,(std::rand() % 100 - 50) * 0.01f };

		float scale = 1.0f;
		particle->parameter.s_scale = 1.0f;
		particle->parameter.e_scale = 5.0f;
		// Linear interpolation (A method of curve fitting using linear polynomials to construct new data points within the range of a discrete set of known data points)
		particle->parameters.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		Vector3 color = { 1,1,1 };
		particle->parameter.s_color = { 0,0,0 };
		particle->parameter.e_color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };
		particle->parameters.isDead = false;

		XMMATRIX mat = { position.x,position.y,position.z,0,
						rotation.x,rotation.y,rotation.z,0,
						scale,velocity.x,velocity.y ,velocity.z ,
						color.x,color.y,color.z,alpha };

		particle->parameters.position = position;
		particle->parameters.rotation = rotation;
		particle->parameters.velocity = velocity;
		particle->parameters.scale = scale;
		particle->parameters.color = { color.x,color.y,color.z,alpha };
		particle->parameters.billboardActive = 1;

		particleManager->Add(particle, "particle");
	}
}

void ParticleEmitter::CreateShock(const Vector3& pos, const Vector3& arg_rotation)
{
	for (int i = 0; i < 3; i++)
	{
		Particle* particle = new Particle();
		Vector3 position = pos;
		Vector3 rotation = arg_rotation;

		particle->parameters.frame = 0;
		particle->parameter.num_frame = 10 + 5 * i;

		Vector3 velocity = { 0,0,0 };
		particle->parameter.accel = { 0,0,0 };

		particle->parameter.s_rotation = rotation;
		particle->parameter.e_rotation = rotation;

		float scale = 1.0f;
		particle->parameter.s_scale = 1.0f;
		particle->parameter.e_scale = 3.0f;
		// Linear interpolation (A method of curve fitting using linear polynomials to construct new data points within the range of a discrete set of known data points)
		particle->parameters.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		Vector3 color = { (std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f),(std::rand() % 100 * 0.01f) };

		float alpha = 1.0f;

		particle->parameter.s_color = { 1,1,1 };
		particle->parameter.e_color = { 0,0,0 };
		particle->parameters.isDead = false;
		particle->parameters.position = position;
		particle->parameters.rotation = rotation;
		particle->parameters.velocity = velocity;
		particle->parameters.scale = scale;
		particle->parameters.color = { color.x,color.y,color.z,alpha };
		particle->parameters.billboardActive = 0;

		particleManager->Add(particle, "shock");
	}
}
void ParticleEmitter::CreateParryEffect(const Vector3& pos)
{
	for (int i = 0; i < 30; i++)
	{
		Particle* particle = new Particle();
		Vector3 position = pos;
		Vector3 rotation = { 0,0,0 };

		particle->parameters.frame = 0;
		particle->parameter.num_frame = 35;

		Vector3 velocity = { (std::rand() % 100 - 50) * 0.0005f,std::rand() % 100 * -0.001f,(std::rand() % 100 - 50) * 0.0005f };
		particle->parameter.accel = { 0,0.005f,0 };
		particle->parameter.s_rotation = { 0,0,std::rand() % 200 * 0.01f * (float)XM_PI };
		particle->parameter.e_rotation = { 0,0,std::rand() % 200 * 0.01f * (float)XM_PI };


		float scale = std::rand() % 100 * 0.01f;
		particle->parameter.s_scale = scale;
		particle->parameter.e_scale = 0.0f;
		// Linear interpolation (A method of curve fitting using linear polynomials to construct new data points within the range of a discrete set of known data points)
		particle->parameters.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		Vector3 color = { 0,1,0 };
		float alpha = 1.0f;

		particle->parameter.s_color = { 0,1,0 };
		particle->parameter.e_color = { 0,0,0 };
		particle->parameters.isDead = false;
		particle->parameters.position = position;
		particle->parameters.rotation = rotation;
		particle->parameters.velocity = velocity;
		particle->parameters.scale = scale;
		particle->parameters.color = { color.x,color.y,color.z,alpha };
		particle->parameters.billboardActive = 1;

		particleManager->Add(particle, "starEffect");
	}
}

void ParticleEmitter::CreateWalkDust(const Vector3& pos, const Vector3& direction)
{
	Vector3 dir = direction;

	if (dir.Length() == 0)
	{
		return;
	}
	Vector3 baseDirection = { 0,0,1 };

	int reversal = 1;

	if (dir.Dot({ 1,0,0 }) < 0)
	{
		baseDirection = { 0,0,-1 };
		reversal = -1;
	}
	const float dirRotY = acosf(dir.Dot(baseDirection));

	for (int i = 0; i < 2; i++)
	{
		Particle* particle = new Particle();
		Vector3 position = pos + Vector3{ (std::rand() % 100 - 50) * 0.003f, 0, (std::rand() % 100 - 50) * 0.003f };
		Vector3 rotation = { 0,0,0 };

		particle->parameters.frame = 0;
		particle->parameter.num_frame = 20;

		Vector3 velocity = { (std::rand() % 100 - 50) * 0.001f, -0.01f,(std::rand() % 100) * -0.0002f };
		velocity = reversal * velocity * XMMatrixRotationAxis({ 0,1,0 }, dirRotY);

		float scale = 0.25f;

		particle->parameter.accel = { 0,0.002f,0 };
		particle->parameter.s_rotation = { 0,0,0 };
		particle->parameter.e_rotation = { 0,0,0 };
		particle->parameter.s_scale = scale;
		particle->parameter.e_scale = scale * 10;
		// Linear interpolation (A method of curve fitting using linear polynomials to construct new data points within the range of a discrete set of known data points)
		particle->parameters.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		Vector3 color = 0.03f;

		float alpha = 0.01f;

		particle->parameter.s_color = color;
		particle->parameter.e_color = { 0,0,0 };
		particle->parameters.isDead = false;
		particle->parameters.position = position;
		particle->parameters.rotation = rotation;
		particle->parameters.velocity = velocity;
		particle->parameters.scale = scale;
		particle->parameters.color = { color.x,color.y,color.z,alpha };
		particle->parameters.billboardActive = 1;

		particleManager->Add(particle, "particle");
	}
}
void ParticleEmitter::CreateRunDust(const Vector3& pos, const Vector3& direction)
{
	Vector3 dir = direction;

	if (dir.Length() == 0)
	{
		return;
	}

	Vector3 baseDirection = { 0,0,1 };

	int reversal = 1;

	if (dir.Dot({ 1,0,0 }) < 0)
	{
		baseDirection = { 0,0,-1 };
		reversal = -1;
	}

	const float dirRotY = acosf(dir.Dot(baseDirection));

	for (int i = 0; i < 5; i++)
	{
		Particle* particle = new Particle();

		Vector3 position = pos + Vector3{ (std::rand() % 100 - 50) * 0.003f, 0, (std::rand() % 100 - 50) * 0.003f };
		Vector3 rotation = { 0,0,0 };

		particle->parameters.frame = 0;
		particle->parameter.num_frame = 20;

		Vector3 velocity = { (std::rand() % 100 - 50) * 0.001f, -0.01f,(std::rand() % 100) * -0.0002f };
		velocity = reversal * velocity * XMMatrixRotationAxis({ 0,1,0 }, dirRotY);

		float scale = 0.25f;

		particle->parameter.accel = { 0,0.002f,0 };
		particle->parameter.s_rotation = { 0,0,0 };
		particle->parameter.e_rotation = { 0,0,0 };
		particle->parameter.s_scale = scale;
		particle->parameter.e_scale = scale * 8;
		// Linear interpolation (A method of curve fitting using linear polynomials to construct new data points within the range of a discrete set of known data points)
		particle->parameters.scaleVel = (particle->parameter.e_scale - particle->parameter.s_scale) / particle->parameter.num_frame;

		Vector3 color = 0.03f;

		float alpha = 0.01f;

		particle->parameter.s_color = color;
		particle->parameter.e_color = { 0,0,0 };
		particle->parameters.isDead = false;
		particle->parameters.position = position;
		particle->parameters.rotation = rotation;
		particle->parameters.velocity = velocity;
		particle->parameters.scale = scale;
		particle->parameters.color = { color.x,color.y,color.z,alpha };
		particle->parameters.billboardActive = 1;

		particleManager->Add(particle, "particle");
	}
}

void ParticleEmitter::Draw()
{
	particleManager->Draw();
}

void ParticleEmitter::Update()
{
	particleManager->Update();
}

void ParticleEmitter::ShutDown()
{
	particleManager->ShutDown();
}
