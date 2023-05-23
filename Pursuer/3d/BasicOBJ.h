#pragma once

#include<string>

#include "Object3D.h"
#include "Vector.h"
#include "CollisionInfo.h"
#include "SafeDelete.h"
#include "BaseCollider.h"
#include "CollisionManager.h"

class BaseCollider;
class BasicOBJ
{
protected:
	struct HitBox
	{
	public:
		void SetPosition(const Vector3& pos) { position = pos; }

		void SetScale(const Vector3& scale) { this->scale = scale; }

		const Vector3& GetPosition() { return position; }

		const Vector3& GetScale() { return scale; }
	private:
		Vector3 position;

		Vector3 scale;
	};

	// Box for judging a hit
	HitBox hitBox;

	static int bbIndex;
public:
	static void SetBbIndex()
	{
		bbIndex = DirectXCommon::GetInstance()->GetBbIndex();
		Object3d::SetBbIndex(bbIndex);
	}

	BasicOBJ() = default;

	virtual ~BasicOBJ();

	void Create(Model* model);

	virtual void Initialize();

	virtual void Update();

	virtual void ReadyToDraw();

	virtual void Draw();

	void CustomDraw(const bool fbx = false, const bool shade = false, BLENDING type = ALPHA, const bool customPipeline = false, const int lightRootParameterIndex = 3);

	inline  const std::string& GetPipelineName()
	{
		return pipelineName;
	}

	const XMMATRIX& GetMatWorld()
	{
		return object->GetMatWorld();
	}

	const XMMATRIX& GetRotateMat()
	{
		return object->GetRotateMatrix();
	}

	void SetCollider(BaseCollider* collider);

	virtual void OnCollision(const CollisionInfo& info) {}

	void SetPosition(const Vector3& pos) { position = pos; }

	inline const Vector3& GetPosition() { return position; }

	void SetScale(const Vector3& scale) { this->scale = scale; }

	inline const Vector3& GetScale() { return scale; }

	void SetRotation(const Vector3& rot) { rotation = rot; }

	inline const Vector3& GetRotation() { return rotation; }

	void SetColor(const XMFLOAT4& color) { this->color = color; }

	inline const XMFLOAT4& GetColor() { return color; }

	inline const bool IsDead() { return dead; }
protected:
	std::unique_ptr<Object3d> object;

	Vector3 position = { 0,0,0 };
	Vector3 scale = { 1,1,1 };
	Vector3 rotation = { 0,0,0 };
	Vector3 velocity = { 0,0,0 };
	Vector3 aceel = { 0,0,0 };
	XMFLOAT4 color = { 1,1,1,1 };

	std::string pipelineName = "BasicObj";

	bool dead = false;

	const char* name = nullptr;

	BaseCollider* collider = nullptr;
};